#pragma once

#include <WriterPlugin.h>

#include <ClusterData/ClusterData.h>

#include <QDialog>
#include <QTextLine>
#include <QHBoxLayout>
#include <QPushButton>
#include <QColor>

#include <vector>

using namespace mv::plugin;
using namespace mv::gui;

struct DataContent {
    DataContent() : numClusters(0), clusterSizes{}, clusterNames{}, clusterIDs{}, clusterColors {}, clusterIndices{}, parentName(""), parentNumPoints(0) {};

    uint32_t numClusters;
    std::vector<uint32_t> clusterSizes;
    std::vector<std::string> clusterNames;
    std::vector<std::string> clusterIDs;
    std::vector<int32_t> clusterColors;
    std::vector<uint32_t> clusterIndices;

    std::string parentName;
    uint32_t parentNumPoints;
};

// =============================================================================
// Loading input box
// =============================================================================

class ClusterExporterDialog : public QDialog
{
    Q_OBJECT
public:
    ClusterExporterDialog(QWidget* parent, const QString& dataName = "") :
        QDialog(parent), _writeButton("Write file")
    {
        setWindowTitle("Cluster Exporter");

        QLabel* indicesLabel = new QLabel("Save cluster data set to disk: " + dataName);

        _writeButton.setDefault(true);

        connect(&_writeButton, &QPushButton::pressed, this, [this]() {emit closeDialog(); });
        connect(this, &ClusterExporterDialog::closeDialog, this, &QDialog::accept);

        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(indicesLabel);
        layout->addWidget(&_writeButton);
        setLayout(layout);
    }

signals:
    void closeDialog();

private:
    QPushButton     _writeButton;
};

// =============================================================================
// View
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
    DataContent retrieveDataSetContent(mv::Dataset<Clusters>& dataSet);

    void writeClusterDataToBinary(const QString& writePath, const DataContent& dataContent);

    void writeInfoTextForBinary(const QString& writePath, const DataContent& dataContent);

};


// =============================================================================
// Factory
// =============================================================================

class ClusterExporterFactory : public WriterPluginFactory
{
    Q_INTERFACES(mv::plugin::WriterPluginFactory mv::plugin::PluginFactory)
        Q_OBJECT
        Q_PLUGIN_METADATA(IID   "nl.tudelft.ClusterExporter"
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