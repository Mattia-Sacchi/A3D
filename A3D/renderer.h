#ifndef A3DRENDERER_H
#define A3DRENDERER_H

#include "A3D/common.h"
#include <cstdint>
#include "A3D/scene.h"

namespace A3D {

class Renderer : public NonCopyable
{
protected:
	Renderer();
	
public:
	virtual ~Renderer();
	
	std::uintptr_t rendererID() const;
	
	virtual void Draw(CoreGLFunctions*, Entity* root, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix) =0;
	virtual void Delete(MeshCache*) =0;
	virtual void Delete(MaterialCache*) =0;
	virtual void DeleteAllResources() =0;
	void CleanupRenderCache();
	
protected:
	void addToMeshCaches(QPointer<MeshCache>);
	void addToMaterialCaches(QPointer<MaterialCache>);
	void runDeleteOnAllResources();
	
private:
	std::uintptr_t m_rendererID;
	std::vector<QPointer<MeshCache>> m_meshCaches;
	std::vector<QPointer<MaterialCache>> m_materialCaches;
	
public:
	static Renderer* getRenderer(std::uintptr_t rendererID);
	
private:
	static std::uintptr_t createRendererID(Renderer*);
	static void removeRendererID(std::uintptr_t rendererID);
};

}

#endif // A3DRENDERER_H
