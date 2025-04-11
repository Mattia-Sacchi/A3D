#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>

#include "A3D/view.h"
#include "A3D/keyboardcameracontroller.h"
#include "A3D/textbillboardentity.h"

int main(int argc, char* argv[]) {

	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s          = new A3D::Scene(&w);
	A3D::PointLightInfo& l = s->getOrCreateLight(0);
	l.position             = QVector3D(0.f, 2.2f, 5.f);
	l.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	A3D::MaterialProperties* Concrete002  = nullptr;
	A3D::MaterialProperties* Metal035     = nullptr;
	A3D::MaterialProperties* FloorTiles06 = nullptr;
	A3D::Cubemap* Cubemap001              = nullptr;

	{
		auto loadPBRMaterial = [s](QString path, QString baseName, QString fileExtension) -> A3D::MaterialProperties* {
			A3D::MaterialProperties* matProperties                                  = new A3D::MaterialProperties(s->resourceManager());
			static std::map<A3D::MaterialProperties::TextureSlot, QString> suffixes = {
				{   A3D::MaterialProperties::AlbedoTextureSlot,     "Color"},
				{   A3D::MaterialProperties::NormalTextureSlot,  "NormalGL"},
				{ A3D::MaterialProperties::MetallicTextureSlot,  "Metallic"},
				{A3D::MaterialProperties::RoughnessTextureSlot, "Roughness"},
				{       A3D::MaterialProperties::AOTextureSlot,        "AO"},
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
	}

	{
		A3D::Model* model = new A3D::Model();

		{
			A3D::Mesh* sampleMeshA = A3D::Mesh::generateSurfaceMesh(
				s->resourceManager(), { 100, 200, 300, 400, 500, 600, 800, 1200 }, { 1000, 2000, 3000, 3500, 4000, 5000, 6000, 7000 },
				{ 290, 290, 290, 270, 300, 400, 500, 500, 290, 290, 290, 250, 250, 400, 500, 500, 250, 250, 220, 220, 200, 300, 400, 400, 220, 200, 180, 180, 200, 250, 300, 100,
			      220, 200, 180, 160, 100, 100, 100, 100, 180, 160, 160, 120, 50,  50,  50,  40,  170, 150, 150, 100, 50,  40,  40,  40,  150, 120, 120, 100, 50,  40,  40,  40 }
			);

			A3D::Group* g = model->getOrAddGroup("Default");
			g->setMesh(sampleMeshA);
			g->setMaterial(A3D::Material::standardMaterial(A3D::Material::PBRMaterial));
			g->setMaterialProperties(FloorTiles06);
		}

		A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
		e->setModel(model);
	}

	{

		A3D::Model* model = new A3D::Model();

		{

			A3D::Mesh* sampleMeshB = A3D::Mesh::generateSurfaceMesh(
				s->resourceManager(), { 0, 10, 15, 20, 30, 40, 50, 75, 100 }, { 0, 1, 2, 3, 4, 5 },

				{ 1800, 2000, 2600, 3000, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100,
			      1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100 }
			);

			A3D::Group* g = model->getOrAddGroup("Default");

			A3D::LineGroup* lg = new A3D::LineGroup(s->resourceManager());

			{
				A3D::LineGroup::Vertex vxOrigin, vxMaxX, vxMaxY, vxMaxZ;
				vxOrigin.Position3D = QVector3D(0.f, 0.f, 0.f);
				vxOrigin.Color4D    = QVector4D(0.f, 0.f, 0.f, 1.f);
				vxMaxX.Position3D   = QVector3D(1.f, 0.f, 0.f);
				vxMaxX.Color4D      = QVector4D(1.f, 0.f, 0.f, 1.f);
				vxMaxY.Position3D   = QVector3D(0.f, 1.f, 0.f);
				vxMaxY.Color4D      = QVector4D(0.f, 1.f, 0.f, 1.f);
				vxMaxZ.Position3D   = QVector3D(0.f, 0.f, 1.f);
				vxMaxZ.Color4D      = QVector4D(0.f, 0.f, 1.f, 1.f);

				lg->vertices().push_back(vxOrigin);
				lg->vertices().push_back(vxMaxX);
				lg->vertices().push_back(vxOrigin);
				lg->vertices().push_back(vxMaxY);
				lg->vertices().push_back(vxOrigin);
				lg->vertices().push_back(vxMaxZ);
				lg->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
				lg->setThickness(0.05f);
			}

			g->setLineGroup(lg);
			g->setMesh(sampleMeshB);
			g->setMaterial(A3D::Material::standardMaterial(A3D::Material::PBRMaterial));
			g->setMaterialProperties(FloorTiles06);
		}

		A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
		e->setModel(model);
		model->setPosition(QVector3D(2, 0, 0));
	}

	{
		auto text = s->emplaceChildEntity<A3D::TextBillboardEntity>();

		text->setFont(QFont("Arial", 64));
		text->setColor(Qt::red);
		text->setText("Hello, world!");
		text->setProperty("Value", 0);

		QTimer* t = new QTimer();
		t->start(100);
		QObject::connect(t, &QTimer::timeout, t, [=]() {
			int val = text->property("Value").toInt();
			val++;
			text->setProperty("Value", val);

			text->setText(QString("Hello, world! %1").arg(val));
		});
	}

	A3D::View* v = new A3D::View(&w);
	v->camera().setPosition(QVector3D(10.f, 0.f, 2.f));
	v->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	v->setScene(s);

	A3D::KeyboardCameraController* keyCamController = new A3D::KeyboardCameraController(v);
	keyCamController->setBaseMovementSpeed(QVector3D(9.f, 9.f, 9.f));
	keyCamController->setKeyBinding(Qt::Key_Space, A3D::KeyboardCameraController::ACT_MOVE_UPWARD);
	keyCamController->setKeyBinding(Qt::Key_Shift, A3D::KeyboardCameraController::ACT_MOVE_DOWNWARD);

	v->setController(keyCamController);

	QTimer t;
	if(v->format().swapInterval() > 0)
		t.setInterval(0);
	else
		t.setInterval(10);
	t.start();

	QObject::connect(&t, &QTimer::timeout, v, &A3D::View::updateView);
	QObject::connect(&t, &QTimer::timeout, s, &A3D::Scene::updateScene);

	v->run();
	s->run();

	w.setCentralWidget(v);
	w.show();
	int rv = a.exec();
	return rv;
}
