#pragma once

#include <actions/DatasetPickerAction.h>

#include <QDialog>

/**
 * Loader input dialog class
 *
 * @authors Alexander Vieth and Thomas Kroes
 */
class LoaderInputDialog : public QDialog
{
    Q_OBJECT

public:
    /**
	 * Construct with pointer to \p parent widget and initial \p fileName
	 * @param parent Pointer to parent widget
	 * @param fileName Initial file name
     */
    LoaderInputDialog(QWidget* parent, QString fileName);

    /** Get preferred size */
    QSize sizeHint() const override {
        return { 400, 50 };
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
    mv::gui::StringAction            _datasetNameAction;        /** Dataset name action */
    mv::gui::DatasetPickerAction     _datasetPickerAction;      /** Dataset picker action for picking source datasets */
    mv::gui::TriggerAction           _loadAction;               /** Load action */
    mv::gui::GroupAction             _groupAction;              /** Group action */
};