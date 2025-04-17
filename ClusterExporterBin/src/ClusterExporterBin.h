#pragma once

#include <WriterPlugin.h>

#include <ClusterData/ClusterData.h>

#include "ClusterUtils.h"

#include <QCheckBox>
#include <QColor>
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

// =============================================================================
// ClusterExporter WriterPlugin
// =============================================================================

class ClusterExporter : public WriterPlugin
{
    Q_OBJECT
public:
    ClusterExporter(const PluginFactory* factory);
    ~ClusterExporter(void) override;

    void init() override;

    void writeData() Q_DECL_OVERRIDE;

private:
    /*! Get data set contents from core
     *
     * \param dataSetName Data set name to request from core
    */
    utils::DataContent retrieveDataSetContent(mv::Dataset<Clusters>& dataSet);

    void writeClusterDataToBinary(const QString& writePath, const utils::DataContent& dataContent, bool saveColors);

    void writeInfoTextForBinary(const QString& writePath, const utils::DataContent& dataContent, bool saveColors);

};


// =============================================================================
// Factory
// =============================================================================

class ClusterExporterFactory : public WriterPluginFactory
{
    Q_INTERFACES(mv::plugin::WriterPluginFactory mv::plugin::PluginFactory)
        Q_OBJECT
        Q_PLUGIN_METADATA(IID   "manivault.studio.ClusterExporter"
            FILE  "ClusterExporter.json")

public:
    ClusterExporterFactory(void) {}
    ~ClusterExporterFactory(void) override {}

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    WriterPlugin* produce() override;

    mv::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};