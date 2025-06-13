#ifndef INDICATORSPREVIEWWIDGET_H
#define INDICATORSPREVIEWWIDGET_H

#include "ui_indicatorspreviewwidget.h"
#include "../../A3D/chart.h"
#include "../Dialogs/addliindicatorsdialog.h"
#include "../Dialogs/editliindicatorsdialog.h"

class ChartAxisSettings;

class IndicatorsPreviewWidget : public QWidget {
	Q_OBJECT

public:
    explicit IndicatorsPreviewWidget(QWidget* parent = nullptr);

    void addIndicators(std::vector<A3D::ChartAxisIndicator>);
    std::vector<A3D::ChartAxisIndicator> indicators() const;

    void clear();

private slots:
    // Add
    void onAddButtonClicked();
    void onAddDialogAccepted();

    // Edit
    void onItemDoubleClicked(QModelIndex const&);
    void onEditIndicatorsClicked();
    void onEditDialogFinished();

    // Remove
    void onRemoveButtonClicked();

    void onItemSelectionChanged();

private:
    void editIndicators(std::vector<A3D::ChartAxisIndicator> const&);
    ChartAxisSettings* m_settings;
    AddLiIndicatorsDialog m_addDialog;
    EditLiIndicatorsDialog m_editDialog;
    std::vector<A3D::ChartAxisIndicator> m_indicators;
    Ui::IndicatorsPreviewWidget ui;
};

#endif // INDICATORSPREVIEWWIDGET_H
