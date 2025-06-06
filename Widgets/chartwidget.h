#ifndef _CALIBRATIONWIDGET_HPP_
#define _CALIBRATIONWIDGET_HPP_

#include <QWidget>
#include <QTimer>
#include "../A3D/scene.h"
#include "../A3D/view.h"
#include "../A3D/surfacechartentity.h"
#include "../A3D/keyboardcameracontroller.h"
#include "../A3D/charteditorcontroller.h"

class ChartWidget : public QWidget {
	Q_OBJECT
public:
    ChartWidget(QWidget* parent = nullptr);

    void setMap(A3D::MapChart3D);

    inline A3D::SurfaceChartEntity* chart() const { return m_chart; }

private:
    A3D::Scene* m_scene;
    A3D::SurfaceChartEntity* m_chart;
    A3D::View* m_view;
    A3D::KeyboardCameraController* m_keyCamController;
    A3D::ChartEditorController* m_chartEditorController;
    A3D::MapChart3D m_map;

    QTimer m_timer;
};

#endif
