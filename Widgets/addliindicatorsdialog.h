#ifndef ADDLIINDICATORSDIALOG_H
#define ADDLIINDICATORSDIALOG_H

#include <QDialog>

namespace Ui {
class AddLiIndicatorsDialog;
}

class AddLiIndicatorsDialog : public QDialog {
	Q_OBJECT

public:
	explicit AddLiIndicatorsDialog(QWidget* parent = nullptr);
	~AddLiIndicatorsDialog();

private:
	Ui::AddLiIndicatorsDialog* ui;
};

#endif // ADDLIINDICATORSDIALOG_H
