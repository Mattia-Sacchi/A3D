#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include "ui_calibrationwidget.h"
#include "settingsdialog.h"
#include "../A3D/chart.h"
class CalibrationWidget : public QWidget {
    Q_OBJECT

public:
    explicit CalibrationWidget(QWidget* parent = nullptr);

    A3D::SurfaceChartEntity* chart() const;

private slots:
    void onSettingsButtonClicked();
    void onSettingsDialogAccepted();

private:
    Ui::CalibrationWidget ui;
    SettingsDialog* m_settingsDialog;
};

#endif // CALIBRATIONWIDGET_H
