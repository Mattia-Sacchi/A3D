#ifndef CHARTAXISSETTINGS_H
#define CHARTAXISSETTINGS_H

#include "ui_chartaxissettings.h"
#include "../A3D/chart.h"
#include "addliindicatorsdialog.h"
#include "editliindicatorsdialog.h"

class ChartAxisSettings : public QWidget {
    Q_OBJECT

public:
    explicit ChartAxisSettings(QWidget* parent = nullptr);

private slots:
    void onLinearInterpolatedRadioButtonClicked();
    void onEnumeratedRadioButtonClicked();

    void onLinearAddClicked();

    void onLinearAddDialogAccepted();

    void onLinearEditDialogAccepted();

    void onLinearEditDialogClicked(std::vector<A3D::ChartAxisIndicator> const&);

private:
    void setChartAxisType(A3D::ChartAxisType);
    A3D::ChartAxisType m_type;
    A3D::ChartAxisData m_axisData;

    AddLiIndicatorsDialog m_linearAddDialog;
    EditLiIndicatorsDialog m_linearEditDialog;
    Ui::ChartAxisSettings ui;
};

#endif // CHARTAXISSETTINGS_H
