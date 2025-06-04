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

    connect(ui.LinearPreviewWidget->indicatorPreviewWidget(), &IndicatorsPreviewWidget::addClicked, this, &ChartAxisSettings::onLinearAddClicked);
    connect(ui.LinearPreviewWidget->indicatorPreviewWidget(), &IndicatorsPreviewWidget::editIndicators, this, &ChartAxisSettings::onLinearEditDialogClicked);
    connect(&m_linearAddDialog, &QDialog::accepted, this, &ChartAxisSettings::onLinearAddDialogAccepted);
    connect(&m_linearEditDialog, &QDialog::rejected, this, &ChartAxisSettings::onLinearEditDialogRejected);
    connect(&m_linearEditDialog, &QDialog::accepted, this, &ChartAxisSettings::onLinearEditDialogAccepted);
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

void ChartAxisSettings::linearEditFinished() {
    std::vector<A3D::ChartAxisIndicator> indicators = m_linearEditDialog.indicators();
    ui.LinearPreviewWidget->indicatorPreviewWidget()->addIndicators(indicators);
}

void ChartAxisSettings::onLinearEditDialogAccepted() {
    linearEditFinished();
}

void ChartAxisSettings::onLinearEditDialogRejected() {
    linearEditFinished();
}

void ChartAxisSettings::onLinearEditDialogClicked(std::vector<A3D::ChartAxisIndicator> const& indicators) {
    std::vector<IndicatorInfo> infos;
    infos.clear();

    m_linearEditDialog.reset();

    infos.emplace_back(indicators[0]);

    for(size_t i = 0; i < indicators.size() - 1; i++) {
        IndicatorInfo current = IndicatorInfo(indicators[i]);
        IndicatorInfo next    = IndicatorInfo(indicators[i + 1]);

        if(current != next && std::find(infos.begin(), infos.end(), next) == infos.end())
            infos.push_back(next);
    }
    size_t count = infos.size();

    if(count == 1) {
        // Ho un solo tipo di indicatore quindi li posso pushare in edit
        m_linearEditDialog.editIndicators(indicators);
        m_linearEditDialog.open();
        return;
    }

    // Insert here dialog
    IncompatibilityDialog dialog(this, infos);
    dialog.exec();

    switch(dialog.getUserChoice()) {
    case IncompatibilityDialog::UC_CHOOSE:
        {
            IndicatorInfo info = dialog.getInfo();
            m_linearEditDialog.setStyle(info.m_style);
            m_linearEditDialog.setChartIndicatorsType(info.m_type);
        }
        break;
    case IncompatibilityDialog::UC_CONTINUE:
        break;
    default:
    case IncompatibilityDialog::UC_DISCARD:
        // Total rollback
        ui.LinearPreviewWidget->indicatorPreviewWidget()->addIndicators(indicators);
        return;
    }
    m_linearEditDialog.editIndicators(indicators);
    m_linearEditDialog.open();
}

void ChartAxisSettings::onLinearAddDialogAccepted() {
	std::vector<A3D::ChartAxisIndicator> indicators = m_linearAddDialog.indicators();
	ui.LinearPreviewWidget->indicatorPreviewWidget()->addIndicators(indicators);
}

void ChartAxisSettings::onLinearAddClicked() {
    m_linearAddDialog.open();

    m_linearAddDialog.setStyle(ui.chartAxisGeneralSettings->style());
}

void ChartAxisSettings::onLinearInterpolatedRadioButtonClicked() {
    setChartAxisType(A3D::CHAXIS_LINEAR_INTERPOLATED);
}

void ChartAxisSettings::onEnumeratedRadioButtonClicked() {
    setChartAxisType(A3D::CHAXIS_ENUMERATED);
}
