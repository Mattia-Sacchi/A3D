#include "renderer.h"

namespace A3D {

static std::uintptr_t g_lastRendererID = 0;
static std::map<std::uintptr_t, Renderer*> g_renderers;

std::uintptr_t Renderer::createRendererID(Renderer* renderer) {
	
	do {
		++g_lastRendererID;
	} while(g_renderers.find(g_lastRendererID) != g_renderers.end());
	
	g_renderers[g_lastRendererID] = renderer;
	return g_lastRendererID;
}

void Renderer::removeRendererID(std::uintptr_t rendererID) {
	g_renderers.erase(rendererID);
}
Renderer* Renderer::getRenderer(std::uintptr_t rendererID) {
	auto it = g_renderers.find(rendererID);
	if(it == g_renderers.end())
		return nullptr;
	return it->second;
}



Renderer::Renderer()
	: m_rendererID(0)
{
	dbgConstruct("Renderer")
	m_rendererID = Renderer::createRendererID(this);
}

Renderer::~Renderer() {
	removeRendererID(rendererID());
	dbgDestruct("Renderer")
}

std::uintptr_t Renderer::rendererID() const {
	return m_rendererID;
}

void Renderer::CleanupRenderCache() {
	cleanupQPointers(m_meshCaches);
	cleanupQPointers(m_materialCaches);
}

void Renderer::runDeleteOnAllResources() {
	for(auto it = m_meshCaches.begin(); it != m_meshCaches.end(); ++it) {
		QPointer<MeshCache>& mc = *it;
		if(mc.isNull())
			continue;
		delete mc.get();
	}
	m_meshCaches.clear();
	
	for(auto it = m_materialCaches.begin(); it != m_materialCaches.end(); ++it) {
		QPointer<MaterialCache>& mc = *it;
		if(mc.isNull())
			continue;
		delete mc.get();
	}
	m_materialCaches.clear();
}

void Renderer::addToMaterialCaches(QPointer<MaterialCache> material) {
	m_materialCaches.push_back(material);
}

void Renderer::addToMeshCaches(QPointer<MeshCache> mesh) {
	m_meshCaches.push_back(mesh);
}

}
