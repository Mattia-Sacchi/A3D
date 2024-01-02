#include <QApplication>
#include <QMainWindow>

#include "A3D/view.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QMainWindow w;
	A3D::View* v = new A3D::View;
	v->camera().setPosition(QVector3D(-10.f, 0.f, 0.f));
	v->camera().setOrientationTarget(QVector3D(0.f,0.f,0.f));
	v->camera().setPerspective(45.f, 16.f/9.f);
	w.setCentralWidget(v);
	w.show();
	return a.exec();
}
