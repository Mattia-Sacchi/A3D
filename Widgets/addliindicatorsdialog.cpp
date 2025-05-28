#include "addliindicatorsdialog.h"
#include "ui_addliindicatorsdialog.h"


AddLiIndicatorsDialog::AddLiIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::AddLiIndicatorsDialog),
	  m_mode(AM_BY_STEP){
	ui->setupUi(this);
	setMode(m_mode);

	connect(ui->byStepRadioButton, &QRadioButton::clicked, this , &AddLiIndicatorsDialog::onByStepButtonClicked);
	connect(ui->rawEditRadioButton, &QRadioButton::clicked, this , &AddLiIndicatorsDialog::onRawAddButtonClicked);
	connect(ui->countRadioButton, &QRadioButton::clicked, this , &AddLiIndicatorsDialog::onCountButtonClicked);
	connect(ui->stepRadioButton, &QRadioButton::clicked, this , &AddLiIndicatorsDialog::onStepSizeButtonClicked);
}

void AddLiIndicatorsDialog::onRawAddButtonClicked() {
	setMode(AM_RAW_ADD);
}

void AddLiIndicatorsDialog::onByStepButtonClicked(){
	setMode(AM_BY_STEP);
}

void AddLiIndicatorsDialog::onCountButtonClicked()
{
	setCountOrStepSizeMode(true);

}


void AddLiIndicatorsDialog::onStepSizeButtonClicked()
{
	setCountOrStepSizeMode(false);
}

void AddLiIndicatorsDialog::setCountOrStepSizeMode(bool mode){
	m_countMode = mode;
	ui->stepSizeDoubleSpinBox->setHidden(mode);
	ui->stepSizeDoubleSpinBox->setHidden(mode);
	ui->countSpinBox->setHidden(!mode);
	ui->countLabel->setHidden(!mode);
}


void AddLiIndicatorsDialog::setMode(AddMode mode){
	m_mode = mode;
	switch(m_mode)
	{
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

// TODO:  Normalize value when they arrive
std::vector<A3D::ChartAxisIndicator> AddLiIndicatorsDialog::indicators() const
{
	std::vector<float> values;
	std::vector<A3D::ChartAxisIndicator> indicators;

	A3D::ChartAxisIndicatorType type = ui->majorRadioButton->isChecked() ? A3D::CHAXIND_MAJOR_INDICATOR : A3D::CHAXIND_MINOR_INDICATOR;
	int stringPrecision = ui->stringPrecisionSpinBox->value();
	A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();

	switch(m_mode)
	{
	default:
	case AM_BY_STEP:
		{
			// Copied from A3D/chart.h
			float from = ui->fromDoubleSpinBox->value();
			float to = ui->toDoubleSpinBox->value();
			if(m_countMode)
			{

				size_t count = ui->countSpinBox->value();

				values.reserve(count);


				float fInverseIndicatorCount = 1.f / static_cast<float>(count - 1);
				for(size_t i = 0; i < count; ++i) {
					float const val = from + ((to - from) * fInverseIndicatorCount * static_cast<float>(i));
					indicators.push_back(A3D::ChartAxisIndicator(type, val, 0.f, QString::number(val, 'f', stringPrecision), style));
				}

			}
			else
			{

				float stepSize = ui->stepSizeDoubleSpinBox->value();
				size_t const indicatorCount = static_cast<size_t>((to - from) / stepSize);

				if(indicatorCount > 1000) // Too many, limit to 1000 points...
				{
					//addEquidistantIndicatorsByIndicatorCount(from, to, 1000, stringPrecision, type);
					// AAAH IDK what to do in this case
					return;
				}

				m_indicators.reserve(m_indicators.size() + indicatorCount + 1);

				for(size_t i = 0; i < indicatorCount; ++i) {
					float const val = from + stepSize * static_cast<float>(i);
					m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
				}

				float const val = to;
				m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));

			}
		}

	case AM_RAW_ADD:
		values = ui->rawEditWidget->values();
		break;
	}
}



AddLiIndicatorsDialog::~AddLiIndicatorsDialog() {
	delete ui;
}
