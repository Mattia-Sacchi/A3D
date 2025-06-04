#ifndef LISTINDICATORSPREVIEWWIDGET_H
#define LISTINDICATORSPREVIEWWIDGET_H

#include "ui_listindicatorspreviewwidget.h"
#include "../A3D/chart.h"
#include "editenumdialog.h"

class ListIndicatorsPreviewWidget : public QWidget {
	Q_OBJECT

public:
    explicit ListIndicatorsPreviewWidget(QWidget* parent = nullptr);

    void addIndicators(std::vector<A3D::ChartAxisIndicator> const&);
    void addIndicator(A3D::ChartAxisIndicator const&);

    std::vector<A3D::ChartAxisIndicator> indicators() const;
signals:
    void addClicked();
private slots:
    void onItemDoubleClicked(QModelIndex const&);
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onEditIndicartorsClicked();
    void onItemSelectionChanged();
    void onEditAccepted();

private:
    std::vector<A3D::ChartAxisIndicator> m_indicators;
    EditEnumDialog m_dialog;
    Ui::ListIndicatorsPreviewWidget ui;
};

#endif // LISTINDICATORSPREVIEWWIDGET_H
