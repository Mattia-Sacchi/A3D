#include "charteditorcontroller.h"
#include "view.h"
#include "scene.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

namespace A3D {

ChartEditorController::ChartEditorController(View* view)
	: ViewController{ view },
	  m_editFilterMask(0xFFFFFFFF),
	  m_currentDragMode(DM_NONE) {}

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
				updateSelectionFromMouse(me->position(), me->modifiers(), m_selectedSurface);
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
				if(!m_selectedSurface) {
					m_currentDragMode = DM_NONE;
					break;
				}

				updateSelectionFromMouse(me->pos(), me->modifiers(), m_selectedSurface);
				break;
			case DM_OPERATION:
				if(!m_selectedSurface) {
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
				if(updateSelectionFromMouse(me->position(), me->modifiers(), nullptr))
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
	if(!m_selectedSurface || m_selectedPoints.empty() || !m_selectionResult)
		return false;

	QPointF const cursorPosition = mouseViewPos;

	QPointF const normalizedPosition = view()->toNormalizedPoint(cursorPosition);
	QVector3D unprojMousePos         = view()->camera().unprojectPoint(normalizedPosition);

	QMatrix4x4 targetEntityMatrix
		= view()->calculateFullMatrix(m_selectionResult->m_resultingEntity, m_selectionResult->m_resultingModel, m_selectionResult->m_resultingGroup).inverted();

	QVector3D const cameraPosition = targetEntityMatrix.map(view()->camera().position());
	QVector3D const cameraTarget   = targetEntityMatrix.map(unprojMousePos);

	QVector2D const markerXY    = m_selectedSurface->marker();
	QVector3D const markerStart = QVector3D(markerXY.x(), 0.f, markerXY.y());
	QVector3D const markerEnd   = QVector3D(markerXY.x(), 1.f, markerXY.y());

	std::pair<QVector3D, QVector3D> result = getClosestLinePoints(cameraPosition, cameraTarget, markerStart, markerEnd, false, true);

	A3D::LineGroup::Vertex pointA;
	A3D::LineGroup::Vertex pointB;
	pointA.Color4D = QVector4D(0, 1, 0, 1);
	pointB.Color4D = QVector4D(1, 0, 0, 1);

	pointA.Position3D = result.first;
	pointB.Position3D = result.second;

	MapChart3D& map  = m_selectedSurface->mapChart();
	float const newY = map.axisData(AXIS_Y).denormalizeValue(result.second.y());

	// Skip the first offset operation...
	if(m_currentDragMode == DM_OPERATION) {
		float deltaIncrement = newY - m_editInitialDragValue;
		deltaIncrement /= m_selectedPoints.front().m_weight;
		map.offsetY(m_selectedPoints, deltaIncrement, true);
	}

	qDebug() << result.second.y() << "(Delta: " << (newY - m_editInitialDragValue) << " -> Exp. Result: " << newY << ")";

	m_editInitialDragValue = newY;

	A3D::Group* altMarkerGroup = m_selectedSurface->model()->getOrAddGroup("MarkerTest");
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

bool ChartEditorController::updateSelectionFromMouse(QPointF mouseViewPos, Qt::KeyboardModifiers modifiers, Entity* filterEntity) {
	// Selection
	QPointF const cursorPosition     = mouseViewPos;
	QPointF const normalizedPosition = view()->toNormalizedPoint(cursorPosition);
	QVector3D unprojMousePos         = view()->camera().unprojectPoint(normalizedPosition);

	unprojMousePos -= view()->camera().position();
	unprojMousePos *= view()->camera().farPlane();
	unprojMousePos += view()->camera().position();

	m_selectionResult = view()->scene()->intersect(view()->camera().position(), unprojMousePos, filterEntity);

	if(!m_selectionResult)
		return false;

	SurfaceChartEntity* chartEntity = qobject_cast<SurfaceChartEntity*>(m_selectionResult->m_resultingEntity.get());
	if(!chartEntity)
		return false;

	m_selectedSurface = chartEntity;

	QVector2D axisCoordIntersection
		= m_selectedSurface->mapChart().meshCoordinateToAxisCoordinate(QVector2D(m_selectionResult->m_groupLocalHitPoint.x(), m_selectionResult->m_groupLocalHitPoint.z()));
	m_selectedPoints = m_selectedSurface->mapChart().searchNearestPointsToAxisCoordinate(axisCoordIntersection, QVector2D(400, 2500));

	// Should never happen.
	if(m_selectedPoints.empty())
		return false;

	// CTRL is pressed: Only focus on the nearest data point.
	if(modifiers & Qt::ControlModifier) {
		m_selectedPoints.resize(1);
		m_selectedPoints[0].m_weight = 1.f;
	}

	QVector3D axisResultingValue = m_selectedSurface->mapChart().getValueFromSearchResult(m_selectedPoints);
	QVector3D meshResultingValue = m_selectedSurface->mapChart().axisCoordinateToMeshCoordinate(axisResultingValue);

	m_selectedSurface->setMarker(QVector2D(meshResultingValue.x(), meshResultingValue.z()));
	qDebug() << axisResultingValue.x() << axisResultingValue.y() << axisResultingValue.z();

	view()->update();
	return true;
}

}
