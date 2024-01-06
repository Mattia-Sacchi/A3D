#ifndef A3DRENDERER_H
#define A3DRENDERER_H

#include "A3D/common.h"
#include <cstdint>
#include "A3D/scene.h"
#include "A3D/camera.h"

namespace A3D {

class Renderer : public NonCopyable {
protected:
	Renderer();

public:
	struct DrawInfo {
		MaterialProperties m_calculatedMaterialProperties;
		QMatrix4x4 m_calculatedMatrix;
		QMatrix4x4 m_projMatrix;
		QMatrix4x4 m_viewMatrix;
	};

	virtual ~Renderer();

	std::uintptr_t rendererID() const;

	virtual void Draw(Group*, DrawInfo const&) = 0;
	virtual void PreLoadEntity(Entity*)        = 0;
	virtual void Delete(MeshCache*)            = 0;
	virtual void Delete(MaterialCache*)        = 0;
	virtual void Delete(TextureCache*)         = 0;
	virtual void DeleteAllResources()          = 0;

	void PreLoadEntityTree(Entity*);
	void CleanupRenderCache();
	void DrawAll(Entity* root, Camera const& camera);

protected:
	virtual void BeginOpaque();
	virtual void EndOpaque();
	virtual void BeginTranslucent();
	virtual void EndTranslucent();

	void addToMeshCaches(QPointer<MeshCache>);
	void addToMaterialCaches(QPointer<MaterialCache>);
	void addToTextureCaches(QPointer<TextureCache>);
	void runDeleteOnAllResources();
	void invalidateCache();

private:
	void BuildDrawLists(Camera const& camera, Entity* root, QMatrix4x4 const& cascadeMatrix);

	struct GroupBufferData {
		Group* m_group;
		QMatrix4x4 m_transform;
		MaterialProperties m_materialProperties;
		float m_distanceFromCamera;
	};

	static bool OpaqueSorter(GroupBufferData const& a, GroupBufferData const& b);
	static bool TranslucentSorter(GroupBufferData const& a, GroupBufferData const& b);

	std::uintptr_t m_rendererID;
	std::vector<QPointer<MeshCache>> m_meshCaches;
	std::vector<QPointer<MaterialCache>> m_materialCaches;
	std::vector<QPointer<TextureCache>> m_textureCaches;

	std::vector<GroupBufferData> m_opaqueGroupBuffer;
	std::vector<GroupBufferData> m_translucentGroupBuffer;

public:
	static Renderer* getRenderer(std::uintptr_t rendererID);

private:
	static std::uintptr_t createRendererID(Renderer*);
	static void removeRendererID(std::uintptr_t rendererID);
};

}

#endif // A3DRENDERER_H
