#include "calibrationwidget.h"

CalibrationWidget::CalibrationWidget(QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    m_settingsDialog = new SettingsDialog(this);

    connect(m_settingsDialog, &SettingsDialog::mapUpdated, this, &CalibrationWidget::onMapUpdated);
    connect(m_settingsDialog, &SettingsDialog::generalInfoChanged, this, &CalibrationWidget::onGeneralInfoChanged);
    connect(ui.settingsButton, &QPushButton::clicked, this, &CalibrationWidget::onSettingsButtonClicked);
}

void CalibrationWidget::onSettingsButtonClicked() {
    A3D::SurfaceChartEntity* chart = ui.chartWidget->chart();
    m_settingsDialog->setMap(chart->mapChart());
    GeneralInfo info;
    info.m_worldColor  = Qt::white;
    info.m_markerColor = chart->markerColor();
    info.m_variants    = chart->renderVariants();
    m_settingsDialog->setGeneralInfo(info);
    m_settingsDialog->open();
}

void CalibrationWidget::onGeneralInfoChanged(GeneralInfo info) {
    ui.chartWidget->chart()->setMarkerColor(info.m_markerColor);
    ui.chartWidget->chart()->setRenderVariants(info.m_variants);
}

void CalibrationWidget::onMapUpdated(A3D::MapChart3D const& map) {
    ui.chartWidget->chart()->setChart(map);
}

A3D::SurfaceChartEntity* CalibrationWidget::chart() const {
    return ui.chartWidget->chart();
}
