#include "calibrationwidget.h"

CalibrationWidget::CalibrationWidget(QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    m_settingsDialog = new SettingsDialog(this);

    connect(m_settingsDialog, &QDialog::accepted, this, &CalibrationWidget::onSettingsDialogAccepted);
    connect(ui.settingsButton, &QPushButton::clicked, this, &CalibrationWidget::onSettingsButtonClicked);
}

void CalibrationWidget::onSettingsButtonClicked() {
    m_settingsDialog->open();
}

void CalibrationWidget::onSettingsDialogAccepted() {}

A3D::SurfaceChartEntity* CalibrationWidget::chart() const {
    return ui.chartWidget->chart();
}
