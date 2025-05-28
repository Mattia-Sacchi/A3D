#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"
#include "../A3D/chart.h"

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget* parent = nullptr);

    void onSettingsAccepted();

private:
	Ui::SettingsDialog ui;
};

#endif // SETTINGSDIALOG_H
