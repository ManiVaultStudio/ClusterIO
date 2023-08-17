#include "ClusterExporter.h"

#include <actions/PluginTriggerAction.h>

#include <PointData/PointData.h>

#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>

#include <fstream>
#include <iterator>
#include <algorithm>

Q_PLUGIN_METADATA(IID "nl.tudelft.ClusterExporter")

using namespace hdps;
using namespace hdps::gui;

ClusterExporter::ClusterExporter(const PluginFactory* factory) :
    WriterPlugin(factory)
{
}

ClusterExporter::~ClusterExporter(void)
{
}

void ClusterExporter::init()
{
}

void ClusterExporter::writeData()
{
    auto inputDataset = getInputDataset<Clusters>();

    // Let the user select one of those data sets
    ClusterExporterDialog inputDialog(nullptr, inputDataset->text());
    inputDialog.setModal(true);

    int ok = inputDialog.exec();

    if ((ok == QDialog::Accepted)) {

        // Let the user chose the save path
        QSettings settings(QLatin1String{ "HDPS" }, QLatin1String{ "Plugins/" } + getKind());
        const QLatin1String directoryPathKey("directoryPath");
        const auto directoryPath = settings.value(directoryPathKey).toString() + "/";
        QString fileName = QFileDialog::getSaveFileName(
            nullptr, tr("Save data set"), directoryPath + inputDataset->text() + ".bin", tr("Binary file (*.bin);;All Files (*)"));

        // Only continue when the dialog has not been not canceled and the file name is non-empty.
        if (fileName.isNull() || fileName.isEmpty())
        {
            qDebug() << "ClusterExporter: No data written to disk - File name empty";
            return;
        }
        else
        {
            // store the directory name
            settings.setValue(directoryPathKey, QFileInfo(fileName).absolutePath());

            // get data from core
            DataContent dataContent = retrieveDataSetContent(inputDataset);
            writeClusterDataToBinary(fileName, dataContent);
            writeInfoTextForBinary(fileName, dataContent);
            qDebug() << "ClusterExporter: Data written to disk - File name: " << fileName;
            return;
        }
    }
    else
    {
        qDebug() << "ClusterExporter: No data written to disk - No data set selected";
        return;
    }
}

DataContent ClusterExporter::retrieveDataSetContent(hdps::Dataset<Clusters>& dataSet) {
    DataContent dataContent;

    dataContent.numClusters = dataSet->getClusters().size();

    dataContent.clusterNames.reserve(dataContent.numClusters);
    dataContent.clusterSizes.reserve(dataContent.numClusters);

    for (const auto& cluster : dataSet->getClusters())
    {
        dataContent.clusterIndices.insert(dataContent.clusterIndices.end(), cluster.getIndices().begin(), cluster.getIndices().end());
        dataContent.clusterNames.emplace_back(cluster.getName().toStdString());
        dataContent.clusterIDs.emplace_back(cluster.getId().toStdString());
        dataContent.clusterSizes.emplace_back(cluster.getNumberOfIndices());

        auto color = cluster.getColor();
        auto colorVec = std::vector<int>{ color.red(), color.green(), color.blue() };
        dataContent.clusterColors.insert(dataContent.clusterColors.end(), colorVec.begin(), colorVec.end());
    }

    const auto parent = dataSet->getParent().getDataset();

    dataContent.parentName = parent->getGuiName().toStdString();

    if(parent->getDataType() == PointType)
        dataContent.parentNumPoints= dataSet->getDataHierarchyItem().getParent().getDataset<Points>()->getNumPoints();

    return dataContent;
}

template <typename T>
void writeVec(const std::vector<T>& vec, std::ofstream& fout)
{
    size_t length = vec.size();
    fout.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
    fout.write(reinterpret_cast<const char*>(vec.data()), length * sizeof(T));
}
template void writeVec<int32_t>(const std::vector<int32_t>& vec, std::ofstream& fout);
template void writeVec<uint32_t>(const std::vector<uint32_t>& vec, std::ofstream& fout);

void ClusterExporter::writeClusterDataToBinary(const QString& writePath, const DataContent& dataContent)
{
    std::ofstream fout(writePath.toStdString(), std::ofstream::out | std::ofstream::binary);

    auto writeString = [&fout](const std::string& s) -> void {
        size_t length = s.size();
        fout.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
        fout.write(reinterpret_cast<const char*>(s.c_str()), length);
        };

    fout.write(reinterpret_cast<const char*>(&dataContent.numClusters), sizeof(uint32_t));

    writeString(dataContent.parentName);

    fout.write(reinterpret_cast<const char*>(&dataContent.parentNumPoints), sizeof(uint32_t));

    writeVec(dataContent.clusterSizes, fout);
    writeVec(dataContent.clusterColors, fout);
    writeVec(dataContent.clusterIndices, fout);

    size_t length = dataContent.clusterNames.size();
    fout.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
    for (const std::string& clusterName : dataContent.clusterNames) {
        writeString(clusterName);
    }

    length = dataContent.clusterIDs.size();
    fout.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
    for (const std::string& clusterID : dataContent.clusterIDs) {
        writeString(clusterID);
    }

    fout.close();
}

void ClusterExporter::writeInfoTextForBinary(const QString& writePath, const DataContent& dataContent) {
    std::string infoText;
    std::string fileName = QFileInfo(writePath).fileName().toStdString();

    infoText += fileName + "\n";
    infoText += "Num clusters: " + std::to_string(dataContent.numClusters) + "\n";
    infoText += "Source data: " + dataContent.parentName + "\n";
    if(dataContent.parentNumPoints != 0)
        infoText += "Num data points (source): " + std::to_string(dataContent.parentNumPoints) + "\n";

    std::ofstream fout(writePath.section(".", 0, 0).toStdString() + ".txt");
    fout << infoText;
    fout.close();
}

// =============================================================================
// Factory
// =============================================================================

WriterPlugin* ClusterExporterFactory::produce()
{
    return new ClusterExporter(this);
}

QIcon ClusterExporterFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("database", color);
}

DataTypes ClusterExporterFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;
    supportedTypes.append(ClusterType);
    return supportedTypes;
}

PluginTriggerActions ClusterExporterFactory::getPluginTriggerActions(const hdps::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getPluginInstance = [this](const Dataset<Clusters>& dataset) -> ClusterExporter* {
        return dynamic_cast<ClusterExporter*>(plugins().requestPlugin(getKind(), { dataset }));
    };

    if (PluginFactory::areAllDatasetsOfTheSameType(datasets, ClusterType)) {
        if (datasets.count() >= 1) {
            auto pluginTriggerAction = new PluginTriggerAction(const_cast<ClusterExporterFactory*>(this), this, "ClusterExporter", "Export cluster to binary file", getIcon(), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                for (auto dataset : datasets)
                    getPluginInstance(dataset);
            });

            pluginTriggerActions << pluginTriggerAction;
        }
    }

    return pluginTriggerActions;
}