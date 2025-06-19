#include "generalpreview.h"
#include "ui_generalpreview.h"
#include "customframe.h"

GeneralPreview::GeneralPreview(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::GeneralPreview) {
	ui->setupUi(this);

    ui->previewWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(ui->previewWidget, &QListWidget::itemSelectionChanged, this, &GeneralPreview::onItemSelectionChanged);
    connect(ui->removeIndicatorsButton, &QPushButton::clicked, this, &GeneralPreview::onRemoveButtonClicked);
}

GeneralPreview::~GeneralPreview() {
	delete ui;
}

void GeneralPreview::onRemoveButtonClicked() {
    QList<QListWidgetItem*> itemsToRemove = ui->previewWidget->selectedItems();

    for(size_t i = ui->previewWidget->count() - 1; i >= 0; --i) {
        QListWidgetItem* item = ui->previewWidget->item(i);
        if(!itemsToRemove.contains(item))
            continue;
        delete ui->previewWidget->takeItem(i);
        emit removedItem(i);
    }
}

void GeneralPreview::onItemSelectionChanged() {

    size_t count         = ui->previewWidget->selectedItems().count();
    bool isCountSingle   = count == 1;
    bool isCountMultiple = count > 1;

    ui->addIndicatorsButton->setEnabled(isAddEnabled());

    if(isEditEnabled()) {
        ui->editIndicatorsButton->setEnabled(true);
        if(isCountSingle)
            ui->editIndicatorsButton->setText("Edit indicator");
        else if(isMultiEditEnabled() && isCountMultiple)
            ui->editIndicatorsButton->setText("Edit indicators");
        else
            ui->editIndicatorsButton->setEnabled(false);
    }
    else
        ui->editIndicatorsButton->setEnabled(false);

    if(isRemoveEnabled()) {
        ui->removeIndicatorsButton->setEnabled(true);
        if(isCountSingle)
            ui->removeIndicatorsButton->setText("Remove indicator");
        else if(isCountMultiple)
            ui->removeIndicatorsButton->setText("Remove indicators");
        else
            ui->removeIndicatorsButton->setEnabled(false);
    }
    else
        ui->removeIndicatorsButton->setEnabled(isRemoveEnabled());

    for(size_t i = 0; i < ui->previewWidget->count(); ++i) {
        QListWidgetItem* item = ui->previewWidget->item(i);
        CustomFrame* frame    = qobject_cast<CustomFrame*>(ui->previewWidget->itemWidget(item));
        if(!frame)
            continue;

        frame->setHighlighted(item->isSelected());
    }
}
