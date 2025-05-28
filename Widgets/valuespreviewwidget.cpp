#include "valuespreviewwidget.h"

ValuesPreviewWidget::ValuesPreviewWidget(QWidget* parent)
    : QWidget(parent) {
	ui.setupUi(this);

    ui.previewWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    connect(ui.previewWidget, &QListWidget::itemSelectionChanged, this, &ValuesPreviewWidget::onItemSelectionChanged);
    connect(ui.addValueButton, &QPushButton::clicked, this, &ValuesPreviewWidget::onAddButtonClicked);
    connect(ui.removeValueButton, &QPushButton::clicked, this, &ValuesPreviewWidget::onRemoveButtonClicked);

    ui.removeValueButton->setEnabled(false);
}

std::vector<float> ValuesPreviewWidget::values() const {
    return m_values;
}

void ValuesPreviewWidget::addValues(std::vector<float> values)
{
	for(int i = 0; i < values.size(); i++)
	{
		addValue(values.at(i));
	}
}

void ValuesPreviewWidget::addValue(float value)
{
	m_values.push_back(value);

}

void ValuesPreviewWidget::onAddButtonClicked() {
    QString text = QString("Example %1").arg(m_values.size() + 1);

    QListWidgetItem* newLabel = new QListWidgetItem;
    newLabel->setText(text);

    ui.previewWidget->insertItem(m_values.size(), newLabel);
}

void ValuesPreviewWidget::onRemoveButtonClicked() {
    QList<QListWidgetItem*> itemsToRemove = ui.previewWidget->selectedItems();

    for(int i = ui.previewWidget->count() - 1; i >= 0; --i) {
        QListWidgetItem* item = ui.previewWidget->item(i);
        if(!itemsToRemove.contains(item))
            continue;
        // Qt documentation says that this is the correct way
        delete ui.previewWidget->takeItem(i);
        m_values.erase(m_values.begin() + i);
    }
}



void ValuesPreviewWidget::onItemSelectionChanged() {
    int count   = ui.previewWidget->selectedItems().count();
    bool result = count > 0;
    if(count == 1) {
        ui.removeValueButton->setText("Remove value");
    }
    else if(count > 1) {
        ui.removeValueButton->setText("Remove values");
    }

    ui.removeValueButton->setEnabled(result);
}
