#include "ClusterExporterJson.h"

#include <actions/PluginTriggerAction.h>

#include <PointData/PointData.h>

#include <QFileDialog>
#include <QSettings>

#include <fstream>
#include <iostream>

Q_PLUGIN_METADATA(IID "manivault.studio.ClusterExporterJson")

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

ClusterExporterJson::ClusterExporterJson(const PluginFactory* factory) :
    WriterPlugin(factory)
{
}

void ClusterExporterJson::init()
{
}

void ClusterExporterJson::writeData()
{
    try {
        auto clustersJsonFilePath = QFileDialog::getSaveFileName(
            nullptr,
            tr("Save clusters JSON file"),
            "clusters.json",
            tr("JSON Files (*.json)")
        );

    	if (clustersJsonFilePath.isNull() || clustersJsonFilePath.isEmpty())
    		throw std::runtime_error("File name is empty");

        if (!clustersJsonFilePath.endsWith(".json", Qt::CaseInsensitive))
            clustersJsonFilePath += ".json";

    	auto clustersDataset = getInputDataset<Clusters>();

    	if (!clustersDataset.isValid())
    		throw std::runtime_error("Invalid clusters dataset");

        auto sourcePointsDataset = Dataset<Points>(clustersDataset->getParent());

        if (!sourcePointsDataset.isValid())
            throw std::runtime_error("Invalid source points dataset");

    	QVariantList clustersList;

    	clustersList.reserve(clustersDataset->getClusters().count());

    	for (const auto& cluster : clustersDataset->getClusters()) {
            const auto clusterIndicesList = QVariantList(cluster.getIndices().begin(), cluster.getIndices().end());;

    		QVariantMap clusterMap({
				{ "Name", cluster.getName() },
				{ "ID", cluster.getId() },
				{ "Color", cluster.getColor() },
				{ "Indices", clusterIndicesList }
			});

    		clustersList.push_back(clusterMap);
    	}

    	QFile jsonFile(clustersJsonFilePath);

    	if (!jsonFile.open(QFile::WriteOnly))
    		throw std::runtime_error("Unable to open file for writing");

        const auto sourcePointsDatasetMap = QVariantMap({
            { "NumberOfPoints", sourcePointsDataset->getNumPoints() }
        });

    	const auto jsonDocument = QJsonDocument::fromVariant(QVariantMap({
			{ "Clusters", clustersList },
            { "SourcePointsDataset", sourcePointsDatasetMap }
		}));

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        jsonFile.write(jsonDocument.toJson());

        addNotification(QString("Clusters exported to <i>%1<i/>").arg(clustersJsonFilePath));
    }
    catch (std::exception& e)
    {
        mv::help().addNotification("Unable to export clusters", e.what(), StyledIcon("circle-exclamation"));
    }
    catch (...) {
        mv::help().addNotification("Unable to export clusters", "An expected problem occurred", StyledIcon("circle-exclamation"));
    }
}

WriterPlugin* ClusterExporterJsonFactory::produce()
{
    return new ClusterExporterJson(this); 
}

DataTypes ClusterExporterJsonFactory::supportedDataTypes() const
{
	return { ClusterType  };
}

PluginTriggerActions ClusterExporterJsonFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getPluginInstance = [this](const Dataset<Clusters>& dataset) -> ClusterExporterJson* {
        return dynamic_cast<ClusterExporterJson*>(plugins().requestPlugin(getKind(), { dataset }));
    };

    if (PluginFactory::areAllDatasetsOfTheSameType(datasets, ClusterType)) {
        if (datasets.count() >= 1) {
            auto pluginTriggerAction = new PluginTriggerAction(const_cast<ClusterExporterJsonFactory*>(this), this, "Clusters (JSON)", "Export clusters to JSON file", icon(), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                for (const auto& dataset : datasets)
                    getPluginInstance(dataset);
            });

            pluginTriggerActions << pluginTriggerAction;
        }
    }

    return pluginTriggerActions;
}