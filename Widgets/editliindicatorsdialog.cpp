#include "editliindicatorsdialog.h"
#include "ui_editliindicatorsdialog.h"

EditLiIndicatorsDialog::EditLiIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
      ui(new Ui::EditLiIndicatorsDialog) {
    ui->setupUi(this);

    connect(ui->stringPrecisionWidget, &StringPrecisionWidget::stringPrecisionChanged, this, &EditLiIndicatorsDialog::onLabelDigitsChanged);
    reset();
    ui->rawEditWidget->setEditingMode(true);
}

static size_t getPrecisionFromString(const QString text) {
    size_t dotIndex = text.indexOf('.');

    if(dotIndex < 0) {
        // No decimal point found
        return 0;
    }

    // Extract substring after '.'
    QString decimals = text.mid(dotIndex + 1);

    // Trim any trailing non-digits (e.g. thousands separators or suffixes)
    size_t precision = 0;
    for(size_t i = 0; i < decimals.size(); ++i) {
        if(decimals[i].isDigit())
            ++precision;
        else
            break;
    }

    return precision;
}

void EditLiIndicatorsDialog::setStyleNeeded(bool styleNeeded) {
    ui->generalSettings->setHidden(!styleNeeded);
    ui->indicatorTypeWidget->setHidden(!styleNeeded);
    m_styleNeeded = styleNeeded;
}

void EditLiIndicatorsDialog::onLabelDigitsChanged() {
    size_t digits = ui->stringPrecisionWidget->getPrecision();
    ui->rawEditWidget->setStringPrecision(digits);
}

void EditLiIndicatorsDialog::editIndicators(std::vector<A3D::ChartAxisIndicator> const& indicators) {
    size_t maxStringPrecision = 0;
    for(size_t i = 0; i < indicators.size(); ++i) {
        ui->rawEditWidget->addValue(indicators.at(i).m_value);
        size_t prec = getPrecisionFromString(indicators.at(i).m_label);
        if(prec > maxStringPrecision)
            maxStringPrecision = prec;
    }
    ui->stringPrecisionWidget->setPrecision(maxStringPrecision);
    m_indicators = indicators;
    open();
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
    std::vector<float> values    = ui->rawEditWidget->getValues();
    size_t const stringPrecision = ui->stringPrecisionWidget->getPrecision();
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
