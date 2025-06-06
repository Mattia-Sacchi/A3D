#include "chartwidget.h"
#include "calibrationwidget.h"

ChartWidget::ChartWidget(QWidget* parent)
	: QWidget(parent) {
	m_scene = new A3D::Scene(this);

	A3D::PointLightInfo& light = m_scene->getOrCreateLight(0);
	light.position             = QVector3D(0.5f, 10.f, 0.5f);
	light.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	m_view = new A3D::View(this);
	m_view->camera().setPosition(QVector3D(2.f, 2.f, 2.f));
	m_view->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	m_view->setScene(m_scene);

    m_chart = m_scene->emplaceChildEntity<A3D::SurfaceChartEntity>();
	m_chart->setPosition(QVector3D(0.f, 0.f, 0.f));

    m_keyCamController = new A3D::KeyboardCameraController(m_view);
    m_keyCamController->setBaseMovementSpeed(QVector3D(9.f, 9.f, 9.f));
    m_keyCamController->addKeyBinding(Qt::Key_Space, A3D::KeyboardCameraController::ACT_MOVE_UPWARD);
    m_keyCamController->addKeyBinding(Qt::Key_Shift, A3D::KeyboardCameraController::ACT_MOVE_DOWNWARD);

    m_chartEditorController = new A3D::ChartEditorController(m_view);

    if(m_view->format().swapInterval() > 0)
        m_timer.setInterval(0);
    else
        m_timer.setInterval(10);
    m_timer.start();

    QObject::connect(&m_timer, &QTimer::timeout, m_view, &A3D::View::updateView);
    QObject::connect(&m_timer, &QTimer::timeout, m_scene, &A3D::Scene::updateScene);

    m_view->setAutoRefreshEnabled(true);
    m_scene->run();
}

void ChartWidget::setMap(A3D::MapChart3D map) {
    m_chart->setChart(map);
}
