#include "chartwidget.h"
#include "calibrationwidget.h"

ChartWidget::ChartWidget(CalibrationWidget* parent)
	: QWidget(parent) {
	m_scene = new A3D::Scene(this);

	A3D::PointLightInfo& light = m_scene->getOrCreateLight(0);
	light.position             = QVector3D(0.5f, 10.f, 0.5f);
	light.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	m_view = new A3D::View(this);
	m_view->camera().setPosition(QVector3D(2.f, 2.f, 2.f));
	m_view->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	m_view->setScene(m_scene);

	A3D::SurfaceChartEntity* m_chart = m_scene->emplaceChildEntity<A3D::SurfaceChartEntity>();
	m_chart->setRenderVariants(A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS);
	m_chart->setPosition(QVector3D(0.f, 0.f, 0.f));
}

CalibrationWidget* ChartWidget::calWidget() const {
	return qobject_cast<CalibrationWidget*>(parent());
}

void ChartWidget::refresh() {}