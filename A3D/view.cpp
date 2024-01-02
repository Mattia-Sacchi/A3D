#include "view.h"

namespace A3D {

View::View(QWidget *parent)
	: QOpenGLWidget{parent},
	  m_scene(nullptr)
{
	QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
	fmt.setSamples(4);
	fmt.setDepthBufferSize(24);
	fmt.setVersion(3, 3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setAlphaBufferSize(8);
	
	setFormat(fmt);
}

View::~View() {
	
}

}
