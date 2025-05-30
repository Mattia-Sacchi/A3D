#include "chartaxissettings.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
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

void ChartAxisSettings::onLinearEditDialogAccepted() {
    std::vector<A3D::ChartAxisIndicator> indicators = m_linearEditDialog.indicators();
    ui.LinearPreviewWidget->indicatorPreviewWidget()->addIndicators(indicators);
}

struct Info {
    A3D::ChartAxisIndicatorStyle style;
    A3D::ChartAxisIndicatorType type;
    bool operator==(Info const& i) const { return style == i.style && type == i.type; }
    bool operator!=(Info const& i) const { return !(*this == i); }
    Info() = default;
    Info(A3D::ChartAxisIndicator const& indicator) {
        Info i;
        i.style = indicator.m_style;
        i.type  = indicator.m_type;
    }
};

void ChartAxisSettings::onLinearEditDialogClicked(std::vector<A3D::ChartAxisIndicator> const& indicators) {
    std::vector<Info> infos;
    infos.clear();

    for(size_t i = 0; i < indicators.size() - 1; i++) {
        Info current = Info(indicators[i]);
        Info next    = Info(indicators[i + 1]);

        if(infos.empty()) {
            infos.push_back(current);
            if(current != next)
                infos.push_back(next);
            continue;
        }

        if(current != next && std::find(infos.begin(), infos.end(), next) == infos.end())
            infos.push_back(next);
    }

    if(false) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Confirmation");
        msgBox.setText("The selected indicator styles are incompatible. Please choose one from the following options:");
        msgBox.setStandardButtons(QMessageBox::Discard);
        msgBox.setDefaultButton(QMessageBox::Discard);

        int ret = msgBox.exec();

        if(ret == QMessageBox::Yes) {
            qDebug() << "User clicked Yes";
        }
        else if(ret == QMessageBox::No) {
            qDebug() << "User clicked No";
        }
    }

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
