#include "ClusterExporterJson.h"

#include <actions/PluginTriggerAction.h>

#include <PointData/PointData.h>

#include <QFileDialog>
#include <QFileInfo>
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
        auto fileName = QFileDialog::getSaveFileName(
            nullptr,
            tr("Save JSON File"),
            "clusters.json",
            tr("JSON Files (*.json)")
        );

    	if (fileName.isNull() || fileName.isEmpty())
    		throw std::runtime_error("File name is empty");

        if (!fileName.endsWith(".json", Qt::CaseInsensitive))
            fileName += ".json";

    	auto clustersDataset = getInputDataset<Clusters>();

    	if (!clustersDataset.isValid())
    		throw std::runtime_error("Invalid clusters dataset");

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

    	QFile jsonFile(fileName);

    	if (!jsonFile.open(QFile::WriteOnly))
    		throw std::runtime_error("Unable to open file for writing");

    	const auto jsonDocument = QJsonDocument::fromVariant(QVariantMap({
			{ "clusters", clustersList }
		}));

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        jsonFile.write(jsonDocument.toJson());

        addNotification(QString("Clusters exported to %1").arg(fileName));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to export clusters", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to export clusters");
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