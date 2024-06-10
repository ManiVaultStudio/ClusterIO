#include "ClusterExporter.h"

#include <actions/PluginTriggerAction.h>

#include <PointData/PointData.h>

#include <DataHierarchyItem.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

#include <fstream>
#include <iostream>

Q_PLUGIN_METADATA(IID "manivault.studio.ClusterExporter")

using namespace mv;
using namespace mv::gui;

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
            std::cout << "ClusterExporter: No data written to disk - File name empty" << std::endl;
            return;
        }
        else if (inputDataset->getParent().getDataset()->getDataType() != PointType)
        {
            std::cout << "ClusterExporter: Parent of selected dataset must be points data - Doing nothing" << std::endl;
            return;
        }
        else
        {
            // store the directory name
            settings.setValue(directoryPathKey, QFileInfo(fileName).absolutePath());

            // get data from core
            utils::DataContent dataContent = retrieveDataSetContent(inputDataset);
            writeClusterDataToBinary(fileName, dataContent);
            writeInfoTextForBinary(fileName, dataContent);
            std::cout << "ClusterExporter: Data written to disk - File name: " << fileName.toStdString() << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "ClusterExporter: No data written to disk - No data set selected" << std::endl;
        return;
    }
}

utils::DataContent ClusterExporter::retrieveDataSetContent(mv::Dataset<Clusters>& dataSet) {
    utils::DataContent dataContent;

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

    dataContent.parentName = dataSet->getParent().getDataset()->getGuiName().toStdString();

    // assumes that the parent data is point data
    dataContent.parentNumPoints = dataSet->getDataHierarchyItem().getParent()->getDataset<Points>()->getNumPoints();

    return dataContent;
}

void ClusterExporter::writeClusterDataToBinary(const QString& writePath, const utils::DataContent& dataContent)
{
    std::ofstream fout(writePath.toStdString(), std::ofstream::out | std::ofstream::binary);

    utils::writeNum(dataContent.numClusters, fout);
    utils::writeNum(dataContent.parentNumPoints, fout);
    utils::writeVec(dataContent.clusterSizes, fout);
    utils::writeVec(dataContent.clusterColors, fout);
    utils::writeVec(dataContent.clusterIndices, fout);
    utils::writeVecOfStrings(dataContent.clusterNames, fout);
    utils::writeVecOfStrings(dataContent.clusterIDs, fout);
    utils::writeString(dataContent.parentName, fout);

    fout.close();
}

void ClusterExporter::writeInfoTextForBinary(const QString& writePath, const utils::DataContent& dataContent) {
    std::string infoText;
    std::string fileName = QFileInfo(writePath).fileName().toStdString();

    infoText += fileName + "\n";
    infoText += "Num clusters: " + std::to_string(dataContent.numClusters) + "\n";
    infoText += "Source data: " + dataContent.parentName + "\n";
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

PluginTriggerActions ClusterExporterFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getPluginInstance = [this](const Dataset<Clusters>& dataset) -> ClusterExporter* {
        return dynamic_cast<ClusterExporter*>(plugins().requestPlugin(getKind(), { dataset }));
    };

    if (PluginFactory::areAllDatasetsOfTheSameType(datasets, ClusterType)) {
        if (datasets.count() >= 1) {
            auto pluginTriggerAction = new PluginTriggerAction(const_cast<ClusterExporterFactory*>(this), this, "ClusterExporter", "Export cluster to binary file", getIcon(), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                for (const auto& dataset : datasets)
                    getPluginInstance(dataset);
            });

            pluginTriggerActions << pluginTriggerAction;
        }
    }

    return pluginTriggerActions;
}