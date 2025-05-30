#include "editliindicatorsdialog.h"
#include "ui_editliindicatorsdialog.h"

EditLiIndicatorsDialog::EditLiIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
      ui(new Ui::EditLiIndicatorsDialog) {
    ui->setupUi(this);

    connect(ui->stringPrecisionSpinBox, &QSpinBox::editingFinished, this, &EditLiIndicatorsDialog::onLabelDigitsChanged);
}

void EditLiIndicatorsDialog::onLabelDigitsChanged() {
    int digits = ui->stringPrecisionSpinBox->value();
    ui->rawEditWidget->setStringPrecision(digits);
}

void EditLiIndicatorsDialog::editIndicators(std::vector<A3D::ChartAxisIndicator> list) {
    for(size_t i = 0; i < list.size(); ++i)
        ui->rawEditWidget->addValue(list.at(i).m_value);
}

void EditLiIndicatorsDialog::setStyle(A3D::ChartAxisIndicatorStyle style) {
    ui->generalSettings->setStyle(style);
}

// TODO:  Normalize value when they arrive
std::vector<A3D::ChartAxisIndicator> EditLiIndicatorsDialog::indicators() const {
	std::vector<A3D::ChartAxisIndicator> indicators;
    A3D::ChartAxisIndicatorType type   = ui->majorRadioButton->isChecked() ? A3D::CHAXIND_MAJOR_INDICATOR : A3D::CHAXIND_MINOR_INDICATOR;
    int stringPrecision                = ui->stringPrecisionSpinBox->value();
	A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();

    std::vector<float> values;
    values = ui->rawEditWidget->getValues();

    for(size_t i = 0; i < values.size(); i++)
        indicators.push_back(A3D::ChartAxisIndicator(type, values.at(i), 0.f, QString::number(values.at(i), 'f', stringPrecision), style));

    return indicators;
}

EditLiIndicatorsDialog::~EditLiIndicatorsDialog() {
	delete ui;
}
