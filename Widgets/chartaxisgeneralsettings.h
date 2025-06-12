#ifndef CHARTAXISGENERALSETTINGS_H
#define CHARTAXISGENERALSETTINGS_H

#include "ui_chartaxisgeneralsettings.h"
#include "../A3D/chart.h"

enum FontResolutions { FR_Low = 0, FR_Medium, FR_High, FR_Count };

class ChartAxisGeneralSettings : public QWidget {
    Q_OBJECT

public:
    explicit ChartAxisGeneralSettings(QWidget* parent = nullptr);

    A3D::ChartAxisIndicatorStyle style() const;

    void setStyle(A3D::ChartAxisIndicatorStyle);

	static FontResolutions getFontResoulution(size_t fontPointSize);
	static size_t getDisplaySize(FontResolutions);

private slots:

    void onChangeLabelColorButtonClicked();

    void onChangeFontButtonClicked();

    void onHighResolutionRadioButtonClicked();

    void onMediumResolutionRadioButtonClicked();

    void onLowResolutionRadioButtonClicked();

    void onResetButtonClicked();

    void onScaleSliderValueChanged(int);

private:
    void setResoulution(FontResolutions res);
    void updateFontExampleResoulution();
    FontResolutions m_resolution;

    Ui::ChartAxisGeneralSettings ui;
};

#endif // CHARTAXISSETTINGS_H
