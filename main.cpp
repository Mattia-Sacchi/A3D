#include <QApplication>
#include <QMainWindow>

#include "A3D/view.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QMainWindow w;
	
	A3D::Scene* s = new A3D::Scene;
	
	{
		A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
		e->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh));
		e->setMaterial(A3D::Material::standardMaterial(A3D::Material::Basic3DMaterial));
		
	}
	
	{	
		A3D::Entity* eXPlus = s->emplaceChildEntity<A3D::Entity>();
		eXPlus->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh));
		eXPlus->setMaterial(A3D::Material::standardMaterial(A3D::Material::Basic3DMaterial));
		
		QMatrix4x4 mx = QMatrix4x4();
		mx.translate(0.f, 1.f, 0.f);
		mx.scale(0.6f);
		eXPlus->setModelMatrix(mx);
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
