#include "LoaderInputDialog.h"

#include <PointData/PointData.h>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

LoaderInputDialog::LoaderInputDialog(QWidget* parent, QString fileName) :
    QDialog(parent),
    _datasetNameAction(this, "Dataset name", fileName),
    _datasetPickerAction(this, "Source dataset"),
    _loadAction(this, "Load"),
    _groupAction(this, "Settings")
{
    setWindowTitle("Cluster Loader");
    setWindowIcon(StyledIcon("file-import"));

    _groupAction.addAction(&_datasetNameAction);
    _groupAction.addAction(&_datasetPickerAction);
    _groupAction.addAction(&_loadAction);

    _loadAction.setEnabled(false);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_groupAction.createWidget(this));

    setLayout(layout);

    _datasetPickerAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == PointType;
	});

    // Accept when the load action is triggered
    connect(&_loadAction, &TriggerAction::triggered, this, [this]() {
        accept();
	});

    // Enable load once a parent data set was chosen
    connect(&_datasetPickerAction, &DatasetPickerAction::datasetPicked, this, [this](mv::Dataset<mv::DatasetImpl> pickedDataset) {
        _loadAction.setEnabled(true);
	});
}