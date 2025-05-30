#ifndef INDICATORSPREVIEWWIDGET_H
#define INDICATORSPREVIEWWIDGET_H

#include "ui_indicatorspreviewwidget.h"
#include "../A3D/chart.h"

class IndicatorsPreviewWidget : public QWidget {
	Q_OBJECT

public:
    explicit IndicatorsPreviewWidget(QWidget* parent = nullptr);

    void addIndicators(std::vector<A3D::ChartAxisIndicator>);

    void sort();

    std::vector<A3D::ChartAxisIndicator> indicators() const;
signals:
    void addClicked();
    void editIndicators(std::vector<A3D::ChartAxisIndicator>);
private slots:
    void onItemDoubleClicked(QModelIndex const&);
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onEditIndicartorsClicked();
    void onItemSelectionChanged();

private:
    std::vector<A3D::ChartAxisIndicator> m_indicators;
	Ui::IndicatorsPreviewWidget ui;
};

#endif // INDICATORSPREVIEWWIDGET_H
