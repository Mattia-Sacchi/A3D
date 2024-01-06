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
	virtual ~Renderer();

	std::uintptr_t rendererID() const;

	virtual void Draw(Entity* root, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix) = 0;
	virtual void Delete(MeshCache*)                                                                                             = 0;
	virtual void Delete(MaterialCache*)                                                                                         = 0;
	virtual void DeleteAllResources()                                                                                           = 0;

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
	void BuildDrawLists(Entity* root, QMatrix4x4 cascadeMatrix);

	std::uintptr_t m_rendererID;
	std::vector<QPointer<MeshCache>> m_meshCaches;
	std::vector<QPointer<MaterialCache>> m_materialCaches;
	std::vector<QPointer<TextureCache>> m_textureCaches;

	std::vector<std::pair<Entity*, QMatrix4x4>> m_opaqueEntityBuffer;
	std::vector<std::pair<Entity*, QMatrix4x4>> m_translucentEntityBuffer;

public:
	static Renderer* getRenderer(std::uintptr_t rendererID);

private:
	static std::uintptr_t createRendererID(Renderer*);
	static void removeRendererID(std::uintptr_t rendererID);
};

}

#endif // A3DRENDERER_H
