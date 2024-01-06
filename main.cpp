#include <QApplication>
#include <QMainWindow>

#include "A3D/view.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s = new A3D::Scene;

	{
		s->resourceManager().registerTexture("Qt-Logo", new A3D::Texture(QImage(":/A3D/Qt-Logo.webp"), &s->resourceManager()));
	}

	A3D::Entity* e = s;
	for(int i = 0; i < 5; ++i) {
		e = e->emplaceChildEntity<A3D::Entity>();
		e->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh));
		e->setMaterial(A3D::Material::standardMaterial(A3D::Material::Basic3DMaterial));

		e->setPosition(QVector3D(0.f, 1.1f, 0.f));
		e->setScale(QVector3D(0.5f, 0.5f, 0.5f));
		e->setRotation(QQuaternion::fromAxisAndAngle(0.f, 1.f, 0.f, 15.f));
	}

	A3D::View* v = new A3D::View;
	v->camera().setPosition(QVector3D(0.f, 4.f, 10.f));
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
