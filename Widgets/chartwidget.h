#ifndef _CALIBRATIONWIDGET_HPP_
#define _CALIBRATIONWIDGET_HPP_

#include <QWidget>
#include "../A3D/scene.h"
#include "../A3D/view.h"
#include "../A3D/surfacechartentity.h"

class CalibrationWidget;

class ChartWidget : public QWidget {
	Q_OBJECT
public:
	ChartWidget(CalibrationWidget* parent = nullptr);
	void refresh();

private:
	CalibrationWidget* calWidget() const;
	A3D::Scene* m_scene;
	A3D::SurfaceChartEntity* m_chart;
	A3D::View* m_view;
};

#endif
