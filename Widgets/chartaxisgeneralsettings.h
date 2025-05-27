#ifndef CHARTAXISGENERALSETTINGS_H
#define CHARTAXISGENERALSETTINGS_H

#include "ui_chartaxisgeneralsettings.h"

enum FontResolutions
{
    FR_Low = 0,
    FR_Medium,
    FR_High,
    FR_Count
};

class ChartAxisGeneralSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ChartAxisGeneralSettings(QWidget *parent = nullptr);

private slots:
    void onChangeIndicatorColorButtonClicked();

    void onChangeLabelColorButtonClicked();

    void onChangeFontButtonClicked();

    void onHighResolutionRadioButtonClicked();

    void onMediumResolutionRadioButtonClicked();

    void onLowResolutionRadioButtonClicked();

private:
    void setResoulution(FontResolutions res);
    void updateFontExampleResoulution();
    FontResolutions m_resolution;
    Ui::ChartAxisGeneralSettings ui;
};

#endif // CHARTAXISSETTINGS_H
