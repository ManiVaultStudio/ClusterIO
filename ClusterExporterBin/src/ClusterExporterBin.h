#pragma once

#include <WriterPlugin.h>

#include <ClusterData/ClusterData.h>

#include "ClusterUtils.h"

#include <QCheckBox>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QTextLine>

using namespace mv::plugin;
using namespace mv::gui;

// =============================================================================
// Loading input box
// =============================================================================

class ClusterExporterDialog : public QDialog
{
    Q_OBJECT
public:
    ClusterExporterDialog(QWidget* parent, const QString& dataName = "") :
        QDialog(parent), _writeButton("Write file", parent), _saveColors(parent)
    {
        setWindowTitle("Cluster Exporter");

        QLabel* indicesLabel = new QLabel("Save cluster data set to disk: " + dataName);

        _writeButton.setDefault(true);

        QLabel* saveColorsLabel = new QLabel("Save Colors");

        connect(&_writeButton, &QPushButton::pressed, this, [this]() {emit closeDialog(); });
        connect(this, &ClusterExporterDialog::closeDialog, this, &QDialog::accept);

        QGridLayout*layout = new QGridLayout();
        layout->addWidget(indicesLabel,     0, 0, 1, 3);
        layout->addWidget(&_writeButton,    0, 3, 1, 1);
        layout->addWidget(saveColorsLabel,  1, 2, 1, 1, Qt::AlignRight);
        layout->addWidget(&_saveColors,     1, 3, 1, 1);
        setLayout(layout);
    }

    bool saveColors() const { return _saveColors.isChecked(); };

signals:
    void closeDialog();

private:
    QPushButton     _writeButton;
    QCheckBox       _saveColors;
};

/**
 * Cluster exporter bin class
 *
 * Exporter for cluster data in binary format
 *
 * @author Alexander Vieth
 */ 
class ClusterExporterBin : public WriterPlugin
{
public:

    /**
     * Construct with pointer to \p factory
     * @param factory
     */
    ClusterExporterBin(const PluginFactory* factory);

    /** No need for custom destructor */
    ~ClusterExporterBin() override = default;

    /** Initialize the plugin */
    void init() override;

    /** Write the data to a file */
    void writeData() override;

private:

    /*
     * Get data set contents from core
     * @param dataSetName Data set name to request from core
    */
    utils::DataContent retrieveDataSetContent(mv::Dataset<Clusters>& dataSet);

    /**
     * Write cluster data to binary file
     * @param writePath Path to write the file to
     * @param dataContent Data content to write
     * @param saveColors Flag indicating whether to save colors
    */
    void writeClusterDataToBinary(const QString& writePath, const utils::DataContent& dataContent, bool saveColors);

    /**
     * Write info text to file
     * @param writePath Path to write the file to
     * @param dataContent Data content to write
     * @param saveColors Flag indicating whether to save colors
    */
    void writeInfoTextForBinary(const QString& writePath, const utils::DataContent& dataContent, bool saveColors);
};


/**
 * Plugin factory for ClusterExporterJson
 *
 * @author Alexander Vieth
 */
class ClusterExporterBinFactory : public WriterPluginFactory
{
    Q_INTERFACES(mv::plugin::WriterPluginFactory mv::plugin::PluginFactory)
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "manivault.studio.ClusterExporterBin" FILE "ClusterExporterBin.json")

public:

    /** No need for custom constructor */
    ClusterExporterBinFactory() = default;

    /** No need for custom destructor */
    ~ClusterExporterBinFactory() override = default;

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