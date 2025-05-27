#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include "ui_calibrationwidget.h"
#include "settingsdialog.h"

class CalibrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationWidget(QWidget *parent = nullptr);

private slots:
    void on_settingsButton_clicked();

private:
    Ui::CalibrationWidget ui;
    SettingsDialog m_settingsDialog;
};

#endif // CALIBRATIONWIDGET_H
