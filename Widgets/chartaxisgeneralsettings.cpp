#include "chartaxisgeneralsettings.h"
#include <QColorDialog>
#include <QFontDialog>

enum FS {
	FS_Low    = 16,
	FS_Medium = 32,
	FS_High   = 64,
};

uint8_t const FontSizes[FR_Count] = { FS_Low, FS_Medium, FS_High };

ChartAxisGeneralSettings::ChartAxisGeneralSettings(QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);
	ui.indicatorColorWidget->setAutoFillBackground(true);

    ui.scaleValueLabel->setMinimumWidth(ui.scaleValueLabel->fontMetrics().boundingRect("1,000").width());
    ui.scaleValueLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    connect(ui.changeIndicatorColorButton, &QPushButton::clicked, this, &ChartAxisGeneralSettings::onChangeIndicatorColorButtonClicked);
    connect(ui.changeLabelColorButton, &QPushButton::clicked, this, &ChartAxisGeneralSettings::onChangeLabelColorButtonClicked);
    connect(ui.changeFontButton, &QPushButton::clicked, this, &ChartAxisGeneralSettings::onChangeFontButtonClicked);
    connect(ui.highResolutionRadioButton, &QRadioButton::clicked, this, &ChartAxisGeneralSettings::onHighResolutionRadioButtonClicked);
    connect(ui.mediumResolutionRadioButton, &QRadioButton::clicked, this, &ChartAxisGeneralSettings::onMediumResolutionRadioButtonClicked);
    connect(ui.lowResolutionRadioButton, &QRadioButton::clicked, this, &ChartAxisGeneralSettings::onLowResolutionRadioButtonClicked);
    connect(ui.resetButton, &QPushButton::clicked, this, &ChartAxisGeneralSettings::onResetButtonClicked);
    connect(ui.scaleSlider, &QSlider::valueChanged, this, &ChartAxisGeneralSettings::onScaleSliderValueChanged);

    ui.highResolutionRadioButton->setChecked(true);
    setResoulution(FR_High);
}

A3D::ChartAxisIndicatorStyle ChartAxisGeneralSettings::style() const {

    A3D::ChartAxisIndicatorStyle style;

    style.m_indicatorColor = ui.indicatorColorWidget->palette().color(ui.indicatorColorWidget->backgroundRole());
    style.m_labelColor     = ui.labelColorExampleLabel->palette().color(QPalette::WindowText);
    QFont font             = ui.labelColorExampleLabel->font();
    font.setPointSize(FontSizes[m_resolution]);
    style.m_labelFont = font;
    style.m_labelSize = ui.scaleSlider->value() / 100.f;

    return style;
}

FontResolutions ChartAxisGeneralSettings::getFontResoulution(size_t fontPointSize)
{

	FontResolutions resolution = FR_High;
	for(size_t i = 0; i < FR_Count; i++)
	{
		if(fontPointSize == FontSizes[i])
			resolution = static_cast<FontResolutions>(i);
	}
	return resolution;
}

size_t ChartAxisGeneralSettings::getDisplaySize(FontResolutions res)
{
	if(res >= FR_Count || res < FR_Low)
		return FontSizes[FR_High];
	return FontSizes[res] / 4;
}

void ChartAxisGeneralSettings::setStyle(A3D::ChartAxisIndicatorStyle style) {
    QPalette palette;
    palette = ui.indicatorColorWidget->palette();
    palette.setColor(ui.indicatorColorWidget->backgroundRole(), style.m_indicatorColor);
    ui.indicatorColorWidget->setPalette(palette);

    palette = ui.labelColorExampleLabel->palette();
    palette.setColor(QPalette::WindowText, style.m_labelColor);
    ui.labelColorExampleLabel->setPalette(palette);

    QFont font       = style.m_labelFont;
    m_resolution = ChartAxisGeneralSettings::getFontResoulution(font.pointSize());

    font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(m_resolution));

    ui.labelFontExampleLabel->setText(font.family());
    ui.labelFontExampleLabel->setFont(font);
    updateFontExampleResoulution();

    ui.scaleSlider->setValue(style.m_labelSize * 100.f);
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
    actualFont.setPointSize(FontSizes[m_resolution] / 4);
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

void ChartAxisGeneralSettings::onResetButtonClicked() {
    ui.scaleSlider->setValue(100);
}

void ChartAxisGeneralSettings::onScaleSliderValueChanged(int value) {
    ui.scaleValueLabel->setText(QString::number(value / 100.f, 'g', 2));
}
