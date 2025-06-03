#include "addliindicatorsdialog.h"
#include "ui_addliindicatorsdialog.h"

AddLiIndicatorsDialog::AddLiIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::AddLiIndicatorsDialog),
      m_mode(AM_BY_STEP),
      m_countMode(true) {
	ui->setupUi(this);
	setMode(m_mode);
    setCountOrStepSizeMode(m_countMode);

    ui->stepSizeLabel->setMinimumWidth(ui->stepSizeLabel->fontMetrics().boundingRect("Indicators count").width());
    ui->stepSizeLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

	ui->stepSizeDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui->stepSizeDoubleSpinBox->setMinimum(std::numeric_limits<double>::min());

	ui->fromDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui->fromDoubleSpinBox->setMinimum(std::numeric_limits<double>::lowest());

	ui->toDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui->toDoubleSpinBox->setMinimum(std::numeric_limits<double>::lowest());

	ui->countSpinBox->setMaximum(A3D::MaxIndicators);

    connect(ui->byStepRadioButton, &QRadioButton::clicked, this, &AddLiIndicatorsDialog::onByStepButtonClicked);
    connect(ui->rawEditRadioButton, &QRadioButton::clicked, this, &AddLiIndicatorsDialog::onRawAddButtonClicked);
    connect(ui->countRadioButton, &QRadioButton::clicked, this, &AddLiIndicatorsDialog::onCountButtonClicked);
    connect(ui->stepRadioButton, &QRadioButton::clicked, this, &AddLiIndicatorsDialog::onStepSizeButtonClicked);
    connect(ui->stringPrecisionSpinBox, &QSpinBox::editingFinished, this, &AddLiIndicatorsDialog::onLabelDigitsChanged);
}

void AddLiIndicatorsDialog::onRawAddButtonClicked() {
	setMode(AM_RAW_ADD);
}

void AddLiIndicatorsDialog::onByStepButtonClicked() {
	setMode(AM_BY_STEP);
}

void AddLiIndicatorsDialog::onCountButtonClicked() {
	setCountOrStepSizeMode(true);
}

void AddLiIndicatorsDialog::onStepSizeButtonClicked() {
	setCountOrStepSizeMode(false);
}

void AddLiIndicatorsDialog::onLabelDigitsChanged() {
    int digits = ui->stringPrecisionSpinBox->value();
    ui->rawEditWidget->setStringPrecision(digits);
}

void AddLiIndicatorsDialog::setCountOrStepSizeMode(bool mode) {
	m_countMode = mode;
    ui->stepSizeLabel->setHidden(m_countMode);
    ui->stepSizeDoubleSpinBox->setHidden(m_countMode);
    ui->countSpinBox->setHidden(!m_countMode);
    ui->countLabel->setHidden(!m_countMode);
}

void AddLiIndicatorsDialog::setMode(AddMode mode) {
	m_mode = mode;

    switch(m_mode) {
    default:
    case AM_BY_STEP:
        ui->rawEditWidget->setHidden(true);
        ui->byStepWidget->setHidden(false);
        break;
    case AM_RAW_ADD:
        ui->rawEditWidget->setHidden(false);
        ui->byStepWidget->setHidden(true);
        break;
    }
}

void AddLiIndicatorsDialog::setStyle(A3D::ChartAxisIndicatorStyle style) {
    ui->generalSettings->setStyle(style);
}

std::vector<A3D::ChartAxisIndicator> AddLiIndicatorsDialog::getIndicatorFromCount(size_t count) const {
    std::vector<A3D::ChartAxisIndicator> indicators;
    A3D::ChartAxisIndicatorType type   = ui->majorRadioButton->isChecked() ? A3D::CHAXIND_MAJOR_INDICATOR : A3D::CHAXIND_MINOR_INDICATOR;
    int const stringPrecision          = ui->stringPrecisionSpinBox->value();
    A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();

    float const from = ui->fromDoubleSpinBox->value();
    float const to   = ui->toDoubleSpinBox->value();

    float fInverseIndicatorCount = 1.f / static_cast<float>(count - 1);
    for(size_t i = 0; i < count; ++i) {
        float const val = from + ((to - from) * fInverseIndicatorCount * static_cast<float>(i));
        indicators.push_back(A3D::ChartAxisIndicator(type, val, 0.f, QString::number(val, 'f', stringPrecision), style));
    }

    return indicators;
}

// TODO:  Normalize value when they arrive
std::vector<A3D::ChartAxisIndicator> AddLiIndicatorsDialog::indicators() const {
	std::vector<A3D::ChartAxisIndicator> indicators;
    A3D::ChartAxisIndicatorType type   = ui->majorRadioButton->isChecked() ? A3D::CHAXIND_MAJOR_INDICATOR : A3D::CHAXIND_MINOR_INDICATOR;
    int const stringPrecision          = ui->stringPrecisionSpinBox->value();
	A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();

    switch(m_mode) {
	default:
	case AM_BY_STEP:
		{
			// Copied from A3D/chart.h
            float const from = ui->fromDoubleSpinBox->value();
            float const to   = ui->toDoubleSpinBox->value();
            if(m_countMode) {
                indicators = getIndicatorFromCount(ui->countSpinBox->value());
                return indicators;
            }

            float stepSize              = ui->stepSizeDoubleSpinBox->value();
            size_t const indicatorCount = static_cast<size_t>((to - from) / stepSize);

            if(indicatorCount > A3D::MaxIndicators) {
                indicators = getIndicatorFromCount(A3D::MaxIndicators);
                return indicators;
            }

            indicators.reserve(indicatorCount + 1);

            for(size_t i = 0; i < indicatorCount; ++i) {
                float const val = from + stepSize * static_cast<float>(i);
                indicators.push_back(A3D::ChartAxisIndicator(type, val, 0.f, QString::number(val, 'f', stringPrecision), style));
            }

            float const val = to;
            indicators.push_back(A3D::ChartAxisIndicator(type, val, 0.f, QString::number(val, 'f', stringPrecision), style));
		}

    case AM_RAW_ADD:
        {
            std::vector<float> values;
            values = ui->rawEditWidget->getValues();

            for(size_t i = 0; i < values.size(); i++)
                indicators.push_back(A3D::ChartAxisIndicator(type, values.at(i), 0.f, QString::number(values.at(i), 'f', stringPrecision), style));
        }

		break;
	}

    return indicators;
}

AddLiIndicatorsDialog::~AddLiIndicatorsDialog() {
	delete ui;
}
