#include "ClusterLoaderJson.h"

#include "LoaderInputDialog.h"

#include <ClusterData/ClusterData.h>
#include <PointData/PointData.h>

#include <iostream>

Q_PLUGIN_METADATA(IID "manivault.studio.ClusterLoaderJson")

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

ClusterLoaderJson::ClusterLoaderJson(const PluginFactory* factory) :
	LoaderPlugin(factory)
{
}

void ClusterLoaderJson::init()
{
}

void ClusterLoaderJson::loadData()
{
    try {
        const auto fileName = AskForFileName(tr("JSON Files (*.json)"));

        if (fileName.isNull() || fileName.isEmpty())
            return;

        LoaderInputDialog inputDialog(nullptr, QFileInfo(fileName).baseName());

        const auto dialogResult = inputDialog.exec();

        if (dialogResult == QDialog::Accepted && !inputDialog.getDatasetName().isEmpty()) {

            //bool readColors = false;

            //assert(dataContent.numClusters == dataContent.clusterNames.size());
            //assert(dataContent.numClusters == dataContent.clusterSizes.size());
            //assert(!readColors || dataContent.clusterColors.size() == dataContent.numClusters * 3);

            auto sourceDataset = inputDialog.getSourceDataset();

            if (!sourceDataset.isValid())
                throw std::runtime_error("Selected parent data set is not valid");

            //if (dataContent.parentNumPoints != mv::data().getDataset<Points>(sourceDataset.getDatasetId())->getNumPoints())
            //{
            //    std::cout << "ClusterLoaderBin: Selected parent has a different number of points  (" << mv::data().getDataset<Points>(sourceDataset.getDatasetId())->getNumPoints() << ") from loaded clusters (" << dataContent.parentNumPoints << ")" << std::endl;
            //    return;
            //}

            //if (dataContent.parentName != sourceDataset.getDataset()->text().toStdString())
            //{
            //    std::cout << "ClusterLoaderBin: Selected parent data is not the parent of the object to be loaded (we will continue anyways)" << std::endl;
            //    std::cout << "ClusterLoaderBin: loaded parent name: " << dataContent.parentName << std::endl;
            //    std::cout << "ClusterLoaderBin: designated parent name: " << sourceDataset.getDataset()->text().toStdString() << std::endl;
            //}

            auto clusterData = mv::data().createDataset<Clusters>("Cluster", inputDialog.getDatasetName(), sourceDataset);

            events().notifyDatasetAdded(clusterData);

            //size_t numC = dataContent.numClusters;
            //size_t globalIndicesOffset = 0;

            //for (size_t i = 0; i < numC; i++) {
            //    Cluster cluster;

            //    cluster.setName(QString::fromStdString(dataContent.clusterNames[i]));

            //    cluster.getIndices() = std::vector<std::uint32_t>(dataContent.clusterIndices.begin() + globalIndicesOffset, dataContent.clusterIndices.begin() + globalIndicesOffset + dataContent.clusterSizes[i]);
            //    globalIndicesOffset += dataContent.clusterSizes[i];

            //    if (readColors)
            //    {
            //        QColor color = { dataContent.clusterColors[i * 3], dataContent.clusterColors[i * 3 + 1] , dataContent.clusterColors[i * 3 + 2] };
            //        cluster.setColor(color);
            //    }

            //    clusterData->addCluster(cluster);
            //}

            events().notifyDatasetDataChanged(clusterData);
    }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load clusters", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to load clusters");
    }
}

LoaderPlugin* ClusterLoaderJsonFactory::produce()
{
    return new ClusterLoaderJson(this);
}

DataTypes ClusterLoaderJsonFactory::supportedDataTypes() const
{
    return { ClusterType };
}
