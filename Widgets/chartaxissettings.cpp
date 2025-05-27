#include "chartaxissettings.h"
#include <QColorDialog>
#include <QFontDialog>

static uint8_t FontSizes[FR_Count] = { 16, 32, 64 };

ChartAxisSettings::ChartAxisSettings(QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);
	ui.indicatorColorWidget->setAutoFillBackground(true);
}

void ChartAxisSettings::on_changeIndicatorColorButton_clicked() {
	QColor color = QColorDialog::getColor();

	if(!color.isValid())
		return;

	QPalette palette = ui.indicatorColorWidget->palette();
	palette.setColor(ui.indicatorColorWidget->backgroundRole(), color);
	ui.indicatorColorWidget->setPalette(palette);
}

void ChartAxisSettings::on_changeLabelColorButton_clicked() {
	QColor color = QColorDialog::getColor();

	if(!color.isValid())
		return;
	QPalette palette = ui.labelColorExampleLabel->palette();
	palette.setColor(QPalette::WindowText, color);
	ui.labelColorExampleLabel->setPalette(palette);
}

void ChartAxisSettings::on_changeFontButton_clicked() {

	bool result      = false;
	QFont actualFont = ui.labelFontExampleLabel->font();
	QFont font       = QFontDialog::getFont(&result, actualFont);

	if(!result)
		return;

	ui.labelFontExampleLabel->setText(font.family());
	ui.labelFontExampleLabel->setFont(font);
	updateFontExampleResoulution();
}

void ChartAxisSettings::setResoulution(FontResolutions res) {
	m_resoulution = res;
	updateFontExampleResoulution();
}

void ChartAxisSettings::updateFontExampleResoulution() {
	QFont actualFont = ui.labelFontExampleLabel->font();
	actualFont.setPointSize(FontSizes[m_resoulution]);
	ui.labelFontExampleLabel->setFont(actualFont);
}

void ChartAxisSettings::on_highResolutionRadioButton_clicked() {
	setResoulution(FR_High);
}

void ChartAxisSettings::on_mediumResolutionRadioButton_clicked() {
	setResoulution(FR_Medium);
}

void ChartAxisSettings::on_lowResolutionRadioButton_clicked() {
	setResoulution(FR_Low);
}
