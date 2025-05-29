#include "indicatorspreviewwidget.h"

IndicatorsPreviewWidget::IndicatorsPreviewWidget(QWidget* parent)
    : QWidget(parent) {
	ui.setupUi(this);

    ui.previewWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    connect(ui.previewWidget, &QListWidget::doubleClicked, this, &IndicatorsPreviewWidget::onItemDoubleClicked);
    connect(ui.previewWidget, &QListWidget::itemSelectionChanged, this, &IndicatorsPreviewWidget::onItemSelectionChanged);
    connect(ui.addIndicatorsButton, &QPushButton::clicked, this, &IndicatorsPreviewWidget::onAddButtonClicked);
    connect(ui.removeIndicatorsButton, &QPushButton::clicked, this, &IndicatorsPreviewWidget::onRemoveButtonClicked);
    connect(ui.editIndicatorsButton, &QPushButton::clicked, this, &IndicatorsPreviewWidget::onEditIndicartorsClicked);

    ui.removeIndicatorsButton->setEnabled(false);
    ui.editIndicatorsButton->setEnabled(false);
}


void IndicatorsPreviewWidget::addIndicators(std::vector<A3D::ChartAxisIndicator> indicators)
{
	for(A3D::ChartAxisIndicator const& it : indicators)
	{
		m_indicators.push_back(it);

		QString text = it.m_label;

		QListWidgetItem* newLabel = new QListWidgetItem;
		newLabel->setText(text);

		newLabel->setFont(it.m_style.m_labelFont);


		ui.previewWidget->insertItem(m_indicators.size(), newLabel);
	}

}

std::vector<A3D::ChartAxisIndicator> IndicatorsPreviewWidget::indicators() const {
    return m_indicators;
}

void IndicatorsPreviewWidget::onAddButtonClicked() {
    emit addClicked();
}

void IndicatorsPreviewWidget::onRemoveButtonClicked() {
    QList<QListWidgetItem*> itemsToRemove = ui.previewWidget->selectedItems();

    for(int i = ui.previewWidget->count() - 1; i >= 0; --i) {
        QListWidgetItem* item = ui.previewWidget->item(i);
        if(!itemsToRemove.contains(item))
            continue;
        // Qt documentation says that this is the correct way
        delete ui.previewWidget->takeItem(i);
        m_indicators.erase(m_indicators.begin() + i);
    }
}

void IndicatorsPreviewWidget::onItemDoubleClicked(QModelIndex const& index) {
    // The idea is to remove the widget and then re add it modified
    int row = index.row();

    A3D::ChartAxisIndicator indicator = m_indicators.at(row);

    delete ui.previewWidget->takeItem(row);
    m_indicators.erase(m_indicators.begin() + row);

    emit editIndicators({ indicator });
}

void IndicatorsPreviewWidget::onEditIndicartorsClicked() {

    std::vector<A3D::ChartAxisIndicator> indicators;
    QList<QListWidgetItem*> selectedItems = ui.previewWidget->selectedItems();

    for(int i = ui.previewWidget->count() - 1; i >= 0; --i) {
        QListWidgetItem* item = ui.previewWidget->item(i);
        if(!selectedItems.contains(item))
            continue;
        // Qt documentation says that this is the correct way
        indicators.push_back(m_indicators.at(i));
        delete ui.previewWidget->takeItem(i);
        m_indicators.erase(m_indicators.begin() + i);
    }
    emit editIndicators(indicators);
}

void IndicatorsPreviewWidget::onItemSelectionChanged() {
    int count   = ui.previewWidget->selectedItems().count();
    bool result = count > 0;
    if(count == 1) {
        ui.editIndicatorsButton->setText("Edit indicator");
        ui.removeIndicatorsButton->setText("Remove indicator");
    }
    else if(count > 1) {
        ui.editIndicatorsButton->setText("Edit indicators");
        ui.removeIndicatorsButton->setText("Remove indicators");
    }

    ui.editIndicatorsButton->setEnabled(result);
    ui.removeIndicatorsButton->setEnabled(result);
}
