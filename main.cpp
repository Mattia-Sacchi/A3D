#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>
#include "A3D/common.h"
#include "A3D/view.h"
#include "A3D/keyboardcameracontroller.h"
#include "A3D/surfacechartentity.h"
#include "A3D/chart.h"
#include "A3D/charteditorcontroller.h"

// #include "mathfunctions.h"
#include "keyeventmanager.h"

int main(int argc, char* argv[]) {

	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s          = new A3D::Scene(&w);
	A3D::PointLightInfo& l = s->getOrCreateLight(0);
	l.position             = QVector3D(0.f, 2.2f, 5.f);
	l.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	A3D::SurfaceChartEntity* autoUpChart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();
	autoUpChart->setRenderVariants(A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS);

	A3D::MapChart3D autoUpMap;

	{
		A3D::ChartAxisIndicatorStyle majorStyle;
		majorStyle.m_indicatorColor = Qt::blue;
		majorStyle.m_labelColor     = Qt::yellow;
		majorStyle.m_labelFont      = QFont("Helvetica [Cronyx]", 64);

		A3D::ChartAxisIndicatorStyle minorStyle = majorStyle;
		minorStyle.m_indicatorColor.setAlphaF(0.8f);

		A3D::ChartAxisData axis_GasPercent;
		axis_GasPercent.setDefaultIndicatorStyle(majorStyle);
		axis_GasPercent.setMinMax(0, 115);
		axis_GasPercent.addIndicators({ 0, 10, 15, 20, 30, 40, 50, 75, 100 }, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		axis_GasPercent.setDefaultIndicatorStyle(minorStyle);
		axis_GasPercent.addIndicators({ 60, 80, 90 }, 0, A3D::CHAXIND_MINOR_INDICATOR);
		axis_GasPercent.invert();
		axis_GasPercent.setName("Gas Percent");
		autoUpMap.setAxisData(A3D::AXIS_X, axis_GasPercent);

		A3D::ChartAxisData axis_Gear;
		{
			std::vector<A3D::ChartAxisStyledEnumeratedIndicatorData> gearShifts;
			gearShifts.resize(5);

			gearShifts[0].m_label                  = "1->2";
			gearShifts[0].m_style                  = majorStyle;
			gearShifts[0].m_style.m_indicatorColor = QColor(124, 179, 66);

			gearShifts[1].m_label                  = "2->3";
			gearShifts[1].m_style                  = majorStyle;
			gearShifts[1].m_style.m_indicatorColor = QColor(205, 220, 57);

			gearShifts[2].m_label                  = "3->4";
			gearShifts[2].m_style                  = majorStyle;
			gearShifts[2].m_style.m_indicatorColor = QColor(255, 235, 59);

			gearShifts[3].m_label                  = "4->5";
			gearShifts[3].m_style                  = majorStyle;
			gearShifts[3].m_style.m_indicatorColor = QColor(255, 152, 0);

			gearShifts[4].m_label                  = "5->6";
			gearShifts[4].m_style                  = majorStyle;
			gearShifts[4].m_style.m_indicatorColor = QColor(255, 87, 34);

			axis_Gear.setIndicators(std::move(gearShifts));
		}
		axis_Gear.invert();
		axis_Gear.setName("Gear");
		autoUpMap.setAxisData(A3D::AXIS_Z, axis_Gear);

		majorStyle.m_indicatorColor = Qt::darkGreen;
		minorStyle.m_indicatorColor = Qt::darkGreen;
		minorStyle.m_indicatorColor.setAlphaF(0.8f);

		A3D::ChartAxisData axis_RPMTarget;
		axis_RPMTarget.setDefaultIndicatorStyle(majorStyle);
		axis_RPMTarget.setMinMax(0, 7000);
		axis_RPMTarget.addEquidistantIndicatorsByStepSize(0.f, 7000.f, 1000.f, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		axis_RPMTarget.setDefaultIndicatorStyle(minorStyle);
		axis_RPMTarget.addEquidistantIndicatorsByStepSize(500.f, 6500.f, 1000.f, 0, A3D::CHAXIND_MINOR_INDICATOR);
		axis_RPMTarget.setName("RPM Target");
		autoUpMap.setAxisData(A3D::AXIS_Y, axis_RPMTarget);
	}

	autoUpMap.setChartPoints(
		{ 0, 10, 15, 20, 30, 40, 50, 75, 100 }, { 0, 1, 2, 3, 4 },
		{
			1800, 2000, 2600, 3000, 3500, 4300, 4600, 5800, 6100, // 2
			1800, 2300, 2900, 3300, 3500, 4300, 4600, 5800, 6100, // 3
			1800, 2300, 2900, 3300, 3500, 4300, 4600, 5800, 6100, // 4
			1800, 2300, 2900, 3300, 3900, 4300, 4600, 6000, 6100, // 5
			1800, 2300, 2900, 3300, 4000, 4600, 4900, 6100, 6400, // 6
		}
	);

	if(!autoUpMap.isValid()) {
		qDebug() << "AutoUp Map is not valid!";
		return 1;
	}

	autoUpChart->setChart(autoUpMap);
	autoUpChart->setLabelDistances(QVector3D(0.1f, 0.1f, 0.1f));

	A3D::SurfaceChartEntity* torqueRestitutionChart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();
	torqueRestitutionChart->setRenderVariants(A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS);

	A3D::MapChart3D torqueRestitutionTimeMap;

	{
		A3D::ChartAxisData axis_Torque;
		axis_Torque.setMinMax(-100, 500);
		axis_Torque.addIndicators({ -100, 0, 50, 100, 200, 300, 400, 500 }, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		//axis_Torque.addEquidistantIndicatorsByStepSize(159.f, 450.f, 50.f, 0, A3D::CHAXIND_MINOR_INDICATOR);
		//axis_Torque.invert();
		axis_Torque.setName("Torque");
		torqueRestitutionTimeMap.setAxisData(A3D::AXIS_X, axis_Torque);

		A3D::ChartAxisData axis_RPM;
		axis_RPM.setMinMax(0, 8000);
		axis_RPM.addEquidistantIndicatorsByStepSize(0, 8000, 1000, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		//axis_RPM.addEquidistantIndicatorsByStepSize(500,7500,500, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		//axis_RPM.invert();
		axis_RPM.setName("RPM");
		torqueRestitutionTimeMap.setAxisData(A3D::AXIS_Z, axis_RPM);

		A3D::ChartAxisData axis_Time;
		axis_Time.setMinMax(-100, 500);
		axis_Time.addEquidistantIndicatorsByStepSize(-100.f, 500.f, 50.f, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		axis_Time.setName("Restitution Time");
		torqueRestitutionTimeMap.setAxisData(A3D::AXIS_Y, axis_Time);
	}

	torqueRestitutionTimeMap.setChartPoints(
		{ -100, 0, 50, 100, 200, 300, 400, 500 }, { 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000 },
		{
			500, 500, 450, 400, 250, 100, 0,   -10,  // 1
			500, 500, 400, 400, 250, 100, 0,   -30,  // 2
			450, 400, 350, 350, 250, 100, 0,   -45,  // 3
			400, 400, 350, 250, 250, 100, 0,   -65,  // 4
			250, 250, 250, 250, 100, 0,   0,   -70,  // 5
			100, 100, 250, 100, 0,   0,   0,   -75,  // 6
			0,   50,  100, 0,   0,   0,   -75, -100, // 7
			-10, -30, -45, -65, -70, -75, -90, -100, // 8

		}
	);

	if(!torqueRestitutionTimeMap.isValid()) {
		qDebug() << "TorqueResitution Map is not valid!";
		return 1;
	}

	torqueRestitutionChart->setChart(torqueRestitutionTimeMap);
	torqueRestitutionChart->setLabelDistances(QVector3D(0.1f, 0.1f, 0.1f));
	torqueRestitutionChart->setPosition(QVector3D(2, 0, 0));

	A3D::SurfaceChartEntity* histoChart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();
	histoChart->setRenderVariants(A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS);
	histoChart->setPosition(QVector3D(0, 2, 0));

	A3D::MapChart3D histoMap;

	{
		A3D::ChartAxisIndicatorStyle majorStyle;
		majorStyle.m_indicatorColor = Qt::blue;
		majorStyle.m_labelColor     = Qt::yellow;
		majorStyle.m_labelFont      = QFont("Helvetica [Cronyx]", 64);

		A3D::ChartAxisIndicatorStyle minorStyle = majorStyle;
		minorStyle.m_indicatorColor.setAlphaF(0.8f);

		A3D::ChartAxisData axis_Iso;
		axis_Iso.setDefaultIndicatorStyle(majorStyle);
		{
			std::vector<A3D::ChartAxisStyledEnumeratedIndicatorData> isos;
			isos.resize(2);

			isos[0].m_label                  = "Current";
			isos[0].m_style                  = majorStyle;
			isos[0].m_style.m_indicatorColor = QColor(124, 179, 66);

			isos[1].m_label                  = "Reference";
			isos[1].m_style                  = majorStyle;
			isos[1].m_style.m_indicatorColor = QColor(205, 220, 57);
			axis_Iso.setIndicators(std::move(isos));
		}
		axis_Iso.setName("ISO");
		histoMap.setAxisData(A3D::AXIS_Z, axis_Iso);

		A3D::ChartAxisData axis_Gear;
		{
			std::vector<A3D::ChartAxisStyledEnumeratedIndicatorData> gears;
			gears.resize(5);

			gears[0].m_label                  = "Up";
			gears[0].m_style                  = majorStyle;
			gears[0].m_style.m_indicatorColor = QColor(124, 179, 66);

			gears[1].m_label                  = "Half Up";
			gears[1].m_style                  = majorStyle;
			gears[1].m_style.m_indicatorColor = QColor(205, 220, 57);

			gears[2].m_label                  = "Center";
			gears[2].m_style                  = majorStyle;
			gears[2].m_style.m_indicatorColor = QColor(255, 235, 59);

			gears[3].m_label                  = "Half Down";
			gears[3].m_style                  = majorStyle;
			gears[3].m_style.m_indicatorColor = QColor(255, 152, 0);

			gears[4].m_label                  = "Down";
			gears[4].m_style                  = majorStyle;
			gears[4].m_style.m_indicatorColor = QColor(255, 87, 34);

			axis_Gear.setIndicators(std::move(gears));
		}
		axis_Gear.setName("Gear");
		histoMap.setAxisData(A3D::AXIS_X, axis_Gear);

		majorStyle.m_indicatorColor = Qt::darkGreen;
		minorStyle.m_indicatorColor = Qt::darkGreen;
		minorStyle.m_indicatorColor.setAlphaF(0.8f);

		A3D::ChartAxisData axis_TargetPositions;
		axis_TargetPositions.setDefaultIndicatorStyle(majorStyle);
		axis_TargetPositions.setMinMax(-50.f, 50.f);
		axis_TargetPositions.addEquidistantIndicatorsByStepSize(-50.f, 50.f, 10.f, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		axis_TargetPositions.setDefaultIndicatorStyle(minorStyle);

		axis_TargetPositions.setName("Target positions");
		histoMap.setAxisData(A3D::AXIS_Y, axis_TargetPositions);
	}

	histoMap.setChartPoints({ 0, 1, 2, 3, 4 }, { 0, 1 }, { -32, -10, 0, 10, 32, 21, 15, 0, -15, -21 });

	if(!histoMap.isValid()) {
		qDebug() << "AutoUp Map is not valid!";
		return 1;
	}

	histoChart->setChart(histoMap);
	histoChart->setLabelDistances(QVector3D(0.1f, 0.1f, 0.1f));

	A3D::View* v = new A3D::View(&w);
	v->camera().setPosition(QVector3D(2.f, 2.f, 2.f));
	v->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	v->setScene(s);

	A3D::KeyboardCameraController* keyCamController = new A3D::KeyboardCameraController(v);
	keyCamController->setBaseMovementSpeed(QVector3D(9.f, 9.f, 9.f));
	keyCamController->addKeyBinding(Qt::Key_Space, A3D::KeyboardCameraController::ACT_MOVE_UPWARD);
	keyCamController->addKeyBinding(Qt::Key_Shift, A3D::KeyboardCameraController::ACT_MOVE_DOWNWARD);

	new A3D::ChartEditorController(v);

	QTimer t;
	if(v->format().swapInterval() > 0)
		t.setInterval(0);
	else
		t.setInterval(10);
	t.start();

	QObject::connect(&t, &QTimer::timeout, v, &A3D::View::updateView);
	QObject::connect(&t, &QTimer::timeout, s, &A3D::Scene::updateScene);

#if 0
	QVector3D* pressLocalPos = new QVector3D;
	// Used just for testing

	KeyEventManager* kem = new KeyEventManager(v);
	kem->setBinding(Qt::RightButton, [=](QEvent::Type type) {
		A3D::View* view               = v;
		QPoint cursorPosition         = view->mapFromGlobal(QCursor::pos());
		QPointF normalizedPosition    = view->toNormalizedPoint(cursorPosition.toPointF());
		QVector3D unprojectedMousePos = view->camera().unprojectPoint(normalizedPosition);
		// unprojectedMousePos -= view->camera().position();
		// unprojectedMousePos *= view->camera().farPlane();
		// unprojectedMousePos += view->camera().position();

		if(type == QEvent::MouseMove) {
			if(pressLocalPos->isNull())
				return;
			std::optional<A3D::IntersectionResult> res = s->intersect(view->camera().position(), unprojectedMousePos);

			if(res) {
				QVector3D newHitPoint = res.value().m_groupLocalHitPoint;
				qDebug() << *pressLocalPos << newHitPoint << " dy: " << pressLocalPos->y() - newHitPoint.y();
			}

			QVector3D localCameraDirection = autoUpChart->model()->modelMatrix().map(unprojectedMousePos);
			localCameraDirection           = autoUpChart->model()->getGroup("Marker")->groupMatrix().map(localCameraDirection);

			QVector3D localCameraPosition = autoUpChart->model()->modelMatrix().map(v->camera().position());
			localCameraPosition           = autoUpChart->model()->getGroup("Marker")->groupMatrix().map(localCameraPosition);

			QVector2D const markerXY = autoUpChart->marker();
			QVector3D const markerA  = QVector3D(markerXY.x(), 0.f, markerXY.y());
			QVector3D const markerB  = QVector3D(markerXY.x(), 1.f, markerXY.y());

			// punto e direzione retta 1
			QVector3D p1 = markerA;
			QVector3D d1 = markerB - markerA;

			// punto e direzione retta 2
			QVector3D p2 = localCameraPosition;
			QVector3D d2 = localCameraPosition - localCameraDirection;

			QVector3D w0 = p1 - p2;
			float a      = QVector3D::dotProduct(d1, d1);
			float b      = QVector3D::dotProduct(d1, d2);
			float c      = QVector3D::dotProduct(d2, d2);
			float d      = QVector3D::dotProduct(d1, w0);
			float e      = QVector3D::dotProduct(d2, w0);

			float denom = a * c - b * b;

			float t = (b * e - c * d) / denom;
			float s = (a * e - b * d) / denom;
			t       = std::clamp(t, 0.f, 1.f);

			QVector3D closestPointOnLine1 = p1 + d1 * t;
			QVector3D closestPointOnLine2 = p2 + d2 * s;
			A3D::LineGroup::Vertex pointA;
			A3D::LineGroup::Vertex pointB;
			pointA.Color4D = QVector4D(0, 1, 0, 1);
			pointB.Color4D = QVector4D(0, 1, 0, 1);

			pointA.Position3D = closestPointOnLine1;
			pointB.Position3D = closestPointOnLine2;

			A3D::Group* altMarkerGroup = autoUpChart->model()->getOrAddGroup("MarkerTest");
			if(!altMarkerGroup->lineGroup()) {
				altMarkerGroup->setLineGroup(new A3D::LineGroup);
				altMarkerGroup->lineGroup()->setThickness(0.01);
				altMarkerGroup->lineGroup()->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
			}

			altMarkerGroup->lineGroup()->vertices().clear();
			altMarkerGroup->lineGroup()->vertices().push_back(pointA);
			altMarkerGroup->lineGroup()->vertices().push_back(pointB);
			altMarkerGroup->lineGroup()->invalidateCache();

			// float distance                = (closestPointOnLine1 - closestPointOnLine2).length();
		}
	});
#endif

	v->setAutoRefreshEnabled(true);
	s->run();

	w.setCentralWidget(v);
	w.show();
	int rv = a.exec();
	return rv;
}
