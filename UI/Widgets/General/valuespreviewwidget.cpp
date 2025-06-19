#include "valuespreviewwidget.h"
#include <QDoubleSpinBox>
#include <QListWidget>

ValuesPreviewWidget::ValuesPreviewWidget(QWidget* parent)
    : GeneralPreview(parent) {
    GeneralPreview::onItemSelectionChanged();

    connect(ui->addIndicatorsButton, &QPushButton::clicked, this, &ValuesPreviewWidget::onAddButtonClicked);
}

void ValuesPreviewWidget::setStringPrecision(size_t prec) {
    for(size_t i = 0; i < ui->previewWidget->count(); i++) {
        QListWidgetItem* item      = ui->previewWidget->item(i);
        QWidget* tempDoubleSpinBox = ui->previewWidget->itemWidget(item);
        qobject_cast<QDoubleSpinBox*>(tempDoubleSpinBox)->setDecimals(prec);
    }
}

std::vector<float> ValuesPreviewWidget::getValues() const {
    std::vector<float> values;

    for(size_t i = 0; i < ui->previewWidget->count(); i++) {
        QListWidgetItem* item      = ui->previewWidget->item(i);
        QWidget* tempDoubleSpinBox = ui->previewWidget->itemWidget(item);
        values.push_back(qobject_cast<QDoubleSpinBox*>(tempDoubleSpinBox)->value());
    }

    return values;
}

void ValuesPreviewWidget::addValues(std::vector<float> values) {
    for(int i = 0; i < values.size(); i++) {
		addValue(values.at(i));
	}
}

void ValuesPreviewWidget::clear() {
    ui->previewWidget->clear();
}

void ValuesPreviewWidget::addValue(float value) {

    QListWidgetItem* newWidget    = new QListWidgetItem(ui->previewWidget);
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(ui->previewWidget);

    doubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    doubleSpinBox->setMinimum(std::numeric_limits<double>::lowest());
    doubleSpinBox->setValue(value);
    newWidget->setSizeHint(doubleSpinBox->sizeHint());

    ui->previewWidget->setItemWidget(newWidget, doubleSpinBox);
    ui->previewWidget->addItem(newWidget);
}

void ValuesPreviewWidget::onAddButtonClicked() {

    float defaultValue = 0.f;
    if(ui->previewWidget->count()) {
        QListWidgetItem* item      = ui->previewWidget->item(0);
        QWidget* tempDoubleSpinBox = ui->previewWidget->itemWidget(item);
        defaultValue               = qobject_cast<QDoubleSpinBox*>(tempDoubleSpinBox)->value() + 1;
	}
	addValue(defaultValue);
}

void ValuesPreviewWidget::onRemoveButtonClicked() {
    QList<QListWidgetItem*> itemsToRemove = ui->previewWidget->selectedItems();

    for(int i = ui->previewWidget->count() - 1; i >= 0; --i) {
        QListWidgetItem* item = ui->previewWidget->item(i);
        if(!itemsToRemove.contains(item))
            continue;
        delete ui->previewWidget->takeItem(i);
    }
}

bool ValuesPreviewWidget::isAddEnabled() const {
    return true;
}

bool ValuesPreviewWidget::isEditEnabled() const {
    return false;
}

bool ValuesPreviewWidget::isMultiEditEnabled() const {
    return false;
}

bool ValuesPreviewWidget::isRemoveEnabled() const {
    return true;
}
