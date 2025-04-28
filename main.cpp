#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>
#include "A3D/common.h"
#include "A3D/view.h"
#include "A3D/keycameracontroller.h"
#include "A3D/textbillboardentity.h"
#include "A3D/surfacechartentity.h"
#include "keyeventmanager.h"
#include "mathfunctions.h"
#include "A3D/maps.h"

int main(int argc, char* argv[]) {

	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s          = new A3D::Scene(&w);
	A3D::PointLightInfo& l = s->getOrCreateLight(0);
	l.position             = QVector3D(0.f, 2.2f, 5.f);
	l.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	A3D::SurfaceChartEntity* chart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();

	A3D::Map autoUpMap;
	bool ret = true;
	ret      = ret && autoUpMap.setAxis(A3D::D_X_Axis, { 0, 10, 15, 20, 30, 40, 50, 75, 100 });
	ret      = ret && autoUpMap.setFixedAxis(A3D::D_Z_Axis, { "1to2", "2to3", "3to4", "4to5", "5to6" });
	ret      = ret && autoUpMap.setLinearAxis(A3D::D_Y_Axis, 1800, 6400, 10);
	ret      = ret
	      && autoUpMap.setData(
			  {
				  1800, 2000, 2600, 3000, 3500, 4300, 4600, 5800, 6100, // 2
				  1800, 2300, 2900, 3300, 3500, 4300, 4600, 5800, 6100, // 3
				  1800, 2300, 2900, 3300, 3500, 4300, 4600, 5800, 6100, // 4
				  1800, 2300, 2900, 3300, 3900, 4300, 4600, 6000, 6100, // 5
				  1800, 2300, 2900, 3300, 4000, 4600, 4900, 6100, 6400, // 6
			  }
		  );

	if(!autoUpMap.isValid() || !ret) {
		qDebug() << "Error map is not valid";
		return 1;
	}
	chart->setTickLength(1);
	chart->setMap(s->resourceManager(), autoUpMap);
	chart->setPosition(QVector3D(0, 0, 0));

	A3D::View* v = new A3D::View(&w);
	v->camera().setPosition(QVector3D(2.f, 2.f, 2.f));
	v->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	v->setScene(s);

	A3D::KeyCameraController* keyCamController = new A3D::KeyCameraController(v);
	keyCamController->setBaseMovementSpeed(QVector3D(9.f, 9.f, 9.f));
	keyCamController->setKeyBinding(Qt::Key_Space, A3D::KeyCameraController::ACT_MOVE_UPWARD);
	keyCamController->setKeyBinding(Qt::Key_Shift, A3D::KeyCameraController::ACT_MOVE_DOWNWARD);
	keyCamController->setButtonBinding(Qt::LeftButton, A3D::KeyCameraController::ACT_LOOK_TOWARDS_MOUSE_POSITION);
	keyCamController->setButtonBinding(Qt::RightButton, A3D::KeyCameraController::ACT_SHOOT_RAY_MOUSE_POSITION);
	keyCamController->setKeyBinding(Qt::Key_J, A3D::KeyCameraController::ACT_SHOOT_RAY);
	v->setController(keyCamController);

	QTimer t;
	if(v->format().swapInterval() > 0)
		t.setInterval(0);
	else
		t.setInterval(10);
	t.start();

	QObject::connect(&t, &QTimer::timeout, v, &A3D::View::updateView);
	QObject::connect(&t, &QTimer::timeout, s, &A3D::Scene::updateScene);

	// Used just for testing
	KeyEventManager* kem = new KeyEventManager(v);
	kem->setMouseBinding(Qt::RightButton, [=]() {
		A3D::View* view               = v;
		QVector3D unprojectedMousePos = unprojectPointFrom2D(view, getCurrentNormalizedPos(view));
		unprojectedMousePos -= view->camera().position();
		unprojectedMousePos *= view->camera().farPlane();
		unprojectedMousePos += view->camera().position();
		std::optional<A3D::IntersectionResult> res = s->intersect(view->camera().position(), unprojectedMousePos);

		if(res) {
			qDebug() << "Intersection succeeded!";

			if(res->m_resultingEntity) {
				qDebug() << "Entity:" << res->m_resultingEntity;

				if(res->m_resultingEntity == chart) {
					chart->debug(res->m_groupLocalHitPoint);
					chart->drawIntersect(res->m_groupLocalHitPoint);
				}
			}

			if(res->m_resultingModel)
				qDebug() << "Model:" << res->m_resultingModel;

			if(res->m_resultingGroup)
				qDebug() << "Group:" << res->m_resultingGroup;

			qDebug() << "Hit:" << res->m_groupLocalHitPoint;
			qDebug() << "Global Coordinate:" << res->m_hitPoint;
		}
		else {
			qDebug() << "No intersection!";
		}
	});
	v->run();
	s->run();

	w.setCentralWidget(v);
	w.show();
	int rv = a.exec();
	return rv;
}

/*
	{
		A3D::SurfaceChartEntity* chart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();
		float yMax                     = 200;
		float yMin                     = 0;
		static const size_t size       = 151;
		std::vector<float> ax;
		for(int i = 0; i < size; i++)
			ax.push_back(i);

		std::vector<float> gaussMatrix     = generateGaussianKernel(size, 30.f);
		std::vector<float> edgeGuassMatrix = generateGaussianEdgeKernel(size, 5.f);
		for(int i = 0; i < gaussMatrix.size(); i++) {
			gaussMatrix[i] += edgeGuassMatrix[i] * 0.5;
		}

		A3D::Mesh* sampleMeshC = A3D::Mesh::generateSurfaceMesh(s->resourceManager(), ax, ax, gaussMatrix);
		chart->setTickLength(1);

		chart->loadSurface(sampleMeshC);
		chart->addLinearAxis(A3D::SurfaceChartEntity::X_Axis, 0, 6);
		chart->addLinearAxis(A3D::SurfaceChartEntity::Y_Axis, yMin, yMax);
		chart->addLinearAxis(A3D::SurfaceChartEntity::Z_Axis, 0, 6);

		chart->setPosition(QVector3D(0, 0, 1));
	}
*/