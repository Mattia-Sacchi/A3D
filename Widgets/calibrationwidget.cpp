#include "calibrationwidget.h"

CalibrationWidget::CalibrationWidget(QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    m_settingsDialog = new SettingsDialog(this);

    connect(m_settingsDialog, &SettingsDialog::mapUpdated, this, &CalibrationWidget::onMapUpdated);
    connect(ui.settingsButton, &QPushButton::clicked, this, &CalibrationWidget::onSettingsButtonClicked);
}

void CalibrationWidget::onSettingsButtonClicked() {
    A3D::MapChart3D chart = ui.chartWidget->chart()->mapChart();
    m_settingsDialog->setMap(chart);
    m_settingsDialog->open();
}

void CalibrationWidget::onMapUpdated(A3D::MapChart3D const& map) {
    ui.chartWidget->chart()->setChart(map);
}

A3D::SurfaceChartEntity* CalibrationWidget::chart() const {
    return ui.chartWidget->chart();
}
