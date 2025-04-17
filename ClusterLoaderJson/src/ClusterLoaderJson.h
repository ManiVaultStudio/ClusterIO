#pragma once

#include <actions/DatasetPickerAction.h>

#include <LoaderPlugin.h>

#include <fstream>

using namespace mv::plugin;

class ClusterLoaderBin;

/**
 * ClusterLoaderJson
 *
 * Loader for cluster data in JSON format
 *
 * @author Thomas Kroes
 */
class ClusterLoaderJson : public LoaderPlugin
{
public:

    /**
     * Construct with pointer to \p factory
     * @param factory
     */
    ClusterLoaderJson(const PluginFactory* factory);

    /** No need for custom destructor */
    ~ClusterLoaderJson() override = default;

    /** Initialize the plugin */
    void init() override;

    /** Load the data from a file */
    void loadData() override;
};

/**
 * Plugin factory for ClusterLoaderJson
 *
 * @author Thomas Kroes
 */
class ClusterLoaderJsonFactory : public LoaderPluginFactory
{
    Q_INTERFACES(mv::plugin::LoaderPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "manivault.studio.ClusterLoaderJson" FILE "ClusterLoaderJson.json")

public:

    /** No need for custom constructor */
    ClusterLoaderJsonFactory() = default;

    /** No need for custom destructor */
    ~ClusterLoaderJsonFactory() override = default;

    /**
     * Create a new instance of the plugin
     * @return Pointer to the new plugin instance
     */
    LoaderPlugin* produce() override;

    /**
     * Get the supported data types
     * @return Supported data types
     */
    mv::DataTypes supportedDataTypes() const override;
};