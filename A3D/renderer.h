#ifndef A3DRENDERER_H
#define A3DRENDERER_H

#include "common.h"
#include <cstdint>
#include "scene.h"
#include "camera.h"

namespace A3D {

class Renderer : public NonCopyable {
protected:
	Renderer();

public:
	struct DrawInfo {
		Scene* m_scene;
		QMatrix4x4 m_modelMatrix;
		QMatrix4x4 m_projMatrix;
		QMatrix4x4 m_viewMatrix;
		QVector3D m_groupPosition;
	};

	virtual ~Renderer();

	std::uintptr_t rendererID() const;

	virtual void Draw(Group*, DrawInfo const&)    = 0;
	virtual void PreLoadEntity(Entity*)           = 0;
	virtual void Delete(MeshCache*)               = 0;
	virtual void Delete(MaterialCache*)           = 0;
	virtual void Delete(MaterialPropertiesCache*) = 0;
	virtual void Delete(TextureCache*)            = 0;
	virtual void Delete(CubemapCache*)            = 0;
	virtual void Delete(LineGroupCache*)          = 0;
	virtual void DeleteAllResources()             = 0;

	void PreLoadEntityTree(Entity*);
	void CleanupRenderCache();
	void DrawAll(Scene* root, Camera const& camera);

protected:
	virtual void BeginDrawing(Camera const&, Scene const*);
	virtual void EndDrawing(Scene const*);

	virtual void BeginOpaque();
	virtual void EndOpaque();
	virtual void BeginTranslucent();
	virtual void EndTranslucent();

	void addToMeshCaches(QPointer<MeshCache>);
	void addToMaterialCaches(QPointer<MaterialCache>);
	void addToMaterialPropertiesCaches(QPointer<MaterialPropertiesCache>);
	void addToTextureCaches(QPointer<TextureCache>);
	void addToCubemapCaches(QPointer<CubemapCache>);
	void addToLineGroupCaches(QPointer<LineGroupCache>);
	void runDeleteOnAllResources();
	void invalidateCache();
	void getClosestSceneLights(QVector3D const& pos, std::size_t desiredLightCount, std::vector<std::pair<std::size_t, PointLightInfo>>& result, Scene const* = nullptr);
	Scene const* currentScene() const;

private:
	void BuildDrawLists(Camera const& camera, Entity* root, QMatrix4x4 const& cascadeMatrix);

	struct GroupBufferData {
		Group* m_group;
		QMatrix4x4 m_transform;
		QVector3D m_position;
		float m_distanceFromCamera;
	};

	static bool OpaqueSorter(GroupBufferData const& a, GroupBufferData const& b);
	static bool TranslucentSorter(GroupBufferData const& a, GroupBufferData const& b);

	std::uintptr_t m_rendererID;
	std::vector<QPointer<MeshCache>> m_meshCaches;
	std::vector<QPointer<MaterialCache>> m_materialCaches;
	std::vector<QPointer<MaterialPropertiesCache>> m_materialPropertiesCaches;
	std::vector<QPointer<TextureCache>> m_textureCaches;
	std::vector<QPointer<CubemapCache>> m_cubemapCaches;
	std::vector<QPointer<LineGroupCache>> m_lineGroupCaches;

	std::vector<GroupBufferData> m_opaqueGroupBuffer;
	std::vector<GroupBufferData> m_translucentGroupBuffer;

	Scene const* m_currentScene;

public:
	static Renderer* getRenderer(std::uintptr_t rendererID);

private:
	static std::uintptr_t createRendererID(Renderer*);
	static void removeRendererID(std::uintptr_t rendererID);
};

}

#endif // A3DRENDERER_H
