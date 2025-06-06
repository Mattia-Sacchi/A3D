#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent) {
    ui.setupUi(this);
    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onSettingsAccepted);
}

void SettingsDialog::setMap(A3D::MapChart3D& map) {
    ui.xAxisSettings->setAxisData(map.axisData(A3D::AXIS_X));
    ui.yAxisSettings->setAxisData(map.axisData(A3D::AXIS_Y));
    ui.zAxisSettings->setAxisData(map.axisData(A3D::AXIS_Z));
    m_map = map;
}

void SettingsDialog::onSettingsAccepted() {
    A3D::ChartAxisData xAxisData = ui.xAxisSettings->axisData();
    A3D::ChartAxisData yAxisData = ui.yAxisSettings->axisData();
    A3D::ChartAxisData zAxisData = ui.zAxisSettings->axisData();
    m_map.setAxisData(A3D::AXIS_X, xAxisData);
    m_map.setAxisData(A3D::AXIS_Y, yAxisData);
    m_map.setAxisData(A3D::AXIS_Z, zAxisData);
    emit mapUpdated(m_map);
}
