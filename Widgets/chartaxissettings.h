#ifndef CHARTAXISSETTINGS_H
#define CHARTAXISSETTINGS_H

#include "ui_chartaxissettings.h"

enum FontResolutions
{
    FR_Low = 0,
    FR_Medium,
    FR_High,
    FR_Count
};

class ChartAxisSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ChartAxisSettings(QWidget *parent = nullptr);

private slots:
    void on_changeIndicatorColorButton_clicked();

    void on_changeLabelColorButton_clicked();

    void on_changeFontButton_clicked();

    void on_highResolutionRadioButton_clicked();



    void on_mediumResolutionRadioButton_clicked();

    void on_lowResolutionRadioButton_clicked();

private:
    void setResoulution(FontResolutions res);
    void updateFontExampleResoulution();
    FontResolutions m_resoulution;
    Ui::ChartAxisSettings ui;
};

#endif // CHARTAXISSETTINGS_H
