#ifndef CHARTAXISSETTINGS_H
#define CHARTAXISSETTINGS_H

#include "ui_chartaxissettings.h"



class ChartAxisSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ChartAxisSettings(QWidget *parent = nullptr);

private slots:

private:
    Ui::ChartAxisSettings ui;
};

#endif // CHARTAXISSETTINGS_H
