#pragma once

#include <actions/DatasetPickerAction.h>

#include <LoaderPlugin.h>

#include <fstream>

using namespace mv::plugin;

/**
 * ClusterLoaderBin
 *
 * Loader for cluster data in binary format
 *
 * @author Alex Vieth
 */
class ClusterLoaderBin : public LoaderPlugin
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p factory
     * @param factory
     */
    ClusterLoaderBin(const PluginFactory* factory);

    /** No need for custom destructor */
    ~ClusterLoaderBin() override = default;

    /** Initialize the plugin */
    void init() override;

    /** Load the data from a file */
    void loadData() override;
};


/**
 * Plugin factory for creating cluster loader (BIN) instances
 *
 * @author Alex Vieth
 */
class ClusterLoaderBinFactory : public LoaderPluginFactory
{
    Q_INTERFACES(mv::plugin::LoaderPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "manivault.studio.ClusterLoaderBin" FILE "ClusterLoaderBin.json")

public:
    /** No need for custom constructor */
    ClusterLoaderBinFactory() = default;

    /** No need for custom destructor */
    ~ClusterLoaderBinFactory() override = default;

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