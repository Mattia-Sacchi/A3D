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

ResourceManager* Scene::resourceManager() {
	return &m_resourceManager;
}

ResourceManager const* Scene::resourceManager() const {
	return &m_resourceManager;
}

PointLightInfo& Scene::getOrCreateLight(std::size_t id) {
	return m_lights[id];
}
PointLightInfo const* Scene::getLight(std::size_t id) const {
	auto it = m_lights.find(id);
	if(it == m_lights.end())
		return nullptr;
	return &(it->second);
}
std::map<std::size_t, PointLightInfo> const& Scene::lights() const {
	return m_lights;
}

void Scene::setSkybox(Cubemap* c) {
	m_skybox = c;
}
Cubemap* Scene::skybox() const {
	return m_skybox;
}

}
