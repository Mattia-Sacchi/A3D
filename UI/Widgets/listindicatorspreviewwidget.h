#ifndef LISTINDICATORSPREVIEWWIDGET_H
#define LISTINDICATORSPREVIEWWIDGET_H

#include "ui_listindicatorspreviewwidget.h"
#include "../../A3D/chart.h"
#include "../Dialogs/editenumdialog.h"

class ChartAxisSettings;

class ListIndicatorsPreviewWidget : public QWidget {
	Q_OBJECT

public:
    explicit ListIndicatorsPreviewWidget(QWidget* parent = nullptr);

    void addIndicators(std::vector<A3D::ChartAxisIndicator> const&);

    struct StyledIndicator {
        inline StyledIndicator(A3D::ChartAxisIndicator indicator)
            : m_type(indicator.m_type) {
            m_data.m_label = indicator.m_label;
            m_data.m_style = indicator.m_style;
        }

        A3D::ChartAxisIndicatorType m_type;
        A3D::ChartAxisStyledEnumeratedIndicatorData m_data;
    };

    std::vector<StyledIndicator> enumeratedIndicators() const;

    std::vector<A3D::ChartAxisIndicator> indicators() const;

private slots:

    void onAddButtonClicked();

    void onEditIndicartorsClicked();
    void onItemDoubleClicked(QModelIndex const&);
    void onEditAccepted();

    void onRemoveButtonClicked();

    void onItemSelectionChanged();

private:
    std::vector<A3D::ChartAxisIndicator> m_indicators;
    ChartAxisSettings* m_settings;
    EditEnumDialog m_dialog;
    Ui::ListIndicatorsPreviewWidget ui;
};

#endif // LISTINDICATORSPREVIEWWIDGET_H
