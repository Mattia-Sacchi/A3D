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

    void reset();

    std::vector<A3D::ChartAxisIndicator> indicators();
    void setStyle(A3D::ChartAxisIndicatorStyle style);
    void setChartIndicatorsType(A3D::ChartAxisIndicatorType type);

    void editIndicators(std::vector<A3D::ChartAxisIndicator> const& indicators);

private:
    void onLabelDigitsChanged();
    void setStyleNeeded(bool);
    bool m_styleNeeded;
    std::vector<A3D::ChartAxisIndicator> m_indicators;

    Ui::EditLiIndicatorsDialog* ui;
};

#endif // ADDLIINDICATORSDIALOG_H
