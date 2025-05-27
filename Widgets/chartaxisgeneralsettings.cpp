#include "chartaxisgeneralsettings.h"
#include <QColorDialog>
#include <QFontDialog>

static uint8_t FontSizes[FR_Count] = { 16, 32, 64 };

ChartAxisGeneralSettings::ChartAxisGeneralSettings(QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);
	ui.indicatorColorWidget->setAutoFillBackground(true);

	connect(ui.changeIndicatorColorButton,&QPushButton::clicked,this, &ChartAxisGeneralSettings::onChangeIndicatorColorButtonClicked);

    connect(ui.changeLabelColorButton,&QPushButton::clicked,this, &ChartAxisGeneralSettings::onChangeLabelColorButtonClicked);

    connect(ui.changeFontButton,&QPushButton::clicked,this, &ChartAxisGeneralSettings::onChangeFontButtonClicked);

    connect(ui.highResolutionRadioButton,&QRadioButton::clicked,this, &ChartAxisGeneralSettings::onHighResolutionRadioButtonClicked);

    connect(ui.mediumResolutionRadioButton,&QRadioButton::clicked,this, &ChartAxisGeneralSettings::onMediumResolutionRadioButtonClicked);

    connect(ui.changeFontButton,&QRadioButton::clicked,this, &ChartAxisGeneralSettings::onLowResolutionRadioButtonClicked);
	
}

void ChartAxisGeneralSettings::onChangeIndicatorColorButtonClicked() {
	QColor color = QColorDialog::getColor();

	if(!color.isValid())
		return;

	QPalette palette = ui.indicatorColorWidget->palette();
	palette.setColor(ui.indicatorColorWidget->backgroundRole(), color);
	ui.indicatorColorWidget->setPalette(palette);
}

void ChartAxisGeneralSettings::onChangeLabelColorButtonClicked() {
	QColor color = QColorDialog::getColor();

	if(!color.isValid())
		return;
	QPalette palette = ui.labelColorExampleLabel->palette();
	palette.setColor(QPalette::WindowText, color);
	ui.labelColorExampleLabel->setPalette(palette);
}

void ChartAxisGeneralSettings::onChangeFontButtonClicked() {

	bool result      = false;
	QFont actualFont = ui.labelFontExampleLabel->font();
	QFont font       = QFontDialog::getFont(&result, actualFont);

	if(!result)
		return;

	ui.labelFontExampleLabel->setText(font.family());
	ui.labelFontExampleLabel->setFont(font);
	updateFontExampleResoulution();
}

void ChartAxisGeneralSettings::setResoulution(FontResolutions res) {
	m_resolution = res;
	updateFontExampleResoulution();
}

void ChartAxisGeneralSettings::updateFontExampleResoulution() {
	QFont actualFont = ui.labelFontExampleLabel->font();
	actualFont.setPointSize(FontSizes[m_resolution]);
	ui.labelFontExampleLabel->setFont(actualFont);
}

void ChartAxisGeneralSettings::onHighResolutionRadioButtonClicked() {
	setResoulution(FR_High);
}

void ChartAxisGeneralSettings::onMediumResolutionRadioButtonClicked() {
	setResoulution(FR_Medium);
}

void ChartAxisGeneralSettings::onLowResolutionRadioButtonClicked() {
	setResoulution(FR_Low);
}
