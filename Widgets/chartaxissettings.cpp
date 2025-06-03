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
    A3D::ChartAxisIndicatorStyle m_style;
    A3D::ChartAxisIndicatorType m_type;
    bool operator==(Info const& i) const { return m_style == i.m_style && m_type == i.m_type; }
    bool operator!=(Info const& i) const { return !(*this == i); }
    Info(A3D::ChartAxisIndicator const& indicator) {
        m_style = indicator.m_style;
        m_type  = indicator.m_type;
    }
};

void ChartAxisSettings::onLinearEditDialogClicked(std::vector<A3D::ChartAxisIndicator> const& indicators) {
    std::vector<Info> infos;
    infos.clear();

    infos.emplace_back(indicators[0]);

    for(size_t i = 0; i < indicators.size() - 1; i++) {
        Info current = Info(indicators[i]);
        Info next    = Info(indicators[i + 1]);

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

    QMessageBox msgBox;
    msgBox.setWindowTitle("Confirmation");
    msgBox.setText("The selected indicator styles are incompatible. Please choose one from the following options:");

    msgBox.setStandardButtons(QMessageBox::Discard);
    msgBox.setDefaultButton(QMessageBox::Discard);

    QMap<QPushButton*, Info*> buttons;

    for(size_t i = 0; i < count; i++) {
        Info& info  = infos[i];
        QString pre = info.m_type == A3D::CHAXIND_MINOR_INDICATOR ? "Minor" : "Major";
        pre += ": ";
        QString buttonName  = pre + info.m_style.m_labelFont.family();
        QPushButton* btn    = msgBox.addButton(buttonName, QMessageBox::AcceptRole);
        QFont font          = info.m_style.m_labelFont;
        FontResolutions res = ChartAxisGeneralSettings::getFontResoulution(font.pointSize());
        font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(res));
        btn->setFont(font);

        QPalette palette = btn->palette();
        palette.setColor(QPalette::WindowText, info.m_style.m_labelColor);
        btn->setPalette(palette);

        buttons[btn] = &info;
    }

    msgBox.exec();

    std::vector<A3D::ChartAxisIndicator> tempIndicators = indicators;

    QPushButton* btn = qobject_cast<QPushButton*>(msgBox.clickedButton());
    if(buttons.contains(btn)) {
        Info* i = buttons[btn];
        for(A3D::ChartAxisIndicator& it: tempIndicators) {
            it.m_type  = i->m_type;
            it.m_style = i->m_style;
        }
    }
    else {
        // Total Roll back
        ui.LinearPreviewWidget->indicatorPreviewWidget()->addIndicators(tempIndicators);
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
