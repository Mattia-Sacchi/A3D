#include "editliindicatorsdialog.h"
#include "ui_editliindicatorsdialog.h"

EditLiIndicatorsDialog::EditLiIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
      ui(new Ui::EditLiIndicatorsDialog) {
    ui->setupUi(this);

    connect(ui->stringPrecisionSpinBox, &QSpinBox::editingFinished, this, &EditLiIndicatorsDialog::onLabelDigitsChanged);
    reset();
    ui->rawEditWidget->setEditingMode(true);
}

void EditLiIndicatorsDialog::setStyleNeeded(bool styleNeeded) {
    ui->generalSettings->setHidden(!styleNeeded);
    ui->indicatorTypeWidget->setHidden(!styleNeeded);
    m_styleNeeded = styleNeeded;
}

void EditLiIndicatorsDialog::onLabelDigitsChanged() {
    int digits = ui->stringPrecisionSpinBox->value();
    ui->rawEditWidget->setStringPrecision(digits);
}

void EditLiIndicatorsDialog::editIndicators(std::vector<A3D::ChartAxisIndicator> const& indicators) {
    for(size_t i = 0; i < indicators.size(); ++i)
        ui->rawEditWidget->addValue(indicators.at(i).m_value);
    m_indicators = indicators;
}

void EditLiIndicatorsDialog::setChartIndicatorsType(A3D::ChartAxisIndicatorType type) {
    ui->indicatorTypeWidget->setType(type);
}

void EditLiIndicatorsDialog::setStyle(A3D::ChartAxisIndicatorStyle style) {
    ui->generalSettings->setStyle(style);
    setStyleNeeded(true);
}

void EditLiIndicatorsDialog::reset() {
    setChartIndicatorsType(A3D::CHAXIND_MAJOR_INDICATOR);
    setStyleNeeded(false);
    m_indicators.clear();
    ui->rawEditWidget->clear();
}

// TODO:  Normalize value when they arrive
std::vector<A3D::ChartAxisIndicator> EditLiIndicatorsDialog::indicators() {
    std::vector<float> values = ui->rawEditWidget->getValues();
    int stringPrecision       = ui->stringPrecisionSpinBox->value();
    for(size_t i = 0; i < values.size(); i++) {
        m_indicators[i].m_value = values[i];
        m_indicators[i].m_label = QString::number(values.at(i), 'f', stringPrecision);
    }
    if(!m_styleNeeded)
        return m_indicators;

    A3D::ChartAxisIndicatorType type   = ui->indicatorTypeWidget->type();
    A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();

    for(A3D::ChartAxisIndicator& it: m_indicators) {
        it.m_type  = type;
        it.m_style = style;
    }

    return m_indicators;
}

EditLiIndicatorsDialog::~EditLiIndicatorsDialog() {
	delete ui;
}
