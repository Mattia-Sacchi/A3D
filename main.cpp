#include <QApplication>
#include <QMainWindow>
#include <QFile>

#include "A3D/view.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s = new A3D::Scene(&w);

	A3D::Model* sampleModel = nullptr;
	A3D::MaterialProperties* pbrGold = nullptr;

	{
		s->resourceManager().registerTexture("Foil_AO", new A3D::Texture(QImage("C:\\Users\\mauro.grassia\\Desktop\\FoilPBR\\Foil_AO.png")));
		s->resourceManager().registerTexture("Foil_Color", new A3D::Texture(QImage("C:\\Users\\mauro.grassia\\Desktop\\FoilPBR\\Foil_Color.png")));
		s->resourceManager().registerTexture("Foil_Displacement", new A3D::Texture(QImage("C:\\Users\\mauro.grassia\\Desktop\\FoilPBR\\Foil_Displacement.png")));
		s->resourceManager().registerTexture("Foil_Metalness", new A3D::Texture(QImage("C:\\Users\\mauro.grassia\\Desktop\\FoilPBR\\Foil_Metalness.png")));
		s->resourceManager().registerTexture("Foil_Normal", new A3D::Texture(QImage("C:\\Users\\mauro.grassia\\Desktop\\FoilPBR\\Foil_NormalGL.png")));
		s->resourceManager().registerTexture("Foil_Roughness", new A3D::Texture(QImage("C:\\Users\\mauro.grassia\\Desktop\\FoilPBR\\Foil_Roughness.png")));

		pbrGold = new A3D::MaterialProperties(&s->resourceManager());
		pbrGold->setTexture(s->resourceManager().getLoadedTexture("Foil_AO"), A3D::MaterialProperties::AOTextureSlot);
		pbrGold->setTexture(s->resourceManager().getLoadedTexture("Foil_Color"), A3D::MaterialProperties::AlbedoTextureSlot);
		pbrGold->setTexture(s->resourceManager().getLoadedTexture("Foil_Metalness"), A3D::MaterialProperties::MetallicTextureSlot);
		pbrGold->setTexture(s->resourceManager().getLoadedTexture("Foil_Normal"), A3D::MaterialProperties::NormalTextureSlot);
		pbrGold->setTexture(s->resourceManager().getLoadedTexture("Foil_Roughness"), A3D::MaterialProperties::RoughnessTextureSlot);

		sampleModel = s->resourceManager().loadModel("Sample_OBJ", ":/A3D/sphere.obj");
	}

	A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
	{
		A3D::Model* m = sampleModel->clone(true);
		e->setModel(m);

		for(auto it = m->groups().begin(); it != m->groups().end(); ++it) {
			A3D::Group* g = it->second;
			g->setMaterial(A3D::Material::standardMaterial(A3D::Material::PBRMaterial));
			g->setMaterialProperties(pbrGold);
		}
	}


	{
		QVector3D const fakeLights[4] = {
			{1,7,0},
			{-6,10,3},
			{3,-7,0},
			{6,-2,-5}
		};
		QVector3D const fakeLightColors[4] = {
			{1,1,1},
			{1,1,1},
			{1,1,1},
			{1,0,0}
		};

		for(int i = 0; i < 4; ++i) {
			A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
			A3D::Model* m = sampleModel->clone(true);
			e->setModel(m);
			e->setScale(QVector3D(0.2f,0.2f,0.2f));
			e->setPosition(fakeLights[i]);

			for(auto it = m->groups().begin(); it != m->groups().end(); ++it) {
				A3D::Group* g = it->second;
				g->setMaterial(A3D::Material::standardMaterial(A3D::Material::SampleMaterial));
				g->setMaterialProperties(new A3D::MaterialProperties(&s->resourceManager()));
				g->materialProperties()->setRawValue("Color", fakeLightColors[i]);
			}
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
