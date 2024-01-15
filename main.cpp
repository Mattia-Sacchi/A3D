#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>

#include "A3D/view.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s = new A3D::Scene(&w);
	A3D::Scene::PointLightInfo& l = s->getOrCreateLight(0);
	l.position = QVector3D(0.f, 2.2f, 5.f);
	l.color = QVector4D(1.f, 1.f, 1.f, 500.f);

	A3D::MaterialProperties* Concrete002 = nullptr;
	A3D::MaterialProperties* Metal035 = nullptr;

	A3D::Model* sampleModel = nullptr;

	{
		auto loadPBRMaterial = [s](QString path, QString baseName, QString fileExtension) -> A3D::MaterialProperties* {
			A3D::MaterialProperties* matProperties = new A3D::MaterialProperties(&s->resourceManager());
			static std::map<A3D::MaterialProperties::TextureSlot, QString> suffixes = {
				{A3D::MaterialProperties::AlbedoTextureSlot, "Color"},
				{A3D::MaterialProperties::NormalTextureSlot, "NormalGL"},
				{A3D::MaterialProperties::MetallicTextureSlot, "Metallic"},
				{A3D::MaterialProperties::RoughnessTextureSlot, "Roughness"},
				{A3D::MaterialProperties::AOTextureSlot, "AO"},
			};

			for(std::pair<A3D::MaterialProperties::TextureSlot, QString> const& suffix : qAsConst(suffixes)) {
				QImage img(path + QDir::separator() + baseName + "_" + suffix.second + "." + fileExtension);
				if(!img.isNull()) {
					A3D::Texture* texture = new A3D::Texture(std::move(img), &s->resourceManager());
					s->resourceManager().registerTexture(baseName + "_" + suffix.second, texture);

					matProperties->setTexture(texture, suffix.first);
				}
			}
			return matProperties;
		};

		Concrete002 = loadPBRMaterial(":/A3D/Materials/Concrete002", "Concrete002_4K", "jpg");
		Concrete002->setParent(&s->resourceManager()); // Just to get the clang static analyzer to piss off...

		Metal035 = loadPBRMaterial(":/A3D/Materials/Metal035", "Metal035_2K-JPG", "jpg");
		Metal035->setParent(&s->resourceManager()); // Just to get the clang static analyzer to piss off...

		sampleModel = s->resourceManager().loadModel("Sphere", ":/A3D/Models/Sphere/Sphere.obj");
	}

	A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
	{
		A3D::Model* m = sampleModel->clone(true);
		e->setModel(m);

		for(auto it = m->groups().begin(); it != m->groups().end(); ++it) {
			A3D::Group* g = it->second;
			g->setMaterial(A3D::Material::standardMaterial(A3D::Material::PBRMaterial));
			g->setMaterialProperties(Metal035);
		}
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
