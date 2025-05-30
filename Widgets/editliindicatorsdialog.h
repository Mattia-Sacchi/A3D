#ifndef EDITLIINDICATORSDIALOG_H
#define EDITLIINDICATORSDIALOG_H

#include <QDialog>
#include "../A3D/chart.h"

namespace Ui {
class EditLiIndicatorsDialog;
}

class EditLiIndicatorsDialog : public QDialog {
	Q_OBJECT

public:
    explicit EditLiIndicatorsDialog(QWidget* parent = nullptr);
    ~EditLiIndicatorsDialog();

	std::vector<A3D::ChartAxisIndicator> indicators() const;
    void setStyle(A3D::ChartAxisIndicatorStyle style);

    void editIndicators(std::vector<A3D::ChartAxisIndicator>);

private:
    void onLabelDigitsChanged();

    Ui::EditLiIndicatorsDialog* ui;
};

#endif // ADDLIINDICATORSDIALOG_H
