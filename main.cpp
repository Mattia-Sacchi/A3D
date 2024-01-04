#include <QApplication>
#include <QMainWindow>

#include "A3D/view.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QMainWindow w;
	
	A3D::Scene* s = new A3D::Scene;
	
	A3D::Entity* e;
	{
		e = s->emplaceChildEntity<A3D::Entity>();
		e->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh));
		e->setMaterial(A3D::Material::standardMaterial(A3D::Material::Basic3DMaterial));
		
	}
	
	A3D::Entity* eXPlus;
	{
		eXPlus = s->emplaceChildEntity<A3D::Entity>();
		eXPlus->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh));
		eXPlus->setMaterial(A3D::Material::standardMaterial(A3D::Material::Basic3DMaterial));
		
		eXPlus->setPosition(QVector3D(0.f, 5.f, 0.f));
		eXPlus->setScale(QVector3D(0.5f, 0.5f, 0.5f));
	}
	
	A3D::View* v = new A3D::View;
	v->camera().setPosition(QVector3D(0.f, 0.f, 10.f));
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
