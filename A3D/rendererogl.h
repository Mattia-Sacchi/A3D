#ifndef A3DRENDEREROGL_H
#define A3DRENDEREROGL_H

#include "A3D/common.h"
#include "A3D/renderer.h"
#include "A3D/meshcacheogl.h"
#include "A3D/materialcacheogl.h"
#include <queue>

namespace A3D {

class RendererOGL final : public Renderer
{
public:
	RendererOGL(QOpenGLContext*);
	~RendererOGL();
	
	virtual void Draw(CoreGLFunctions*, Entity* root, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix) override;
	virtual void Delete(MeshCache*) override;
	virtual void Delete(MaterialCache*) override;
	virtual void DeleteAllResources() override;
	
private:
	MeshCacheOGL* buildMeshCache(CoreGLFunctions*, Mesh*);
	MaterialCacheOGL* buildMaterialCache(CoreGLFunctions*, Material*);
	
	QPointer<QOpenGLContext> m_context;
};

}

#endif // A3DRENDEREROGL_H
