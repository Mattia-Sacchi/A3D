#include "A3D/rendererogl.h"
#include <cstddef>

namespace A3D {

RendererOGL::RendererOGL(QOpenGLContext* ctx, CoreGLFunctions* gl)
	: Renderer(),
	  m_context(ctx),
	  m_gl(gl),
	  m_skyboxMaterial(nullptr),
	  m_skyboxMesh(nullptr),
	  m_sceneUBO(0) {
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

	{
		SceneUBO_Data newSceneData = m_sceneData;
		if(m_closestSceneLightsBuffer.capacity() < LightCount)
			m_closestSceneLightsBuffer.reserve(LightCount);

		getClosestSceneLights(drawInfo.m_groupPosition, LightCount, m_closestSceneLightsBuffer);

		std::size_t lightCount = std::min<std::size_t>(LightCount, m_closestSceneLightsBuffer.size());

		for(std::size_t i = 0; i < lightCount; ++i) {
			newSceneData.m_lightPos[i] = QVector4D(m_closestSceneLightsBuffer[i].second.position, 0.f);
			newSceneData.m_lightColor[i] = m_closestSceneLightsBuffer[i].second.color;
		}
		for(std::size_t i = lightCount; i < LightCount; ++i) {
			newSceneData.m_lightColor[i] = QVector4D(0.f, 0.f, 0.f, 0.f);
			newSceneData.m_lightPos[i] = QVector4D(0.f, 0.f, 0.f, -1.f);
		}

		if(std::memcmp(&m_sceneData, &newSceneData, sizeof(m_sceneData))) {
			std::memcpy(&m_sceneData, &newSceneData, sizeof(m_sceneData));
			RefreshSceneUBO();
		}
	}


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


void RendererOGL::RefreshSceneUBO() {
	if(!m_sceneUBO)
		return;
	m_gl->glBindBuffer(GL_UNIFORM_BUFFER, m_sceneUBO);
	m_gl->glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_sceneData), &m_sceneData);
	m_gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RendererOGL::BeginDrawing(Camera const& cam, Scene const* scene) {
	Renderer::BeginDrawing(cam, scene);
	m_gl->glDisable(GL_BLEND);
	m_gl->glEnable(GL_CULL_FACE);
	m_gl->glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	m_gl->glClearColor(0.f, 0.f, 0.f, 0.f);
	m_gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_skyboxView = cam.getView();
	m_skyboxProj = cam.getProjection();

	if(!m_sceneUBO) {
		m_gl->glGenBuffers(1, &m_sceneUBO);

		if(!m_sceneUBO)
			return;

		m_gl->glBindBuffer(GL_UNIFORM_BUFFER, m_sceneUBO);
		m_gl->glBufferData(GL_UNIFORM_BUFFER, sizeof(m_sceneData), nullptr, GL_DYNAMIC_DRAW);
		m_gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	if(m_sceneData.m_cameraPos != cam.position()) {
		m_sceneData.m_cameraPos = cam.position();
		m_gl->glBindBuffer(GL_UNIFORM_BUFFER, m_sceneUBO);
		m_gl->glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SceneUBO_Data, m_cameraPos), sizeof(m_sceneData.m_cameraPos), &m_sceneData.m_cameraPos);
		m_gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	m_gl->glBindBufferBase(GL_UNIFORM_BUFFER, RendererOGL::UBO_SceneBinding, m_sceneUBO);
}

void RendererOGL::EndDrawing(Scene const* scene) {
	Renderer::EndDrawing(scene);
}

void RendererOGL::BeginOpaque() {

	if(currentScene() && currentScene()->skybox()) {
		Cubemap* c = currentScene()->skybox();

		// Draw skybox
		if(!m_skyboxMesh)
			m_skyboxMesh = A3D::Mesh::standardMesh(A3D::Mesh::CubeIndexedMesh);
		if(!m_skyboxMaterial)
			m_skyboxMaterial = A3D::Material::standardMaterial(A3D::Material::SkyboxMaterial);

		MeshCacheOGL* meshCache = buildMeshCache(m_skyboxMesh);
		MaterialCacheOGL* matCache = buildMaterialCache(m_skyboxMaterial);
		CubemapCacheOGL* ccCache = buildCubemapCache(c);

		m_gl->glDepthMask(GL_FALSE);
		m_gl->glDisable(GL_CULL_FACE);
		matCache->install(m_gl);
		ccCache->applyToSlot(m_gl, static_cast<GLuint>(A3D::MaterialProperties::CubeMapTextureSlot));
		meshCache->render(m_gl, QMatrix4x4(), m_skyboxView, m_skyboxProj);
		m_gl->glEnable(GL_CULL_FACE);
		m_gl->glDepthMask(GL_TRUE);
	}

	m_gl->glEnable(GL_DEPTH_TEST);
	m_gl->glDepthFunc(GL_LESS);
}
void RendererOGL::EndOpaque() {
}

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

void RendererOGL::Delete(CubemapCache* cubemapCache) {
	if(m_context.isNull()) {
		log(LC_Debug, "Couldn't delete CubemapCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete cubemapCache;
}

void RendererOGL::DeleteAllResources() {
	if(m_context.isNull()) {
		log(LC_Debug, "Couldn't delete resources: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	Renderer::runDeleteOnAllResources();

	if(m_sceneUBO) {
		m_gl->glDeleteBuffers(1, &m_sceneUBO);
		m_sceneUBO = 0;
	}
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

CubemapCacheOGL* RendererOGL::buildCubemapCache(Cubemap* cubemap) {
	std::pair<CubemapCacheOGL*, bool> cc = cubemap->getOrEmplaceCubemapCache<CubemapCacheOGL>(rendererID());

	if(cc.first->isDirty())
		cc.first->update(m_gl);

	if(cc.second)
		addToCubemapCaches(cc.first);

	return cc.first;
}

}
