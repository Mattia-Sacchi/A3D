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

ResourceManager& Scene::resourceManager() {
	return m_resourceManager;
}

ResourceManager const& Scene::resourceManager() const {
	return m_resourceManager;
}

void Scene::setAmbientLight(QVector3D const& ambientLight) {
	m_ambientLight = ambientLight;
}
QVector3D Scene::ambientLight() const {
	return m_ambientLight;
}

}
