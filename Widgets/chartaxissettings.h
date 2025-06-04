#ifndef CHARTAXISSETTINGS_H
#define CHARTAXISSETTINGS_H

#include "ui_chartaxissettings.h"
#include "../A3D/chart.h"
#include "addliindicatorsdialog.h"
#include "editliindicatorsdialog.h"

struct IndicatorInfo {
    A3D::ChartAxisIndicatorStyle m_style;
    A3D::ChartAxisIndicatorType m_type;
    bool operator==(IndicatorInfo const& i) const { return m_style == i.m_style && m_type == i.m_type; }
    bool operator!=(IndicatorInfo const& i) const { return !(*this == i); }
    IndicatorInfo(A3D::ChartAxisIndicator const& indicator) {
        m_style = indicator.m_style;
        m_type  = indicator.m_type;
    }
};

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

    void onLinearEditDialogRejected();

    void onLinearEditDialogClicked(std::vector<A3D::ChartAxisIndicator> const&);

private:
    void linearEditFinished();
    void setChartAxisType(A3D::ChartAxisType);
    A3D::ChartAxisType m_type;
    A3D::ChartAxisData m_axisData;

    AddLiIndicatorsDialog m_linearAddDialog;
    EditLiIndicatorsDialog m_linearEditDialog;
    Ui::ChartAxisSettings ui;
};

#endif // CHARTAXISSETTINGS_H
