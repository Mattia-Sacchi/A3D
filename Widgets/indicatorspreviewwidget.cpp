#include "indicatorspreviewwidget.h"
#include "chartaxisgeneralsettings.h"

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

std::vector<float> IndicatorsPreviewWidget::values() const {
    std::vector<float> values;
    values.reserve(m_indicators.size());
    for(A3D::ChartAxisIndicator const& it: m_indicators) {
        values.push_back(it.m_value);
    }
    return values;
}

void IndicatorsPreviewWidget::sort() {
    addIndicators(m_indicators);
}

void IndicatorsPreviewWidget::addIndicators(std::vector<A3D::ChartAxisIndicator> indicators) {

    for(A3D::ChartAxisIndicator const& it: indicators) {

        float value = it.m_value;

        if(std::find_if(
               m_indicators.begin(), m_indicators.end(),
               [value](A3D::ChartAxisIndicator const& it) -> bool {
                   return it.m_value == value;
               }
           )
           != m_indicators.end())
            continue;

        m_indicators.push_back(it);
	}

    std::sort(m_indicators.begin(), m_indicators.end(), [](A3D::ChartAxisIndicator const& a, A3D::ChartAxisIndicator const& b) -> bool {
        return a.m_value < b.m_value;
    });

    ui.previewWidget->clear();
    for(A3D::ChartAxisIndicator const& it: m_indicators) {
        QString text = it.m_label;

        QListWidgetItem* newLabel = new QListWidgetItem;
        newLabel->setText(text);

        QFont font = it.m_style.m_labelFont;

        FontResolutions res = ChartAxisGeneralSettings::getFontResoulution(font.pointSize());
        font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(res));

        newLabel->setFont(font);
        ui.previewWidget->insertItem(ui.previewWidget->count(), newLabel);
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
