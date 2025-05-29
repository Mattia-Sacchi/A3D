#include "addliindicatorsdialog.h"
#include "ui_addliindicatorsdialog.h"

AddLiIndicatorsDialog::AddLiIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::AddLiIndicatorsDialog),
      m_mode(AM_BY_STEP),
      m_countMode(true) {
	ui->setupUi(this);
	setMode(m_mode);
    setCountOrStepSizeMode(true);

    connect(ui->byStepRadioButton, &QRadioButton::clicked, this, &AddLiIndicatorsDialog::onByStepButtonClicked);
    connect(ui->rawEditRadioButton, &QRadioButton::clicked, this, &AddLiIndicatorsDialog::onRawAddButtonClicked);
    connect(ui->countRadioButton, &QRadioButton::clicked, this, &AddLiIndicatorsDialog::onCountButtonClicked);
    connect(ui->stepRadioButton, &QRadioButton::clicked, this, &AddLiIndicatorsDialog::onStepSizeButtonClicked);
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

void AddLiIndicatorsDialog::setCountOrStepSizeMode(bool mode) {
	m_countMode = mode;
	ui->stepSizeDoubleSpinBox->setHidden(mode);
	ui->stepSizeDoubleSpinBox->setHidden(mode);
	ui->countSpinBox->setHidden(!mode);
	ui->countLabel->setHidden(!mode);
}

void AddLiIndicatorsDialog::setMode(AddMode mode) {
	m_mode = mode;
    ui->rawEditWidget->setHidden(m_mode);
    ui->byStepWidget->setHidden(m_mode);
}

std::vector<A3D::ChartAxisIndicator> AddLiIndicatorsDialog::getIndicators(size_t count) const {
    float from = ui->fromDoubleSpinBox->value();
    float to   = ui->toDoubleSpinBox->value();
    std::vector<A3D::ChartAxisIndicator> indicators;
    int stringPrecision                = ui->stringPrecisionSpinBox->value();
    A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();
    A3D::ChartAxisIndicatorType type   = ui->majorRadioButton->isChecked() ? A3D::CHAXIND_MAJOR_INDICATOR : A3D::CHAXIND_MINOR_INDICATOR;

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
    int stringPrecision                = ui->stringPrecisionSpinBox->value();
	A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();

    switch(m_mode) {
	default:
	case AM_BY_STEP:
		{
			// Copied from A3D/chart.h
			float from = ui->fromDoubleSpinBox->value();
            float to   = ui->toDoubleSpinBox->value();
            if(m_countMode) {

				size_t count = ui->countSpinBox->value();

                indicators = getIndicators(count);
			}
            else {

                float stepSize              = ui->stepSizeDoubleSpinBox->value();
				size_t const indicatorCount = static_cast<size_t>((to - from) / stepSize);

                if(indicatorCount > A3D::MaxIndicators) // Too many, limit to 1000 points...
                {
                    indicators = getIndicators(A3D::MaxIndicators);
                    break;
				}

                indicators.reserve(indicatorCount + 1);

				for(size_t i = 0; i < indicatorCount; ++i) {
					float const val = from + stepSize * static_cast<float>(i);
                    indicators.push_back(A3D::ChartAxisIndicator(type, val, 0.f, QString::number(val, 'f', stringPrecision), style));
				}

				float const val = to;
                indicators.push_back(A3D::ChartAxisIndicator(type, val, 0.f, QString::number(val, 'f', stringPrecision), style));
			}
		}

    case AM_RAW_ADD:
        {
            std::vector<float> values;
            values = ui->rawEditWidget->values();
        }

		break;
	}

    return indicators;
}

AddLiIndicatorsDialog::~AddLiIndicatorsDialog() {
	delete ui;
}
