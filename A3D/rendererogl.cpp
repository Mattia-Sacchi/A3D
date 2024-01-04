#include "rendererogl.h"

namespace A3D {

RendererOGL::RendererOGL(QOpenGLContext* ctx)
	: Renderer(),
	  m_context(ctx)
{
	dbgConstruct("RendererOGL")
}

RendererOGL::~RendererOGL() {
	dbgDestruct("RendererOGL started")
	DeleteAllResources();
	dbgDestruct("RendererOGL finished")
}

void RendererOGL::Draw(CoreGLFunctions* gl, Entity* root, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix) {
	std::vector<QPointer<Entity>> const& entities = root->childrenEntities();
	
	for(auto it = entities.begin(); it != entities.end(); ++it) {
		QPointer<Entity> const& p = *it;
		if(p.isNull())
			continue;
		
		Entity* e = p.get();
		Mesh* mesh = e->mesh();
		Material* mat = e->material();
		if(!mesh || !mat)
			continue;
		
		MeshCacheOGL* meshCache = buildMeshCache(gl, mesh);
		MaterialCacheOGL* matCache = buildMaterialCache(gl, mat);
		
		matCache->render(gl, parentMatrix * e->modelMatrix(), viewMatrix, projMatrix);
		meshCache->render(gl);
	}
}

class ContextSwitcher : public NonCopyable {
public:
	ContextSwitcher(QOpenGLContext* newContext)
		: m_oldContext(QOpenGLContext::currentContext()),
		  m_newContext(newContext)
	{
		if(m_newContext && m_newContext != m_oldContext)
			m_newContext->makeCurrent(m_newContext->surface());
	}
	
	~ContextSwitcher() {
		if(m_newContext != m_oldContext) {
			if(m_oldContext)
				m_oldContext->makeCurrent(m_oldContext->surface());
			else if(m_newContext)
				m_newContext->doneCurrent();
		}
	}
	
private:
	QOpenGLContext* m_oldContext;
	QOpenGLContext* m_newContext;
};

void RendererOGL::Delete(MeshCache* meshCache) {
	if(m_context.isNull()) {
		qDebug() << "Couldn't delete MeshCache: OpenGL context is unavailable. A memory leak might have happened.";
		return;
	}
	
	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);
	
	delete meshCache;
}

void RendererOGL::Delete(MaterialCache* matCache) {
	if(m_context.isNull()) {
		qDebug() << "Couldn't delete MaterialCache: OpenGL context is unavailable. A memory leak might have happened.";
		return;
	}
	
	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);
	
	delete matCache;
}

void RendererOGL::DeleteAllResources() {
	if(m_context.isNull()) {
		qDebug() << "Couldn't delete resources: OpenGL context is unavailable. A memory leak might have happened.";
		return;
	}
	
	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);
	
	Renderer::runDeleteOnAllResources();
}

MeshCacheOGL* RendererOGL::buildMeshCache(CoreGLFunctions* gl, Mesh* mesh) {
	std::pair<MeshCacheOGL*, bool> mc = mesh->getOrEmplaceMeshCache<MeshCacheOGL>(rendererID());
	
	if(mc.first->isDirty())
		mc.first->update(gl);
	
	if(mc.second)
		addToMeshCaches(mc.first);
	
	return mc.first;
}

MaterialCacheOGL* RendererOGL::buildMaterialCache(CoreGLFunctions* gl, Material* material) {
	std::pair<MaterialCacheOGL*, bool> mc = material->getOrEmplaceMaterialCache<MaterialCacheOGL>(rendererID());
	
	if(mc.first->isDirty())
		mc.first->update(gl);
	
	if(mc.second)
		addToMaterialCaches(mc.first);
	
	return mc.first;
}

}
