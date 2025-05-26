#include "charteditorcontroller.h"
#include "view.h"
#include "scene.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

namespace A3D {

ChartEditorController::ChartEditorController(View* view)
	: ViewController{ view },
	  m_editInitialDragValue(0.f),
	  m_editFilterMask(0xFFFFFFFF),
	  m_currentDragMode(DM_NONE) {
	m_currentSurfaceChart = m_surfaceChartsDetected.end();
}

bool ChartEditorController::update(std::chrono::milliseconds) {
	return true;
}

void ChartEditorController::setEditFilterMask(std::uint32_t mask) {
	m_editFilterMask = mask;
}

std::uint32_t ChartEditorController::editFilterMask() const {
	return m_editFilterMask;
}

bool ChartEditorController::eventFilter(QObject* watched, QEvent* event) {
	if(!view() || !view()->scene())
		return eventFilter(watched, event);

	switch(event->type()) {
	case QEvent::MouseButtonRelease:
		{
			QMouseEvent* me = static_cast<QMouseEvent*>(event);
			if(m_currentDragMode == DM_SELECTION && me->button() == Qt::LeftButton) {
				updateSelectionFromMouse(me->position(), me->modifiers());
				m_currentDragMode = DM_NONE;
				break;
			}
			else if(m_currentDragMode == DM_OPERATION && me->button() == Qt::RightButton) {
				updateOperationFromMouse(me->position(), me->modifiers());
				m_currentDragMode = DM_NONE;
				break;
			}
		}
		break;
	case QEvent::MouseMove:
		{
			QMouseEvent* me = static_cast<QMouseEvent*>(event);
			switch(m_currentDragMode) {
			case DM_SELECTION:
				if(m_currentSurfaceChart == m_surfaceChartsDetected.end()) {
					m_currentDragMode = DM_NONE;
					break;
				}

				updateSelectionFromMouse(me->pos(), me->modifiers());
				break;
			case DM_OPERATION:
				if(m_currentSurfaceChart == m_surfaceChartsDetected.end()) {
					m_currentDragMode = DM_NONE;
					break;
				}

				updateOperationFromMouse(me->position(), me->modifiers());
			default:
				break;
			}
		}
		break;
	case QEvent::MouseButtonPress:
		{
			QMouseEvent* me = static_cast<QMouseEvent*>(event);
			if(m_currentDragMode != DM_NONE)
				break;

			switch(me->button()) {
			case Qt::LeftButton:
				if(updateSelectionFromMouse(me->position(), me->modifiers()))
					m_currentDragMode = DM_SELECTION;
				break;
			case Qt::RightButton:
				if(updateOperationFromMouse(me->position(), me->modifiers()))
					m_currentDragMode = DM_OPERATION;
				break;
			default:
				break;
			}
		}
		break;

	default:
		break;
	}
	return QObject::eventFilter(watched, event);
}

bool ChartEditorController::updateOperationFromMouse(QPointF mouseViewPos, Qt::KeyboardModifiers modifiers) {
	if(m_currentSurfaceChart == m_surfaceChartsDetected.end() || !m_currentSurfaceChart->m_selectionResult || m_currentSurfaceChart->m_selectedPoints.empty())
		return false;

	if(!m_currentSurfaceChart->m_surface) {
		m_surfaceChartsDetected.erase(m_currentSurfaceChart);
		m_currentSurfaceChart = m_surfaceChartsDetected.end();
		return false;
	}

	QPointF const cursorPosition = mouseViewPos;

	QPointF const normalizedPosition = view()->toNormalizedPoint(cursorPosition);
	QVector3D unprojMousePos         = view()->camera().unprojectPoint(normalizedPosition);

	QMatrix4x4 targetEntityMatrix = view()
	                                    ->calculateFullMatrix(
											m_currentSurfaceChart->m_selectionResult->m_resultingEntity, // Entity
											m_currentSurfaceChart->m_selectionResult->m_resultingModel,  // Model
											m_currentSurfaceChart->m_selectionResult->m_resultingGroup   // Group
										)
	                                    .inverted();

	QVector3D const cameraPosition = targetEntityMatrix.map(view()->camera().position());
	QVector3D const cameraTarget   = targetEntityMatrix.map(unprojMousePos);

	QVector2D const markerXY    = m_currentSurfaceChart->m_surface->marker();
	QVector3D const markerStart = QVector3D(markerXY.x(), 0.f, markerXY.y());
	QVector3D const markerEnd   = QVector3D(markerXY.x(), 1.f, markerXY.y());

	std::pair<QVector3D, QVector3D> result = getClosestLinePoints(cameraPosition, cameraTarget, markerStart, markerEnd, false, false);

	A3D::LineGroup::Vertex pointA;
	A3D::LineGroup::Vertex pointB;
	pointA.Color4D = QVector4D(0, 1, 0, 1);
	pointB.Color4D = QVector4D(1, 0, 0, 1);

	pointA.Position3D = result.first;
	pointB.Position3D = result.second;

	MapChart3D& map  = m_currentSurfaceChart->m_surface->mapChart();
	float const newY = map.axisData(AXIS_Y).denormalizeValue(result.second.y());

	// Skip the first offset operation...
	if(m_currentDragMode == DM_OPERATION) {
		float deltaIncrement = newY - m_editInitialDragValue;
		deltaIncrement /= m_currentSurfaceChart->m_selectedPoints.front().m_weight;
		map.offsetY(m_currentSurfaceChart->m_selectedPoints, deltaIncrement, A3D::CT_CLAMP_OFFSET);
	}

	qDebug() << result.second.y() << "(Delta: " << (newY - m_editInitialDragValue) << " -> Exp. Result: " << newY << ")";

	m_editInitialDragValue = newY;

	A3D::Group* altMarkerGroup = m_currentSurfaceChart->m_surface->model()->getOrAddGroup("MarkerTest");
	if(!altMarkerGroup->lineGroup()) {
		altMarkerGroup->setLineGroup(new A3D::LineGroup);
		altMarkerGroup->lineGroup()->setThickness(0.01);
		altMarkerGroup->lineGroup()->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
	}

	altMarkerGroup->lineGroup()->vertices().clear();
	altMarkerGroup->lineGroup()->vertices().push_back(pointA);
	altMarkerGroup->lineGroup()->vertices().push_back(pointB);
	altMarkerGroup->lineGroup()->invalidateCache();

	return true;
}

bool ChartEditorController::updateSelectionFromMouse(QPointF mouseViewPos, Qt::KeyboardModifiers modifiers) {
	// Selection
	QPointF const cursorPosition     = mouseViewPos;
	QPointF const normalizedPosition = view()->toNormalizedPoint(cursorPosition);
	QVector3D unprojMousePos         = view()->camera().unprojectPoint(normalizedPosition);

	unprojMousePos -= view()->camera().position();
	unprojMousePos *= view()->camera().farPlane();
	unprojMousePos += view()->camera().position();

	std::function<bool(Entity const*)> filter;

	if(m_currentDragMode != DM_NONE) {
		if(m_currentSurfaceChart != m_surfaceChartsDetected.end()) {
			Entity const* selEntity = m_currentSurfaceChart->m_surface;
			if(!selEntity) {
				m_surfaceChartsDetected.erase(m_currentSurfaceChart);
				m_currentSurfaceChart = m_surfaceChartsDetected.end();
			}
			else {
				filter = [selEntity](Entity const* e) -> bool {
					return (e == selEntity);
				};
			}
		}
	}

	//qDebug() << "Ray from " << view()->camera().position() << " with direction " << unprojMousePos;
	std::optional<IntersectionResult> selectionResult = view()->scene()->intersect(view()->camera().position(), unprojMousePos, filter);
	//std::optional<IntersectionResult> selectionResult = view()->scene()->intersect(QVector3D(2, 2, 2), QVector3D(-579.159f, -672.229f, -474.662f).normalized(), filter);
	if(!selectionResult)
		return false;

	qDebug() << "Intersect: " << selectionResult->m_groupLocalHitPoint << " -> " << selectionResult->m_hitPoint;

	SurfaceChartEntity* chartEntity = qobject_cast<SurfaceChartEntity*>(selectionResult->m_resultingEntity.get());
	if(!chartEntity)
		return false;

	SurfaceChartEditInfo* scei = nullptr;
	for(auto it = m_surfaceChartsDetected.begin(); it != m_surfaceChartsDetected.end();) {
		if(!it->m_surface)
			m_surfaceChartsDetected.erase(it);
		else if(it->m_surface == chartEntity) {
			scei                  = &*it;
			m_currentSurfaceChart = it;
			break;
		}
		else
			++it;
	}

	if(!scei) {
		scei                  = &m_surfaceChartsDetected.emplace_front();
		m_currentSurfaceChart = m_surfaceChartsDetected.begin();

		scei->m_surface     = chartEntity;
		MapChart3D const& c = chartEntity->mapChart();

		// Default radius = 1/5th of the chart's range.
		scei->m_selectionRadius = QVector2D(1.f, 1.f);
		if(c.axisData(AXIS_X).type() == CHAXIS_LINEAR_INTERPOLATED)
			scei->m_selectionRadius.setX(std::abs(c.axisData(AXIS_X).maximum() - c.axisData(AXIS_X).minimum()) / 5.f);
		if(c.axisData(AXIS_Z).type() == CHAXIS_LINEAR_INTERPOLATED)
			scei->m_selectionRadius.setY(std::abs(c.axisData(AXIS_Z).maximum() - c.axisData(AXIS_Z).minimum()) / 5.f);
	}

	scei->m_selectionResult = std::move(selectionResult);

	QVector2D axisCoordIntersection
		= scei->m_surface->mapChart().meshCoordinateToAxisCoordinate(QVector2D(scei->m_selectionResult->m_groupLocalHitPoint.x(), scei->m_selectionResult->m_groupLocalHitPoint.z())
	    );

	Group* g = scei->m_surface->model()->getOrAddGroup("SelectionRadius");
	g->setRenderOptions(Group::Hidden);

	if(!g->materialProperties()) {
		g->setMesh(Mesh::standardMesh(Mesh::CubeIndexedMesh));
		g->setMaterial(Material::standardMaterial(Material::WireframeMaterial));
		g->setMaterialProperties(new MaterialProperties());
	}

	if(modifiers & Qt::ControlModifier) {
		scei->m_selectedPoints = scei->m_surface->mapChart().searchNearestPointsToAxisCoordinate(axisCoordIntersection);
	}
	else {
		scei->m_selectedPoints = scei->m_surface->mapChart().searchNearestPointsToAxisCoordinate(axisCoordIntersection, scei->m_selectionRadius);
	}

	// Should never happen.
	if(scei->m_selectedPoints.empty())
		return false;

	// CTRL is pressed: Only focus on the nearest data point.
	if(modifiers & Qt::ControlModifier) {
		scei->m_selectedPoints.resize(1);
		scei->m_selectedPoints[0].m_weight = 1.f;
	}

	if(scei->m_selectedPoints.size() == 1) {
		QVector2D markerRealPosition = scei->m_surface->mapChart().axisCoordinateToMeshCoordinate(scei->m_selectedPoints[0].m_coordinate);
		scei->m_surface->setMarker(markerRealPosition);
	}
	else {
		scei->m_surface->setMarker(QVector2D(scei->m_selectionResult->m_groupLocalHitPoint.x(), scei->m_selectionResult->m_groupLocalHitPoint.z()));

		QVector2D axis2DCoordinates = scei->m_surface->mapChart().meshCoordinateToAxisCoordinate(scei->m_surface->marker());
		QVector3D axisHitPosition   = scei->m_surface->mapChart().getValueFromAxisCoordinate(axis2DCoordinates);
		QVector3D meshHitPosition   = scei->m_surface->mapChart().axisCoordinateToMeshCoordinate(axisHitPosition);

		g->setPosition(meshHitPosition);

		float scaleMin = scei->m_surface->mapChart().axisData(AXIS_Y).minimum();
		float scaleMax = scei->m_surface->mapChart().axisData(AXIS_Y).maximum();

		QVector3D realScaleObject = QVector3D(scei->m_selectionRadius.x() - 0.5f, (scaleMax - scaleMin) / 5.f, scei->m_selectionRadius.y() - 0.5f);
		QVector3D meshScaleObject = scei->m_surface->mapChart().axisDeltaCoordinateToMeshDeltaCoordinate(realScaleObject);
		g->setScale(meshScaleObject);
		g->setRenderOptions(Group::NoOptions);
	}

	view()->update();
	return true;
}

}
