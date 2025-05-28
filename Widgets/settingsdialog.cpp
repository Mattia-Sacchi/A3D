#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent) {
    ui.setupUi(this);
    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onSettingsAccepted);
}

void SettingsDialog::onSettingsAccepted() {}
