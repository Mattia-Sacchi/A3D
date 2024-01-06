#include "A3D/scene.h"

namespace A3D {

Scene::Scene(QObject* parent)
	: Entity{ nullptr } {
	QObject::setParent(parent);
	log(LC_Debug, "Constructor: Scene");
}
Scene::~Scene() {
	log(LC_Debug, "Destructor: Scene");
}

}
