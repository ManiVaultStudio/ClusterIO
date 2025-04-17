#include "ClusterLoaderJson.h"

#include "LoaderInputDialog.h"

#include <ClusterData/ClusterData.h>
#include <PointData/PointData.h>

#include <QFileDialog>

#include <iostream>

Q_PLUGIN_METADATA(IID "manivault.studio.ClusterLoaderJson")

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

ClusterLoaderJson::ClusterLoaderJson(const PluginFactory* factory) :
	LoaderPlugin(factory)
{
}

void ClusterLoaderJson::init()
{
}

void ClusterLoaderJson::loadData()
{
    try {
        QString clustersJsonFilePath = QFileDialog::getOpenFileName(
            nullptr,
            QObject::tr("Open clusters JSON file"),
            QString(getSetting("Directory/Open", "~").toString()),
            QObject::tr("JSON Files (*.json)")
        );

        if (clustersJsonFilePath.isNull() || clustersJsonFilePath.isEmpty())
            throw std::runtime_error("File name is empty");

        setSetting("Directory/Open", QFileInfo(clustersJsonFilePath).absoluteDir().absolutePath());

        LoaderInputDialog inputDialog(nullptr, QFileInfo(clustersJsonFilePath).baseName());

        const auto dialogResult = inputDialog.exec();

        if (dialogResult == QDialog::Rejected)
            return;

    	if (inputDialog.getDatasetName().isEmpty())
            throw std::runtime_error("Dataset name is empty");

        if (!QFileInfo(clustersJsonFilePath).exists())
            throw std::runtime_error("File does not exist");

        QFile jsonFile(clustersJsonFilePath);

        if (!jsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray data = jsonFile.readAll();

        auto jsonDocument = QJsonDocument::fromJson(data);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        const auto documentVariantMap = jsonDocument.toVariant().toMap();

        if (!documentVariantMap.contains("Clusters"))
            throw std::runtime_error("<b>Clusters</b> not found in JSON document");

        if (!documentVariantMap.contains("SourcePointsDataset"))
            throw std::runtime_error("<b>SourcePointsDataset</b> not found in JSON document");

        const auto sourcePointsDatasetMap = documentVariantMap["SourcePointsDataset"].toMap();

        if (!sourcePointsDatasetMap.contains("NumberOfPoints"))
            throw std::runtime_error("<b>SourcePointsDataset/NumberOfPoints</b> not found in JSON document");
        
        const auto numberOfPoints = sourcePointsDatasetMap["NumberOfPoints"].toInt();

        if (numberOfPoints != Dataset<Points>(inputDialog.getSourceDataset())->getNumPoints())
            throw std::runtime_error("Number of points in JSON document does not match the selected dataset");

    	const auto clustersList = documentVariantMap["Clusters"].toList();

        auto sourceDataset = inputDialog.getSourceDataset();

        if (!sourceDataset.isValid())
            throw std::runtime_error("Selected parent data set is not valid");

        auto clusterData = mv::data().createDataset<Clusters>("Cluster", inputDialog.getDatasetName(), sourceDataset);

        events().notifyDatasetAdded(clusterData);

        clusterData->getTask().setSubtasks(clustersList.count());
        clusterData->getTask().setRunning();

        for (const auto& clusterVariant : clustersList) {
            const auto clusterIndex = clustersList.indexOf(clusterVariant);

            clusterData->getTask().setSubtaskStarted(clusterIndex);
            {
	            const auto clusterMap = clusterVariant.toMap();

        		Cluster cluster;

	            cluster.setName(clusterMap["Name"].toString());
	            cluster.setId(clusterMap["ID"].toString());
	            cluster.setColor(clusterMap["Color"].toString());

	            const auto& indices = clusterMap["Indices"].toList();

	            cluster.getIndices().reserve(indices.size());

        		std::transform(indices.begin(), indices.end(), std::back_inserter(cluster.getIndices()), [](const QVariant& v) { return v.toInt(); });

	            clusterData->addCluster(cluster);
            }
            clusterData->getTask().setSubtaskFinished(clusterIndex);
        }

        clusterData->getTask().setFinished();

        events().notifyDatasetDataChanged(clusterData);

        addNotification(QString("Clusters loaded from <i>%1</i>").arg(clustersJsonFilePath));
    }
    catch (std::exception& e)
    {
        mv::help().addNotification("Cannot load clusters", e.what(), StyledIcon("circle-exclamation"));
    }
    catch (...) {
        mv::help().addNotification("Cannot load clusters", "An expected problem occurred", StyledIcon("circle-exclamation"));
    }
}

LoaderPlugin* ClusterLoaderJsonFactory::produce()
{
    return new ClusterLoaderJson(this);
}

DataTypes ClusterLoaderJsonFactory::supportedDataTypes() const
{
    return { ClusterType };
}
