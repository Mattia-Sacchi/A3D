#include "A3D/rendererogl.h"

namespace A3D {

RendererOGL::RendererOGL(QOpenGLContext* ctx, CoreGLFunctions* gl)
	: Renderer(),
	  m_context(ctx),
	  m_gl(gl) {
	log(LC_Debug, "Constructor: RendererOGL");
}

RendererOGL::~RendererOGL() {
	log(LC_Debug, "Destructor: RendererOGL (start)");
	DeleteAllResources();
	log(LC_Debug, "Destructor: RendererOGL (end)");
}

void RendererOGL::Draw(Group* g, DrawInfo const& drawInfo) {
	if(!g || g->renderOptions() & Group::Hidden)
		return;

	Mesh* mesh                  = g->mesh();
	Material* mat               = g->material();
	MaterialProperties* matProp = g->materialProperties();
	if(!mesh || !mat || !matProp)
		return;

	MeshCacheOGL* meshCache                  = buildMeshCache(mesh);
	MaterialCacheOGL* matCache               = buildMaterialCache(mat);
	MaterialPropertiesCacheOGL* matPropCache = buildMaterialPropertiesCache(matProp);

	Mesh::RenderOptions meshRenderOptions    = mesh->renderOptions();
	Material::RenderOptions matRenderOptions = mat->renderOptions();

	if(meshRenderOptions & Mesh::DisableCulling || matRenderOptions & Material::Translucent || matProp->isTranslucent())
		m_gl->glDisable(GL_CULL_FACE);

	matCache->install(m_gl);
	matPropCache->install(m_gl, matCache);
	for(std::size_t i = 0; i < MaterialProperties::MaxTextures; ++i) {
		Texture* t = matProp->texture(static_cast<MaterialProperties::TextureSlot>(i));
		if(t) {
			TextureCacheOGL* tCache = buildTextureCache(t);
			tCache->applyToSlot(m_gl, static_cast<GLuint>(i));
		}
	}
	meshCache->render(m_gl, drawInfo.m_modelMatrix, drawInfo.m_viewMatrix, drawInfo.m_projMatrix);

	if(meshRenderOptions & Mesh::DisableCulling || matRenderOptions & Material::Translucent || matProp->isTranslucent())
		m_gl->glEnable(GL_CULL_FACE);
}

void RendererOGL::BeginOpaque() {
	m_gl->glEnable(GL_DEPTH_TEST);
	m_gl->glDepthFunc(GL_LESS);
}
void RendererOGL::EndOpaque() {}
void RendererOGL::BeginTranslucent() {
	m_gl->glDepthMask(GL_FALSE);
	m_gl->glEnable(GL_BLEND);
	m_gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void RendererOGL::EndTranslucent() {
	m_gl->glDepthMask(GL_TRUE);
	m_gl->glDisable(GL_BLEND);
}

class ContextSwitcher : public NonCopyable {
public:
	ContextSwitcher(QOpenGLContext* newContext)
		: m_oldContext(QOpenGLContext::currentContext()),
		  m_newContext(newContext) {
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
		log(LC_Debug, "Couldn't delete MeshCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete meshCache;
}

void RendererOGL::Delete(MaterialCache* matCache) {
	if(m_context.isNull()) {
		log(LC_Debug, "Couldn't delete MaterialCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete matCache;
}

void RendererOGL::Delete(MaterialPropertiesCache* matPropCache) {
	if(m_context.isNull()) {
		log(LC_Debug, "Couldn't delete MaterialPropertiesCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete matPropCache;
}

void RendererOGL::Delete(TextureCache* texCache) {
	if(m_context.isNull()) {
		log(LC_Debug, "Couldn't delete TextureCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete texCache;
}

void RendererOGL::DeleteAllResources() {
	if(m_context.isNull()) {
		log(LC_Debug, "Couldn't delete resources: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	Renderer::runDeleteOnAllResources();
}

void RendererOGL::PreLoadEntity(Entity* e) {
	if(!e)
		return;

	Model* model = e->model();
	if(!model || model->renderOptions() & Model::Hidden)
		return;

	std::map<QString, QPointer<Group>> const& groups = model->groups();
	for(auto it = groups.begin(); it != groups.end(); ++it) {
		Group* g = it->second;
		if(!g)
			continue;

		Mesh* mesh                  = g->mesh();
		Material* mat               = g->material();
		MaterialProperties* matProp = g->materialProperties();

		if(mesh)
			buildMeshCache(mesh);

		if(mat)
			buildMaterialCache(mat);

		if(matProp) {
			buildMaterialPropertiesCache(matProp);

			for(std::size_t i = 0; i < MaterialProperties::MaxTextures; ++i) {
				if(Texture* t = matProp->texture(static_cast<MaterialProperties::TextureSlot>(i)))
					buildTextureCache(t);
			}
		}
	}
}

MeshCacheOGL* RendererOGL::buildMeshCache(Mesh* mesh) {
	std::pair<MeshCacheOGL*, bool> mc = mesh->getOrEmplaceMeshCache<MeshCacheOGL>(rendererID());

	if(mc.first->isDirty())
		mc.first->update(m_gl);

	if(mc.second)
		addToMeshCaches(mc.first);

	return mc.first;
}

MaterialCacheOGL* RendererOGL::buildMaterialCache(Material* material) {
	std::pair<MaterialCacheOGL*, bool> mc = material->getOrEmplaceMaterialCache<MaterialCacheOGL>(rendererID());

	if(mc.first->isDirty())
		mc.first->update(m_gl);

	if(mc.second)
		addToMaterialCaches(mc.first);

	return mc.first;
}
MaterialPropertiesCacheOGL* RendererOGL::buildMaterialPropertiesCache(MaterialProperties* materialProperties) {
	std::pair<MaterialPropertiesCacheOGL*, bool> mc = materialProperties->getOrEmplaceMaterialPropertiesCache<MaterialPropertiesCacheOGL>(rendererID());

	if(mc.first->isDirty())
		mc.first->update(m_gl);

	if(mc.second)
		addToMaterialPropertiesCaches(mc.first);

	return mc.first;
}

TextureCacheOGL* RendererOGL::buildTextureCache(Texture* texture) {
	std::pair<TextureCacheOGL*, bool> tc = texture->getOrEmplaceTextureCache<TextureCacheOGL>(rendererID());

	if(tc.first->isDirty())
		tc.first->update(m_gl);

	if(tc.second)
		addToTextureCaches(tc.first);

	return tc.first;
}

}
