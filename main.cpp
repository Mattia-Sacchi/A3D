#include <QApplication>
#include <QMainWindow>
#include <QFile>

#include "A3D/view.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s = new A3D::Scene;

	A3D::Model* sampleModel = nullptr;
	{
		s->resourceManager().registerTexture("Qt-Logo", new A3D::Texture(QImage(":/A3D/Qt-Logo.webp"), &s->resourceManager()));
		sampleModel = s->resourceManager().loadModel("Sample_OBJ", ":/A3D/teapot.obj", A3D::ResourceManager::IF_OBJ);
	}

	A3D::Entity* e = s;
	for(int i = 1; i <= 5; ++i) {
		e = e->emplaceChildEntity<A3D::Entity>();

		if(sampleModel) {
			e->setModel(sampleModel);
			//e->setPosition(QVector3D(0.f, 1.1f, 0.f));
			//e->setScale(QVector3D(0.5f, 0.5f, 0.5f));
			//e->setRotation(QQuaternion::fromAxisAndAngle(0.f, 1.f, 0.f, 15.f));
			break;
		}
		else {
			A3D::Model* m = new A3D::Model(s);
			e->setModel(m);
			A3D::Group* g = m->getOrAddGroup("Default");

			g->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh));
			g->setMaterial(A3D::Material::standardMaterial(A3D::Material::SampleMaterial));
			g->setMaterialProperties(new A3D::MaterialProperties(&s->resourceManager()));

			m->setPosition(QVector3D(0.f, 1.1f * i, 0.f));
			m->setScale(QVector3D(0.5f * i, 0.5f * i, 0.5f * i));
			m->setRotation(QQuaternion::fromAxisAndAngle(0.f, 1.f, 0.f, 15.f * i));

			if(i == 3)
				g->setRotation(QQuaternion::fromAxisAndAngle(1.f, 0.f, 0.f, 45.f));
		}
	}

	A3D::View* v = new A3D::View;
	v->camera().setPosition(QVector3D(0.f, 8.f, 12.f));
	v->setScene(s);
	v->setRenderTimerEnabled(true);

	w.setCentralWidget(v);
	w.show();
	int rv = a.exec();

	v->setScene(nullptr);
	delete s;
	delete v;
	return rv;
}
