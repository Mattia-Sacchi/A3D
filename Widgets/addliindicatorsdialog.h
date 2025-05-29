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

private:
    std::vector<A3D::ChartAxisIndicator> getIndicators(size_t count) const;
	void setMode(AddMode);

	void onRawAddButtonClicked();
	void onByStepButtonClicked();

	void onCountButtonClicked();
	void onStepSizeButtonClicked();

	void setCountOrStepSizeMode(bool);

	AddMode m_mode;
	bool m_countMode;

	Ui::AddLiIndicatorsDialog* ui;
};

#endif // ADDLIINDICATORSDIALOG_H
