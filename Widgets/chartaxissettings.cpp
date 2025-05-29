#include "chartaxissettings.h"
#include <QColorDialog>
#include <QFontDialog>

ChartAxisSettings::ChartAxisSettings(QWidget* parent)
    : QWidget(parent),
      m_type(A3D::CHAXIS_LINEAR_INTERPOLATED) {
	ui.setupUi(this);

    connect(ui.enumeratedRadioButton, &QRadioButton::clicked, this, &ChartAxisSettings::onEnumeratedRadioButtonClicked);
    connect(ui.linearInterpolatedRadioButton, &QRadioButton::clicked, this, &ChartAxisSettings::onLinearInterpolatedRadioButtonClicked);
    setChartAxisType(m_type);

    connect(ui.LinearPreviewWidget->indicatorPreviewWidget(), &IndicatorsPreviewWidget::addClicked, this, &ChartAxisSettings::onLinearAddClicked);

    connect(&m_linearAddDialog, &QDialog::accepted, this, &ChartAxisSettings::onLinearAddDialogAccepted);
}

void ChartAxisSettings::setChartAxisType(A3D::ChartAxisType type) {
    switch(type) {
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

void ChartAxisSettings::onLinearAddDialogAccepted()
{
	std::vector<A3D::ChartAxisIndicator> indicators = m_linearAddDialog.indicators();

	ui.LinearPreviewWidget->indicatorPreviewWidget()->addIndicators(indicators);
}


void ChartAxisSettings::onLinearAddClicked() {

    // TODO: Reset the dialog
    m_linearAddDialog.open();
}

void ChartAxisSettings::onLinearInterpolatedRadioButtonClicked() {
    setChartAxisType(A3D::CHAXIS_LINEAR_INTERPOLATED);
}

void ChartAxisSettings::onEnumeratedRadioButtonClicked() {
    setChartAxisType(A3D::CHAXIS_ENUMERATED);
}
