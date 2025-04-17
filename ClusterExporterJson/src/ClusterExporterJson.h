#pragma once

#include <WriterPlugin.h>

#include <ClusterData/ClusterData.h>

using namespace mv::plugin;
using namespace mv::gui;

/**
 * ClusterExporterJson
 *
 * Exporter for cluster data in JSON format
 *
 * @author Thomas Kroes
 */
class ClusterExporterJson : public WriterPlugin
{
public:

	/**
     * Construct with pointer to \p factory
	 * @param factory 
	 */
	ClusterExporterJson(const PluginFactory* factory);

    /** No need for custom destructor */
    ~ClusterExporterJson() override = default;

    /** Initialize the plugin */
    void init() override;

    /** Write the data to a file */
    void writeData() override;

private:
    //void writeClusterDataToBinary(const QString& writePath, const utils::DataContent& dataContent, bool saveColors);
    //void writeInfoTextForBinary(const QString& writePath, const utils::DataContent& dataContent, bool saveColors);

};

/**
 * Plugin factory for ClusterExporterJson
 *
 * @author: Thomas Kroes
 */
class ClusterExporterJsonFactory : public WriterPluginFactory
{
    Q_INTERFACES(mv::plugin::WriterPluginFactory mv::plugin::PluginFactory)
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "manivault.studio.ClusterExporterJson" FILE "ClusterExporterJson.json")

public:

    /** Default constructor */
    ClusterExporterJsonFactory();

    /** No need for custom destructor */
    ~ClusterExporterJsonFactory() override = default;

    /** Initialize the plugin factory */
    void initialize() override;

    /**
     * Create a new instance of the plugin
     * @return Pointer to the new plugin instance
     */
    WriterPlugin* produce() override;

    /**
     * Get the supported data types
     * @return Supported data types
     */
    mv::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};