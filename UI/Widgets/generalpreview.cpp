#include "generalpreview.h"
#include "ui_generalpreview.h"
#include "customframe.h"

GeneralPreview::GeneralPreview(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::GeneralPreview) {
	ui->setupUi(this);

    ui->previewWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(ui->previewWidget, &QListWidget::itemSelectionChanged, this, &GeneralPreview::onItemSelectionChanged);
    ui->addIndicatorsButton->setEnabled(isAddEnabled());
    ui->editIndicatorsButton->setEnabled(isEditEnabled());
}

GeneralPreview::~GeneralPreview() {
	delete ui;
}

bool GeneralPreview::isAddEnabled() {
    return true;
}

bool GeneralPreview::isEditEnabled() {
    return true;
}

bool GeneralPreview::isMultiEditEnabled() {
    return true;
}

bool GeneralPreview::isRemoveEnabled() {
    return true;
}

void GeneralPreview::onItemSelectionChanged() {
    size_t count         = ui->previewWidget->selectedItems().count();
    bool isCountSingle   = count == 1;
    bool isCountMultiple = count > 1;

    if(isEditEnabled()) {
        ui->editIndicatorsButton->setEnabled(true);
        if(isCountSingle)
            ui->editIndicatorsButton->setText("Edit indicator");
        else if(isMultiEditEnabled() && isCountMultiple)
            ui->editIndicatorsButton->setText("Edit indicators");
        else
            ui->editIndicatorsButton->setEnabled(false);
    }

    if(isRemoveEnabled()) {
        ui->removeIndicatorsButton->setEnabled(true);
        if(isCountSingle)
            ui->removeIndicatorsButton->setText("Edit indicator");
        else if(isMultiEditEnabled() && isCountMultiple)
            ui->removeIndicatorsButton->setText("Edit indicators");
        else
            ui->removeIndicatorsButton->setEnabled(false);
    }

    for(size_t i = 0; i < ui->previewWidget->count(); ++i) {
        QListWidgetItem* item = ui->previewWidget->item(i);
        CustomFrame* frame    = qobject_cast<CustomFrame*>(ui->previewWidget->itemWidget(item));
        if(!frame)
            continue;

        frame->setHighlighted(item->isSelected());
    }
}
