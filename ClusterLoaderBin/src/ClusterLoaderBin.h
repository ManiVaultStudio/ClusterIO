#pragma once

#include <actions/DatasetPickerAction.h>

#include <LoaderPlugin.h>

#include <QDialog>

#include <fstream>

using namespace mv::plugin;

// =============================================================================
// Loading input box
// =============================================================================

class ClusterLoaderBin;

class ClusterLoadingInputDialog : public QDialog
{
    Q_OBJECT

public:
    ClusterLoadingInputDialog(QWidget* parent, ClusterLoaderBin& clusterLoader, QString fileName);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(400, 50);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /** Get the GUI name of the loaded dataset */
    QString getDatasetName() const {
        return _datasetNameAction.getString();
    }

    /** Get smart pointer to dataset (if any) */
    mv::Dataset<mv::DatasetImpl> getSourceDataset() {
        return _datasetPickerAction.getCurrentDataset();
    }

protected:
    mv::gui::StringAction            _datasetNameAction;             /** Dataset name action */
    mv::gui::DatasetPickerAction     _datasetPickerAction;           /** Dataset picker action for picking source datasets */
    mv::gui::TriggerAction           _loadAction;                    /** Load action */
    mv::gui::GroupAction             _groupAction;                   /** Group action */
};

/**
 * ClusterLoaderBin
 *
 * Loader for cluster data in binary format
 *
 * @author Alex Vieth
 */
class ClusterLoaderBin : public LoaderPlugin
{
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
 * Plugin factory for ClusterLoaderJson
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