#ifndef A3DRENDEREROGL_H
#define A3DRENDEREROGL_H

#include "A3D/common.h"
#include "A3D/renderer.h"
#include "A3D/meshcacheogl.h"
#include "A3D/materialcacheogl.h"
#include "A3D/materialpropertiescacheogl.h"
#include "A3D/texturecacheogl.h"
#include "A3D/cubemapcacheogl.h"
#include <queue>

namespace A3D {

class RendererOGL final : public Renderer {
public:
	enum UBOBindings {
		UBO_MeshBinding               = 0,
		UBO_MaterialPropertiesBinding = 1,
		UBO_SceneBinding              = 2,
	};

	RendererOGL(QOpenGLContext*, CoreGLFunctions*);
	~RendererOGL();

	virtual void Draw(Group*, DrawInfo const&) override;
	virtual void PreLoadEntity(Entity*) override;
	virtual void Delete(MeshCache*) override;
	virtual void Delete(MaterialCache*) override;
	virtual void Delete(MaterialPropertiesCache*) override;
	virtual void Delete(TextureCache*) override;
	virtual void Delete(CubemapCache*) override;
	virtual void DeleteAllResources() override;

protected:
	virtual void BeginDrawing(Camera const&, Scene const*) override;
	virtual void EndDrawing(Scene const*) override;

	virtual void BeginOpaque() override;
	virtual void EndOpaque() override;
	virtual void BeginTranslucent() override;
	virtual void EndTranslucent() override;

private:
	enum {
		LightCount = 4
	};

	void RefreshSceneUBO();
	void DrawOpaque(Entity* root, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix, std::deque<Entity*>* translucentList);
	void DrawTranslucent(std::deque<Entity*>& entities, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix);

	MeshCacheOGL* buildMeshCache(Mesh*);
	MaterialCacheOGL* buildMaterialCache(Material*);
	MaterialPropertiesCacheOGL* buildMaterialPropertiesCache(MaterialProperties*);
	TextureCacheOGL* buildTextureCache(Texture*);
	CubemapCacheOGL* buildCubemapCache(Cubemap*);

	QPointer<QOpenGLContext> m_context;
	CoreGLFunctions* m_gl;
	std::vector<Entity*> m_translucentEntityBuffer;
	std::vector<std::pair<std::size_t, PointLightInfo>> m_closestSceneLightsBuffer;

	// Skybox data
	A3D::Material* m_skyboxMaterial;
	A3D::Mesh* m_skyboxMesh;
	QMatrix4x4 m_skyboxView;
	QMatrix4x4 m_skyboxProj;

	struct SceneUBO_Data {
		QVector4D m_cameraPos;

		QVector4D m_lightPos[4];
		QVector4D m_lightColor[4];
	};
	SceneUBO_Data m_sceneData;
	GLuint m_sceneUBO;
};

}

#endif // A3DRENDEREROGL_H
