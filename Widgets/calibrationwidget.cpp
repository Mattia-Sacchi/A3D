#include "calibrationwidget.h"

CalibrationWidget::CalibrationWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

void CalibrationWidget::on_settingsButton_clicked()
{
    m_settingsDialog.exec();
}

