#include "linearindicatorssettingswidget.h"

LinearIndicatorsSettingsWidget::LinearIndicatorsSettingsWidget(QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    ui.maxValueDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui.minValueDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui.maxValueDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
    ui.minValueDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
}

IndicatorsPreviewWidget * LinearIndicatorsSettingsWidget::indicatorPreviewWidget() const{
    return ui.previewWidget;
}
