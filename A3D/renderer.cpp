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

struct OpaqueSorter {
	OpaqueSorter(Camera const& c) : m_camera(c) {}
	
	bool operator() (std::pair<Entity*, QMatrix4x4> const& a, std::pair<Entity*, QMatrix4x4> const& b) const
	{
		return
			QVector3D::dotProduct(a.first->position() - m_camera.position(), m_camera.forward())
			< QVector3D::dotProduct(b.first->position() - m_camera.position(), m_camera.forward());
	}
	
private:
	Camera const& m_camera;
};

struct TranslucentSorter {
	TranslucentSorter(Camera const& c) : m_camera(c) {}
	
	bool operator() (std::pair<Entity*, QMatrix4x4> const& a, std::pair<Entity*, QMatrix4x4> const& b) const
	{
		return
			QVector3D::dotProduct(a.first->position() - m_camera.position(), m_camera.forward())
			> QVector3D::dotProduct(b.first->position() - m_camera.position(), m_camera.forward());
	}
	
private:
	Camera const& m_camera;
};

void Renderer::DrawAll(Entity* root, Camera const& camera) {
	m_opaqueEntityBuffer.clear();
	m_translucentEntityBuffer.clear();
	
	BuildDrawLists(root, QMatrix4x4());
	
	{
		OpaqueSorter os(camera);	
		std::stable_sort(m_opaqueEntityBuffer.begin(), m_opaqueEntityBuffer.end(), os);
	}
	
	{
		TranslucentSorter ts(camera);
		std::stable_sort(m_translucentEntityBuffer.begin(), m_translucentEntityBuffer.end(), ts);
	}
	
	QMatrix4x4 const& projMatrix = camera.getProjection();
	QMatrix4x4 const& viewMatrix = camera.getView();
	
	this->BeginOpaque();
	for(auto it = m_opaqueEntityBuffer.begin(); it != m_opaqueEntityBuffer.end(); ++it) {
		this->Draw(it->first, it->second, projMatrix, viewMatrix);
	}
	this->EndOpaque();
	
	this->BeginTranslucent();
	for(auto it = m_translucentEntityBuffer.begin(); it != m_translucentEntityBuffer.end(); ++it) {
		this->Draw(it->first, it->second, projMatrix, viewMatrix);
	}
	this->EndTranslucent();
}

void Renderer::BuildDrawLists(Entity* e, QMatrix4x4 cascadeMatrix) {
	if(e->renderOptions() & Entity::Hidden)
		return;
	
	cascadeMatrix *= e->modelMatrix();
	
	Mesh* mesh = e->mesh();
	Material* mat = e->material();
	if(mesh && mat) {
		if(mat->renderOptions() & Material::Translucent) {
			m_translucentEntityBuffer.push_back(std::make_pair(e, cascadeMatrix));
		} else {
			m_opaqueEntityBuffer.push_back(std::make_pair(e, cascadeMatrix));
		}
	}
	
	std::vector<QPointer<Entity>> const& subEntities = e->childrenEntities();
	for(auto it = subEntities.begin(); it != subEntities.end(); ++it) {
		if(it->isNull())
			continue;
		BuildDrawLists(it->get(), cascadeMatrix);
	}
}

void Renderer::BeginOpaque() {}
void Renderer::EndOpaque() {}
void Renderer::BeginTranslucent() {}
void Renderer::EndTranslucent() {}

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
