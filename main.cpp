#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>

#include "A3D/view.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s = new A3D::Scene(&w);
	//A3D::PointLightInfo& l = s->getOrCreateLight(0);
	//l.position = QVector3D(0.f, 2.2f, 5.f);
	//l.color = QVector4D(1.f, 1.f, 1.f, 500.f);

	A3D::MaterialProperties* Concrete002 = nullptr;
	A3D::MaterialProperties* Metal035 = nullptr;
	A3D::MaterialProperties* FloorTiles06 = nullptr;

	A3D::Cubemap* Cubemap001 = nullptr;

	A3D::Model* sampleModel = nullptr;

	{
		auto loadPBRMaterial = [s](QString path, QString baseName, QString fileExtension) -> A3D::MaterialProperties* {
			A3D::MaterialProperties* matProperties = new A3D::MaterialProperties(s->resourceManager());
			static std::map<A3D::MaterialProperties::TextureSlot, QString> suffixes = {
				{A3D::MaterialProperties::AlbedoTextureSlot, "Color"},
				{A3D::MaterialProperties::NormalTextureSlot, "NormalGL"},
				{A3D::MaterialProperties::MetallicTextureSlot, "Metallic"},
				{A3D::MaterialProperties::RoughnessTextureSlot, "Roughness"},
				{A3D::MaterialProperties::AOTextureSlot, "AO"},
			};

			for(std::pair<A3D::MaterialProperties::TextureSlot, QString> const& suffix : qAsConst(suffixes)) {
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
		FloorTiles06->setParent(s->resourceManager());

		sampleModel = s->resourceManager()->loadModel("Sphere", ":/A3D/SampleResources/Models/Sphere/Sphere.obj");
		if(!sampleModel) {
			sampleModel = new A3D::Model(s->resourceManager());
			A3D::Group* g = sampleModel->getOrAddGroup("Default");
			g->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh));
		}
	}

	A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
	A3D::Model* m = sampleModel->clone(true);
	e->setModel(m);

	for(auto it = m->groups().begin(); it != m->groups().end(); ++it) {
		A3D::Group* g = it->second;
		// g->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh));
		g->setMaterial(A3D::Material::standardMaterial(A3D::Material::PBRMaterial));
		g->setMaterialProperties(FloorTiles06);
	}


	A3D::View* v = new A3D::View(&w);
	v->camera().setPosition(QVector3D(10.f, 0.f, 2.f));
	v->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	v->setScene(s);
	v->setRenderTimerEnabled(true);

	w.setCentralWidget(v);
	w.show();
	int rv = a.exec();
	return rv;
}
