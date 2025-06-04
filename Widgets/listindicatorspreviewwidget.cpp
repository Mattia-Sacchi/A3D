#include "listindicatorspreviewwidget.h"
#include "chartaxisgeneralsettings.h"
#include <QPainter>
#include "customwidgets.h"

ListIndicatorsPreviewWidget::ListIndicatorsPreviewWidget(QWidget* parent)
    : QWidget(parent) {
	ui.setupUi(this);

    ui.previewWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    connect(ui.previewWidget, &QListWidget::doubleClicked, this, &ListIndicatorsPreviewWidget::onItemDoubleClicked);
    connect(ui.previewWidget, &QListWidget::itemSelectionChanged, this, &ListIndicatorsPreviewWidget::onItemSelectionChanged);
    connect(ui.addIndicatorsButton, &QPushButton::clicked, this, &ListIndicatorsPreviewWidget::onAddButtonClicked);
    connect(ui.removeIndicatorsButton, &QPushButton::clicked, this, &ListIndicatorsPreviewWidget::onRemoveButtonClicked);
    connect(ui.editIndicatorsButton, &QPushButton::clicked, this, &ListIndicatorsPreviewWidget::onEditIndicartorsClicked);

    connect(&m_dialog, &QDialog::accepted, this, &ListIndicatorsPreviewWidget::onEditAccepted);

    ui.removeIndicatorsButton->setEnabled(false);
    ui.editIndicatorsButton->setEnabled(false);
}

void ListIndicatorsPreviewWidget::addIndicator(A3D::ChartAxisIndicator const& indicator) {
    size_t index = ui.previewWidget->count();
    QString text = indicator.m_label;

    QListWidgetItem* newWidget = new QListWidgetItem(ui.previewWidget);
    size_t width               = MinorWidth;
    if(indicator.m_type == A3D::CHAXIND_MAJOR_INDICATOR)
        width = MajorWidth;

    CustomFrame* frame = new CustomFrame(ui.previewWidget, indicator.m_style.m_indicatorColor, width);

    frame->setValues(text, index);

    QLabel& label = frame->label();

    QPalette palette = label.palette();
    label.setForegroundRole(QPalette::WindowText);
    palette.setColor(QPalette::WindowText, indicator.m_style.m_labelColor);
    label.setPalette(palette);

    QFont font          = indicator.m_style.m_labelFont;
    FontResolutions res = ChartAxisGeneralSettings::getFontResoulution(font.pointSize());
    font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(res));
    label.setFont(font);

    // Aumento artificialmente l'altezza per starci dentro
    newWidget->setSizeHint(frame->sizeHint());

    size_t height = newWidget->sizeHint().height() * 3;
    if(height + 20 >= ui.previewWidget->minimumHeight())
        ui.previewWidget->setMinimumHeight(height + 20);

    ui.previewWidget->setItemWidget(newWidget, frame);
    ui.previewWidget->addItem(newWidget);

    m_indicators.push_back(indicator);
}

void ListIndicatorsPreviewWidget::addIndicators(std::vector<A3D::ChartAxisIndicator> const& indicators) {

    for(A3D::ChartAxisIndicator const& it: indicators) {

        float value = it.m_value;

        if(std::find_if(
               m_indicators.begin(), m_indicators.end(),
               [value](A3D::ChartAxisIndicator const& it) -> bool {
                   return std::abs(it.m_value - value) < std::numeric_limits<float>::min();
               }
           )
           != m_indicators.end())
            continue;

        m_indicators.push_back(it);
	}

    std::sort(m_indicators.begin(), m_indicators.end(), [](A3D::ChartAxisIndicator const& a, A3D::ChartAxisIndicator const& b) -> bool {
        return a.m_value < b.m_value;
    });
    // Devo farlo comunque, perché è come mi arrivano

    ui.previewWidget->clear();

    for(A3D::ChartAxisIndicator const& it: m_indicators)
        addIndicator(it);
}

std::vector<A3D::ChartAxisIndicator> ListIndicatorsPreviewWidget::indicators() const {
    return m_indicators;
}

void ListIndicatorsPreviewWidget::onAddButtonClicked() {
    emit addClicked();
}

void ListIndicatorsPreviewWidget::onRemoveButtonClicked() {
    QList<QListWidgetItem*> itemsToRemove = ui.previewWidget->selectedItems();

    for(int i = ui.previewWidget->count() - 1; i >= 0; --i) {
        QListWidgetItem* item = ui.previewWidget->item(i);
        if(!itemsToRemove.contains(item))
            continue;
        // Qt documentation says that this is the correct way
        delete ui.previewWidget->takeItem(i);
        m_indicators.erase(m_indicators.begin() + i);
    }

    for(size_t i = 0; i < ui.previewWidget->count(); i++) {
        QWidget* w               = ui.previewWidget->itemWidget(ui.previewWidget->item(i));
        CustomFrame* customFrame = qobject_cast<CustomFrame*>(w);
        customFrame->setNumber(i);
    }
}

void ListIndicatorsPreviewWidget::onItemDoubleClicked(QModelIndex const& index) {
    // The idea is to remove the widget and then re add it modified
    size_t row = index.row();
    m_dialog.editIndicator({ row, m_indicators[row] });
    m_dialog.open();
}

void ListIndicatorsPreviewWidget::onEditIndicartorsClicked() {

    QList<QListWidgetItem*> selectedItems = ui.previewWidget->selectedItems();

    if(selectedItems.count() > 1 || !selectedItems.count())
        return;

    A3D::ChartAxisIndicator indicator;

    QListWidgetItem* current = selectedItems.first();

    for(size_t i = 0; i < ui.previewWidget->count(); i++) {
        QListWidgetItem* item = ui.previewWidget->item(i);
        if(item == current) {
            m_dialog.editIndicator({ i, m_indicators[i] });
            m_dialog.open();

            return;
        }
    }
}

void ListIndicatorsPreviewWidget::onEditAccepted() {
    EditEnumDialog::IndicatorIterator it = m_dialog.indicator();

    QWidget* w               = ui.previewWidget->itemWidget(ui.previewWidget->item(it.index));
    CustomFrame* customFrame = qobject_cast<CustomFrame*>(w);
    QString text             = it.indicator.m_label;
    customFrame->setText(text);

    size_t width = MinorWidth;
    if(it.indicator.m_type == A3D::CHAXIND_MAJOR_INDICATOR)
        width = MajorWidth;

    customFrame->setBorder(it.indicator.m_style.m_indicatorColor, width);

    QLabel& label = customFrame->label();

    QPalette palette = label.palette();
    label.setForegroundRole(QPalette::WindowText);
    palette.setColor(QPalette::WindowText, it.indicator.m_style.m_labelColor);
    label.setPalette(palette);

    QFont font          = it.indicator.m_style.m_labelFont;
    FontResolutions res = ChartAxisGeneralSettings::getFontResoulution(font.pointSize());
    font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(res));
    label.setFont(font);
}

void ListIndicatorsPreviewWidget::onItemSelectionChanged() {
    int count   = ui.previewWidget->selectedItems().count();
    bool result = count > 0;
    if(count == 1)
        ui.removeIndicatorsButton->setText("Remove indicator");
    else if(count > 1)
        ui.removeIndicatorsButton->setText("Remove indicators");

    ui.editIndicatorsButton->setEnabled(count == 1);
    ui.removeIndicatorsButton->setEnabled(result);
}
