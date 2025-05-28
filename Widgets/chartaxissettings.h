#ifndef CHARTAXISSETTINGS_H
#define CHARTAXISSETTINGS_H

#include "ui_chartaxissettings.h"
#include "../A3D/chart.h"

class ChartAxisSettings : public QWidget {
    Q_OBJECT

public:
    explicit ChartAxisSettings(QWidget* parent = nullptr);

private:
    Ui::ChartAxisSettings ui;
    A3D::ChartAxisData m_axisData;
};

#endif // CHARTAXISSETTINGS_H
