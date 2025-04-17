#include "ClusterLoaderBin.h"

#include "ClusterUtils.h"

#include <ClusterData/ClusterData.h>
#include <PointData/PointData.h>

#include <iostream>
#include <vector>

Q_PLUGIN_METADATA(IID "manivault.studio.ClusterLoaderBin")

using namespace mv;
using namespace mv::gui;

ClusterLoadingInputDialog::ClusterLoadingInputDialog(QWidget* parent, ClusterLoaderBin& binLoader, QString fileName) :
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
        auto dataSets = mv::data().getAllDatasets({ PointType });

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

ClusterLoaderBin::ClusterLoaderBin(const PluginFactory* factory) :
    LoaderPlugin(factory)
{
}

void ClusterLoaderBin::init()
{
}

void ClusterLoaderBin::loadData()
{
    const QString fileName = AskForFileName(tr("BIN Files (*.bin)"));

    // Don't try to load a file if the dialog was cancelled or the file name is empty
    if (fileName.isNull() || fileName.isEmpty())
        return;

    ClusterLoadingInputDialog inputDialog(nullptr, *this, QFileInfo(fileName).baseName());

    // open dialog and wait for user input
    int ok = inputDialog.exec();

    if (ok == QDialog::Accepted && !inputDialog.getDatasetName().isEmpty()) {

        std::cout << "Loading Cluster file: " << fileName.toStdString() << std::endl;

        bool readColors = false;

        // read in binary data, be sure to read in the same other as ClusterExporter::writeClusterDataToBinary wrote them
        utils::DataContent dataContent;
        std::ifstream in(fileName.toStdString(), std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::beg);

            utils::readVal(dataContent.numClusters, in);
            utils::readVal(dataContent.parentNumPoints, in);
            utils::readVec(dataContent.clusterSizes, in);

            utils::readVal(readColors, in);
            if (readColors)
                utils::readVec(dataContent.clusterColors, in);

            utils::readVec(dataContent.clusterIndices, in);
            utils::readVecOfStrings(dataContent.clusterNames, in);

            utils::readString(dataContent.parentName, in);

            in.close();
        }
        else
        {
            throw DataLoadException(fileName, "File was not found at location.");
        }

        assert(dataContent.numClusters == dataContent.clusterNames.size());
        assert(dataContent.numClusters == dataContent.clusterSizes.size());
        assert(!readColors || dataContent.clusterColors.size() == dataContent.numClusters * 3);

        auto sourceDataset = inputDialog.getSourceDataset();

        if (!sourceDataset.isValid())
        {
            std::cout << "ClusterLoaderBin: Selected parent data set is not valid" << std::endl;
            return;
        }

        if (dataContent.parentNumPoints != mv::data().getDataset<Points>(sourceDataset.getDatasetId())->getNumPoints())
        {
            std::cout << "ClusterLoaderBin: Selected parent has a different number of points  (" << mv::data().getDataset<Points>(sourceDataset.getDatasetId())->getNumPoints() << ") from loaded clusters (" << dataContent.parentNumPoints << ")" << std::endl;
            return;
        }

        if (dataContent.parentName != sourceDataset.getDataset()->text().toStdString())
        {
            std::cout << "ClusterLoaderBin: Selected parent data is not the parent of the object to be loaded (we will continue anyways)" << std::endl;
            std::cout << "ClusterLoaderBin: loaded parent name: " << dataContent.parentName << std::endl;
            std::cout << "ClusterLoaderBin: designated parent name: " << sourceDataset.getDataset()->text().toStdString() << std::endl;
        }

        auto clusterData = mv::data().createDataset<Clusters>("Cluster", inputDialog.getDatasetName(), sourceDataset);
        events().notifyDatasetAdded(clusterData);

        size_t numC = dataContent.numClusters;
        size_t globalIndicesOffset = 0;

        for (size_t i = 0; i < numC; i++) {
            Cluster cluster;

            cluster.setName(QString::fromStdString(dataContent.clusterNames[i]));

            cluster.getIndices() = std::vector<std::uint32_t>(dataContent.clusterIndices.begin() + globalIndicesOffset, dataContent.clusterIndices.begin() + globalIndicesOffset + dataContent.clusterSizes[i]);
            globalIndicesOffset += dataContent.clusterSizes[i];
            
            if (readColors)
            {
                QColor color = { dataContent.clusterColors[i * 3], dataContent.clusterColors[i * 3 + 1] , dataContent.clusterColors[i * 3 + 2] };
                cluster.setColor(color);
            }

            clusterData->addCluster(cluster);
        }
        
        events().notifyDatasetDataChanged(clusterData);

        std::cout << "Number of loaded clusters: " << clusterData->getClusters().size() << std::endl;

    }

}

LoaderPlugin* ClusterLoaderBinFactory::produce()
{
    return new ClusterLoaderBin(this);
}

DataTypes ClusterLoaderBinFactory::supportedDataTypes() const
{
    return { ClusterType };
}
