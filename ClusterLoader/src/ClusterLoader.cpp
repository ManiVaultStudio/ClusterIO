#include "ClusterLoader.h"

#include <PointData/PointData.h>
#include <ClusterData/ClusterData.h>

#include <QInputDialog>

#include <fstream>
#include <iterator>
#include <vector>

Q_PLUGIN_METADATA(IID "nl.tudelft.ClusterLoader")

using namespace mv;
using namespace mv::gui;

// Same as in Cluster Exporter
struct DataContent {
    DataContent() : numClusters(0), clusterSizes{}, clusterNames{}, clusterIDs{}, clusterColors{}, clusterIndices{}, parentName(""), parentNumPoints(0) {};

    uint32_t numClusters;
    std::vector<uint32_t> clusterSizes;
    std::vector<std::string> clusterNames;
    std::vector<std::string> clusterIDs;
    std::vector<int32_t> clusterColors;
    std::vector<uint32_t> clusterIndices;

    std::string parentName;
    uint32_t parentNumPoints;
};


// =============================================================================
// View
// =============================================================================

ClusterLoader::~ClusterLoader(void)
{

}

void ClusterLoader::init()
{

}

template <typename T>
void readVec(std::vector<T>& vec, std::ifstream& in)
{
    size_t length;
    in.read(reinterpret_cast<char*>(&length), sizeof(size_t));
    vec.resize(length);
    for (size_t i = 0; i < length; i++)
        in.read(reinterpret_cast<char*>(&vec[i]), sizeof(T));
}
template void readVec<int32_t>(std::vector<int32_t>& vec, std::ifstream& in);
template void readVec<uint32_t>(std::vector<uint32_t>& vec, std::ifstream& in);

void ClusterLoader::loadData()
{
    const QString fileName = AskForFileName(tr("BIN Files (*.bin)"));

    // Don't try to load a file if the dialog was cancelled or the file name is empty
    if (fileName.isNull() || fileName.isEmpty())
        return;

    qDebug() << "Loading Cluster file: " << fileName;

    // read in binary data, be sure to read in the same other as ClusterExporter::writeClusterDataToBinary wrote them
    DataContent dataContent;
    std::ifstream in(fileName.toStdString(), std::ios::in | std::ios::binary);
    if (in)
    {
        auto readString = [&in](std::string& s) -> void {
            size_t length;
            in.read(reinterpret_cast<char*>(&length), sizeof(size_t));
            s.resize(length);
            in.read(&s[0], length);
            };

        in.seekg(0, std::ios::beg);
        
        in.read(reinterpret_cast<char*>(&dataContent.numClusters), sizeof(uint32_t));

        readString(dataContent.parentName);

        in.read(reinterpret_cast<char*>(&dataContent.parentNumPoints), sizeof(uint32_t));

        readVec(dataContent.clusterSizes, in);
        readVec(dataContent.clusterColors, in);
        readVec(dataContent.clusterIndices, in);

        size_t length;
        in.read(reinterpret_cast<char*>(&length), sizeof(size_t));
        dataContent.clusterNames.resize(length);
        for (size_t i = 0; i < length; i++) {
            readString(dataContent.clusterNames[i]);
        }

        in.read(reinterpret_cast<char*>(&length), sizeof(size_t));
        dataContent.clusterIDs.resize(length);
        for (size_t i = 0; i < length; i++) {
            readString(dataContent.clusterIDs[i]);
        }

        in.close();
    }
    else
    {
        throw DataLoadException(fileName, "File was not found at location.");
    }

    assert(dataContent.numClusters == dataContent.clusterNames.size());
    assert(dataContent.numClusters == dataContent.clusterSizes.size());
    assert(dataContent.clusterColors.size() == dataContent.numClusters * 3);

    ClusterLoadingInputDialog inputDialog(nullptr, *this, QFileInfo(fileName).baseName());

    // open dialog and wait for user input
    int ok = inputDialog.exec();

    if (ok == QDialog::Accepted && !inputDialog.getDatasetName().isEmpty()) {
    
        auto sourceDataset = inputDialog.getSourceDataset();

        if (!sourceDataset.isValid())
        {
            qDebug() << "ClusterLoader: Selected parent data set is not valid";
            return;
        }

        auto t1 = dataContent.parentName;
        auto t2 = inputDialog.getSourceDataset().getDataset()->text();

        if (dataContent.parentName != sourceDataset.getDataset()->text().toStdString())
        {
            qDebug() << "ClusterLoader: Selected parent data is not the parent of the object to be loaded";
            return;
        }

        auto clusterData = _core->addDataset<Clusters>("Cluster", inputDialog.getDatasetName(), sourceDataset);
        events().notifyDatasetAdded(clusterData);

        size_t numC = dataContent.numClusters;
        size_t globalIndicesOffset = 0;

        for (size_t i = 0; i < numC; i++) {
            Cluster cluster;

            cluster.setName(QString::fromStdString(dataContent.clusterNames[i]));
            cluster.setId(QString::fromStdString(dataContent.clusterIDs[i]));

            QColor color = { dataContent.clusterColors[i * 3], dataContent.clusterColors[i * 3 + 1] , dataContent.clusterColors[i * 3 + 2] };
            cluster.setColor(color);

            cluster.getIndices() = std::vector<std::uint32_t>(dataContent.clusterIndices.begin() + globalIndicesOffset, dataContent.clusterIndices.begin() + globalIndicesOffset + dataContent.clusterSizes[i]);
            globalIndicesOffset += dataContent.clusterSizes[i];

            clusterData->addCluster(cluster);
        }
        
        events().notifyDatasetDataChanged(clusterData);

        qDebug() << "Number of loaded clusters: " << clusterData->getClusters().size();

    }

}

// =============================================================================
// Loading input box
// =============================================================================


ClusterLoadingInputDialog::ClusterLoadingInputDialog(QWidget* parent, ClusterLoader& binLoader, QString fileName) :
    QDialog(parent),
    _datasetNameAction(this, "Dataset name", fileName),
    _datasetPickerAction(this, "Source dataset"),
    _loadAction(this, "Load"),
    _groupAction(this, "Settings")
{
    setWindowTitle("Cluster Loader");

    _groupAction.addAction(&_datasetNameAction);
    _groupAction.addAction(&_datasetPickerAction);
    _groupAction.addAction(&_loadAction);

    _loadAction.setEnabled(false);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_groupAction.createWidget(this));

    setLayout(layout);

    // Update the state of the dataset picker
    const auto updateDatasetPicker = [this]() -> void {
        // Get unique identifier and gui names from all point data sets in the core
        auto dataSets = mv::Application::core()->requestAllDataSets(QVector<mv::DataType> {PointType});

        // Assign found dataset(s)
        _datasetPickerAction.setDatasets(dataSets);
        };

    // Accept when the load action is triggered
    connect(&_loadAction, &TriggerAction::triggered, this, [this]() {
        accept();
        });

    // Enable load once a parent data set was chosen
    connect(&_datasetPickerAction, &DatasetPickerAction::datasetPicked, this, [this](mv::Dataset<mv::DatasetImpl> pickedDataset) {
        _loadAction.setEnabled(true);
        });

    // Update dataset picker at startup
    updateDatasetPicker();
}

// =============================================================================
// Factory
// =============================================================================

QIcon ClusterLoaderFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("database");
}


LoaderPlugin* ClusterLoaderFactory::produce()
{
    return new ClusterLoader(this);
}

DataTypes ClusterLoaderFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;
    supportedTypes.append(ClusterType);
    return supportedTypes;
}
