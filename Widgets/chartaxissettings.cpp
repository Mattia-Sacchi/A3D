#include "chartaxissettings.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
#include "incompatibilitydialog.h"

ChartAxisSettings::ChartAxisSettings(QWidget* parent)
    : QWidget(parent),
      m_type(A3D::CHAXIS_LINEAR_INTERPOLATED) {
	ui.setupUi(this);

    connect(ui.enumeratedRadioButton, &QRadioButton::clicked, this, &ChartAxisSettings::onEnumeratedRadioButtonClicked);
    connect(ui.linearInterpolatedRadioButton, &QRadioButton::clicked, this, &ChartAxisSettings::onLinearInterpolatedRadioButtonClicked);
    setChartAxisType(m_type);
}

void ChartAxisSettings::setChartAxisType(A3D::ChartAxisType type) {
    switch(m_type = type) {
    case A3D::CHAXIS_ENUMERATED:
        ui.EnumeratedPreviewWidget->show();
        ui.LinearPreviewWidget->hide();
        break;
    default:
    case A3D::CHAXIS_LINEAR_INTERPOLATED:
        ui.EnumeratedPreviewWidget->hide();
        ui.LinearPreviewWidget->show();
        break;
    }
}

A3D::ChartAxisIndicatorStyle ChartAxisSettings::style() const {
    return ui.chartAxisGeneralSettings->style();
}

void ChartAxisSettings::onLinearInterpolatedRadioButtonClicked() {
    setChartAxisType(A3D::CHAXIS_LINEAR_INTERPOLATED);
}

void ChartAxisSettings::onEnumeratedRadioButtonClicked() {
    setChartAxisType(A3D::CHAXIS_ENUMERATED);
}
