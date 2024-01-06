#ifndef A3DRENDEREROGL_H
#define A3DRENDEREROGL_H

#include "A3D/common.h"
#include "A3D/renderer.h"
#include "A3D/meshcacheogl.h"
#include "A3D/materialcacheogl.h"
#include "A3D/texturecacheogl.h"
#include <queue>

namespace A3D {

class RendererOGL final : public Renderer {
public:
	RendererOGL(QOpenGLContext*, CoreGLFunctions*);
	~RendererOGL();

	virtual void Draw(Entity* root, QMatrix4x4 const& calcMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix) override;

	virtual void PreLoadEntity(Entity*) override;
	virtual void Delete(MeshCache*) override;
	virtual void Delete(MaterialCache*) override;
	virtual void Delete(TextureCache*) override;
	virtual void DeleteAllResources() override;

protected:
	virtual void BeginOpaque() override;
	virtual void EndOpaque() override;
	virtual void BeginTranslucent() override;
	virtual void EndTranslucent() override;

private:
	void DrawOpaque(Entity* root, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix, std::deque<Entity*>* translucentList);
	void DrawTranslucent(std::deque<Entity*>& entities, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix);

	MeshCacheOGL* buildMeshCache(Mesh*);
	MaterialCacheOGL* buildMaterialCache(Material*);
	TextureCacheOGL* buildTextureCache(Texture*);

	QPointer<QOpenGLContext> m_context;
	CoreGLFunctions* m_gl;
	std::vector<Entity*> m_translucentEntityBuffer;
};

}

#endif // A3DRENDEREROGL_H
