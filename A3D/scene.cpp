#include "scene.h"

namespace A3D {

Scene::Scene(QObject *parent)
	: Entity{nullptr}
{
	QObject::setParent(parent);
	dbgConstruct("Scene")
}
Scene::~Scene() {
	dbgDestruct("Scene")
}

}
