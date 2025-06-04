#ifndef VALUESPREVIEWWIDGET_H
#define VALUESPREVIEWWIDGET_H

#include "ui_valuespreviewwidget.h"
#include "../A3D/chart.h"

class ValuesPreviewWidget : public QWidget {
	Q_OBJECT

public:
    explicit ValuesPreviewWidget(QWidget* parent = nullptr);

    void addValues(std::vector<float>);

    std::vector<float> getValues() const;
    void setStringPrecision(size_t);
	void addValue(float);

    void clear();

    inline void setEditingMode(bool enabled) { ui.addValueButton->setEnabled(!enabled); }
private slots:

    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onItemSelectionChanged();

private:
	Ui::ValuesPreviewWidget ui;
};

#endif // VALUESPREVIEWWIDGET_H
