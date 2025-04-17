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
        const auto fileName = QFileDialog::getSaveFileName(nullptr, tr("JSON Files (*.json)"));

        if (fileName.isNull() || fileName.isEmpty())
            throw std::runtime_error("File name is empty");

    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load clusters", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to load clusters");
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