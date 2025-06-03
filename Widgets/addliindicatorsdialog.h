#ifndef ADDLIINDICATORSDIALOG_H
#define ADDLIINDICATORSDIALOG_H

#include <QDialog>

#include "../A3D/chart.h"
namespace Ui {
class AddLiIndicatorsDialog;
}

enum AddMode {
	AM_BY_STEP = 0,
	AM_RAW_ADD,
};

class AddLiIndicatorsDialog : public QDialog {
	Q_OBJECT

public:
	explicit AddLiIndicatorsDialog(QWidget* parent = nullptr);
	~AddLiIndicatorsDialog();

	std::vector<A3D::ChartAxisIndicator> indicators() const;
    void setStyle(A3D::ChartAxisIndicatorStyle style);

private:
	void setMode(AddMode);

    std::vector<A3D::ChartAxisIndicator> getIndicatorFromCount(size_t) const;

	void onRawAddButtonClicked();
	void onByStepButtonClicked();

	void onCountButtonClicked();
	void onStepSizeButtonClicked();

	void setCountOrStepSizeMode(bool);

    void onLabelDigitsChanged();

	AddMode m_mode;
	bool m_countMode;

	Ui::AddLiIndicatorsDialog* ui;
};

#endif // ADDLIINDICATORSDIALOG_H
