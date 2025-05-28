#include "addliindicatorsdialog.h"
#include "ui_addliindicatorsdialog.h"

AddLiIndicatorsDialog::AddLiIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::AddLiIndicatorsDialog) {
	ui->setupUi(this);
}

AddLiIndicatorsDialog::~AddLiIndicatorsDialog() {
	delete ui;
}
