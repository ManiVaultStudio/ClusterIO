#pragma once

#include <actions/DatasetPickerAction.h>

#include <LoaderPlugin.h>

#include <QDialog>

#include <fstream>

using namespace mv::plugin;

// =============================================================================
// Loading input box
// =============================================================================

class ClusterLoader;

class ClusterLoadingInputDialog : public QDialog
{
    Q_OBJECT

public:
    ClusterLoadingInputDialog(QWidget* parent, ClusterLoader& clusterLoader, QString fileName);

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

// =============================================================================
// View
// =============================================================================

class ClusterLoader : public LoaderPlugin
{
    Q_OBJECT
public:
    ClusterLoader(const PluginFactory* factory) : LoaderPlugin(factory) { }
    ~ClusterLoader(void) override;

    void init() override;

    void loadData() Q_DECL_OVERRIDE;
};


// =============================================================================
// Factory
// =============================================================================

class ClusterLoaderFactory : public LoaderPluginFactory
{
    Q_INTERFACES(mv::plugin::LoaderPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "manivault.studio.ClusterLoader"
                      FILE  "ClusterLoader.json")

public:
    ClusterLoaderFactory();

    ~ClusterLoaderFactory() override {}

    LoaderPlugin* produce() override;

    mv::DataTypes supportedDataTypes() const override;
};