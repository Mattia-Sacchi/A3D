#include "indicatorspreviewwidget.h"
#include "chartaxisgeneralsettings.h"
#include <QPainter>

class ColoredFrame : public QFrame {
public:
    explicit ColoredFrame(QWidget* parent = nullptr, QColor borderColor = Qt::black, size_t borderSize = 2)
        : QFrame(parent),
          m_borderColor(borderColor),
          m_borderSize(borderSize),
          m_label(nullptr) {
        setLayout(new QVBoxLayout);
        layout()->setSpacing(0);
        size_t px = borderSize * 2;
        layout()->setContentsMargins(px, px, px, px);
    }

    void setLabel(QLabel* label) {
        if(m_label) {
            layout()->removeWidget(m_label);
            delete m_label;
        }
        layout()->addWidget(m_label = label);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QFrame::paintEvent(event);

        // Disegna il bordo
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(m_borderColor, m_borderSize)); // Spessore 2px
        painter.drawRect(rect().adjusted(1, 1, -2, -2));
    }

private:
    QColor m_borderColor;
    size_t m_borderSize;
    QLabel* m_label;
};

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

    ui.previewWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.editIndicatorsButton->setEnabled(false);
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

    ui.previewWidget->clear();

    for(A3D::ChartAxisIndicator const& it: m_indicators) {

        QString text = it.m_label;

        QListWidgetItem* newLabel = new QListWidgetItem(ui.previewWidget);
        size_t width              = 2;
        if(it.m_type == A3D::CHAXIND_MAJOR_INDICATOR)
            width = 4;

        ColoredFrame* frame = new ColoredFrame(ui.previewWidget, it.m_style.m_indicatorColor, width);

        QLabel* label = new QLabel(frame);

        label->setText(text);

        QPalette palette = label->palette();
        label->setForegroundRole(QPalette::WindowText);
        palette.setColor(QPalette::WindowText, it.m_style.m_labelColor);
        label->setPalette(palette);

        QFont font          = it.m_style.m_labelFont;
        FontResolutions res = ChartAxisGeneralSettings::getFontResoulution(font.pointSize());
        font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(res));
        label->setFont(font);

        frame->setLabel(label);

        // Aumento artificialmente l'altezza per starci dentro
        newLabel->setSizeHint(frame->sizeHint());

        size_t height = newLabel->sizeHint().height();
        if(height + 20 >= ui.previewWidget->minimumHeight())
            ui.previewWidget->setMinimumHeight(height + 20);

        ui.previewWidget->setItemWidget(newLabel, frame);
        ui.previewWidget->addItem(newLabel);
    }

    ui.previewWidget->viewport()->update();
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
