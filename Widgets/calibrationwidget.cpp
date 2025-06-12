#include "calibrationwidget.h"

CalibrationWidget::CalibrationWidget(QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    m_settingsDialog = new SettingsDialog(this);

    connect(m_settingsDialog, &SettingsDialog::mapUpdated, this, &CalibrationWidget::onMapUpdated);
    connect(m_settingsDialog, &SettingsDialog::generalInfoChanged, this, &CalibrationWidget::onGeneralInfoChanged);
    connect(m_settingsDialog, &SettingsDialog::finished, this, &CalibrationWidget::onDialogFinished);
    connect(ui.settingsButton, &QPushButton::clicked, this, &CalibrationWidget::onSettingsButtonClicked);
}

void CalibrationWidget::onSettingsButtonClicked() {
    ui.chartWidget->stop();
    A3D::SurfaceChartEntity* chart = ui.chartWidget->chart();
    m_settingsDialog->setMap(chart->mapChart());
    GeneralInfo info;
    info.m_worldColor  = ui.chartWidget->worldColor();
    info.m_markerColor = chart->markerColor();
    info.m_variants    = chart->renderVariants();
    m_settingsDialog->setGeneralInfo(info);
    m_settingsDialog->open();
}

void CalibrationWidget::onDialogFinished(int)
{
    ui.chartWidget->restart();
}

void CalibrationWidget::onGeneralInfoChanged(GeneralInfo info) {
    ui.chartWidget->chart()->setMarkerColor(info.m_markerColor);
    ui.chartWidget->chart()->setRenderVariants(info.m_variants);
    ui.chartWidget->setWorldColor(info.m_worldColor);
}

void CalibrationWidget::onMapUpdated(A3D::MapChart3D const& map) {
    ui.chartWidget->chart()->setChart(map);
}

A3D::SurfaceChartEntity* CalibrationWidget::chart() const {
    return ui.chartWidget->chart();
}
