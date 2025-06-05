#ifndef LISTINDICATORSPREVIEWWIDGET_H
#define LISTINDICATORSPREVIEWWIDGET_H

#include "ui_listindicatorspreviewwidget.h"
#include "../A3D/chart.h"
#include "editenumdialog.h"

class ChartAxisSettings;

class ListIndicatorsPreviewWidget : public QWidget {
	Q_OBJECT

public:
    explicit ListIndicatorsPreviewWidget(QWidget* parent = nullptr);

    void addIndicators(std::vector<A3D::ChartAxisIndicator> const&);
    void addIndicator(A3D::ChartAxisIndicator const&);

    std::vector<A3D::ChartAxisIndicator> indicators() const;

private slots:

    void onAddButtonClicked();

    void onEditIndicartorsClicked();
    void onItemDoubleClicked(QModelIndex const&);
    void onEditAccepted();

    void onRemoveButtonClicked();

    void onItemSelectionChanged();

private:
    std::vector<A3D::ChartAxisIndicator> m_indicators;
    ChartAxisSettings* m_settings;
    EditEnumDialog m_dialog;
    Ui::ListIndicatorsPreviewWidget ui;
};

#endif // LISTINDICATORSPREVIEWWIDGET_H
