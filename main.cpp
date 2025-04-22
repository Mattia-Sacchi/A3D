#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>

#include "A3D/view.h"
#include "A3D/keycameracontroller.h"
#include "A3D/textbillboardentity.h"
#include "A3D/surfacechartentity.h"
#include "keyeventmanager.h"

int main(int argc, char* argv[]) {

	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s          = new A3D::Scene(&w);
	A3D::PointLightInfo& l = s->getOrCreateLight(0);
	l.position             = QVector3D(0.f, 2.2f, 5.f);
	l.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	/*A3D::MaterialProperties* Concrete002  = nullptr;
	A3D::MaterialProperties* Metal035     = nullptr;
	A3D::MaterialProperties* FloorTiles06 = nullptr;
	A3D::Cubemap* Cubemap001              = nullptr;

	{
		auto loadPBRMaterial = [s](QString path, QString baseName, QString fileExtension) -> A3D::MaterialProperties* {
			A3D::MaterialProperties* matProperties = new A3D::MaterialProperties(s->resourceManager());
			return matProperties;
			static std::map<A3D::MaterialProperties::TextureSlot, QString> suffixes = {
				{    A3D::MaterialProperties::AlbedoTextureSlot,     "Color" },
				{    A3D::MaterialProperties::NormalTextureSlot,  "NormalGL" },
				{  A3D::MaterialProperties::MetallicTextureSlot,  "Metallic" },
				{ A3D::MaterialProperties::RoughnessTextureSlot, "Roughness" },
				{        A3D::MaterialProperties::AOTextureSlot,        "AO" },
			};

			for(auto it = suffixes.begin(); it != suffixes.end(); ++it) {
				std::pair<A3D::MaterialProperties::TextureSlot, QString> const& suffix = *it;

				A3D::Image img(QImage(path + QDir::separator() + baseName + "_" + suffix.second + "." + fileExtension));
				if(!img.isNull()) {
					A3D::Texture* texture = new A3D::Texture(std::move(img), s->resourceManager());
					s->resourceManager()->registerTexture(baseName + "_" + suffix.second, texture);

					matProperties->setTexture(texture, suffix.first);
				}
				else if(suffix.second == "AO") {
					A3D::Texture* texture = A3D::Texture::standardTexture(A3D::Texture::WhiteTexture);
					matProperties->setTexture(texture, suffix.first);
				}
			}
			return matProperties;
		};

		auto loadCubemap = [s](QString path, QString fileExtension) -> A3D::Cubemap* {
			A3D::Cubemap* cubemap = new A3D::Cubemap(s->resourceManager());

			cubemap->setNX(A3D::Image::HDR(path + "/nx." + fileExtension));
			cubemap->setNY(A3D::Image::HDR(path + "/ny." + fileExtension));
			cubemap->setNZ(A3D::Image::HDR(path + "/nz." + fileExtension));
			cubemap->setPX(A3D::Image::HDR(path + "/px." + fileExtension));
			cubemap->setPY(A3D::Image::HDR(path + "/py." + fileExtension));
			cubemap->setPZ(A3D::Image::HDR(path + "/pz." + fileExtension));

			if(!cubemap->isValid()) {
				delete cubemap;
				return nullptr;
			}
			return cubemap;
		};

		Cubemap001 = loadCubemap(":/A3D/SampleResources/Materials/Cubemap001", "hdr");
		if(Cubemap001) {
			Cubemap001->setParent(s->resourceManager());
			s->setSkybox(Cubemap001);
		}

		Concrete002 = loadPBRMaterial(":/A3D/SampleResources/Materials/Concrete002", "Concrete002_4K", "jpg");
		Concrete002->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...

		Metal035 = loadPBRMaterial(":/A3D/SampleResources/Materials/Metal035", "Metal035_2K-JPG", "jpg");
		Metal035->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...

		FloorTiles06 = loadPBRMaterial(":/A3D/SampleResources/Materials/FloorTiles06", "floor_tiles_06", "png");
		FloorTiles06->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...
	}*/

	{
		A3D::SurfaceChartEntity* chart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();

		std::vector<float> xAxisData = { 0, 10, 15, 20, 30, 40, 50, 75, 100 }; // Gas
		std::vector<float> zAxisData = { 2, 3, 4, 5, 6, 7 };                   // Marcia Target
		float yMax                   = 6100;
		float yMin                   = 1800;

		// Giri
		A3D::Mesh* sampleMeshC = A3D::Mesh::generateSurfaceMesh(
			s->resourceManager(), xAxisData, zAxisData,

			{ 1800, 2000, 2600, 3000, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100,
		      1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100 }
		);
		chart->setTickLength(1);

		chart->loadSurface(sampleMeshC);
		chart->addNormalizedAxis(A3D::SurfaceChartEntity::X_Axis, xAxisData);
		chart->addLinearAxis(A3D::SurfaceChartEntity::Y_Axis, yMin, yMax);
		chart->addNormalizedAxis(A3D::SurfaceChartEntity::Z_Axis, zAxisData);

		chart->setPosition(QVector3D(0, 0, 3));
	}

	{
		A3D::SurfaceChartEntity* chart = s->emplaceChildEntity<A3D::SurfaceChartEntity>();
		float yMax                     = 200;
		float yMin                     = 0;

		A3D::Mesh* sampleMeshC = A3D::Mesh::generateSurfaceMesh(
			s->resourceManager(), { 0, 1, 2, 3, 4, 5, 6 }, { 0, 1, 2, 3, 4, 5, 6 },

			{ 15, 3,  1, 2, 1,  3,  15, 1,  3,  13, 22, 13, 3,  1,  1,  13, 59, 97, 59, 13, 1, 2, 22, 97, 120,
		      97, 22, 2, 1, 13, 59, 97, 59, 13, 1,  1,  3,  13, 22, 13, 3,  1,  15, 3,  1,  2, 1, 3,  15 }
		);
		chart->setTickLength(1);

		chart->loadSurface(sampleMeshC);
		chart->addLinearAxis(A3D::SurfaceChartEntity::X_Axis, 0, 6);
		chart->addLinearAxis(A3D::SurfaceChartEntity::Y_Axis, yMin, yMax);
		chart->addLinearAxis(A3D::SurfaceChartEntity::Z_Axis, 0, 6);

		chart->setPosition(QVector3D(0, 0, 1));
	}

	A3D::View* v = new A3D::View(&w);
	v->camera().setPosition(QVector3D(10.f, 0.f, 2.f));
	v->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	v->setScene(s);

	A3D::KeyCameraController* keyCamController = new A3D::KeyCameraController(v);
	keyCamController->setBaseMovementSpeed(QVector3D(9.f, 9.f, 9.f));
	keyCamController->setKeyBinding(Qt::Key_Space, A3D::KeyCameraController::ACT_MOVE_UPWARD);
	keyCamController->setKeyBinding(Qt::Key_Shift, A3D::KeyCameraController::ACT_MOVE_DOWNWARD);
	keyCamController->setButtonBinding(Qt::LeftButton, A3D::KeyCameraController::ACT_LOOK_MOUSE_POSITION);
	v->setController(keyCamController);

	QTimer t;
	if(v->format().swapInterval() > 0)
		t.setInterval(0);
	else
		t.setInterval(10);
	t.start();

	QObject::connect(&t, &QTimer::timeout, v, &A3D::View::updateView);
	QObject::connect(&t, &QTimer::timeout, s, &A3D::Scene::updateScene);

	KeyEventManager* kem = new KeyEventManager(v);
	{
		A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
		e->setModel(new A3D::Model());
		auto ray = [=](QVector3D endPos) {
			QVector3D cameraPos = v->camera().position();
			A3D::LineGroup::Vertex start;
			start.Position3D = cameraPos;
			start.Color4D    = QVector4D(0, 0, 1, 1);

			A3D::LineGroup::Vertex end;
			end.Position3D = endPos;
			end.Color4D    = QVector4D(0, 0, 1, 1);

			A3D::Group* group = e->model()->getOrAddGroup("test");

			A3D::LineGroup* lg = group->lineGroup();

			if(!lg) {
				lg = new A3D::LineGroup();
				group->setLineGroup(lg);
				lg->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
				lg->setThickness(0.015f);
			}

			lg->vertices().clear();

			lg->vertices().push_back(start);
			lg->vertices().push_back(end);
			lg->invalidateCache();
		};

		kem->setMouseBinding(
			Qt::LeftButton,
			[=]() {
				{
					QPointF cursorPos = v->mapFromGlobal(QCursor::pos());
					QSize size        = v->window()->size();
					QPointF point     = QPointF(cursorPos.x() / size.width(), cursorPos.y() / size.height());
					qDebug() << "Pos " << cursorPos.x() << " " << cursorPos.y();
					qDebug() << "Size " << size.width() << " " << size.height();
					qDebug() << "Point " << point.x() << " " << point.y();

					QMatrix4x4 invProj = v->camera().getProjection().inverted();
					QMatrix4x4 invView = v->camera().getView().inverted();
					float newX         = 2.f * point.x() - 1.f;
					float newY         = -2.f * point.y() + 1.f;
					float newZ         = 10.f;

					QVector4D clip(newX, newY, newZ, 1.f);

					qDebug() << clip;

					QVector4D eye = invProj * clip;
					eye.setW(1.f);

					QVector4D world = invView * eye;
					QVector3D end   = (world.toVector3D() / world.w());
					ray(end * 10.f);
				}
			} // Shooot a ray from the actual position
			/*{
			QVector3D forward     = v->camera().forward();
			QVector3D farPlanePos = forward * v->camera().farPlane();

			A3D::LineGroup::Vertex end;
			end.Position3D = farPlanePos;
			end.Color4D    = QVector4D(1, 0, 0, 1);

			ray(farPlanePos);
		}*/
		);
	}

	//! TODO:
	//! Key Event manager

	v->run();
	s->run();

	w.setCentralWidget(v);
	w.show();
	int rv = a.exec();
	return rv;
}
