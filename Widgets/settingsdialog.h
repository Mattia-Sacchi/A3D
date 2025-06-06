#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"
#include "../A3D/chart.h"

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget* parent = nullptr);

    void setMap(A3D::MapChart3D&);
    void onSettingsAccepted();
signals:
    void mapUpdated(A3D::MapChart3D const&);

private:
	Ui::SettingsDialog ui;

    A3D::MapChart3D m_map;
};

#endif // SETTINGSDIALOG_H
