#ifndef LINEARINDICATORSSETTINGSWIDGET_H
#define LINEARINDICATORSSETTINGSWIDGET_H

#include "ui_linearindicatorssettingswidget.h"

class LinearIndicatorsSettingsWidget : public QWidget {
	Q_OBJECT

public:
    explicit LinearIndicatorsSettingsWidget(QWidget* parent = nullptr);

private:
	Ui::LinearIndicatorsSettingsWidget ui;
};

#endif // LINEARINDICATORSSETTINGSWIDGET_H
