#ifndef A3DMESHCACHEOGL_H
#define A3DMESHCACHEOGL_H

#include "A3D/common.h"
#include "A3D/meshcache.h"
#include "A3D/mesh.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

namespace A3D {

class MeshCacheOGL : public MeshCache {
	Q_OBJECT
public:
	enum {
		Position3DAttribute     = 0,
		Position2DAttribute     = 1,
		TextureCoord2DAttribute = 2,
		Normal3DAttribute       = 3,
		Color3DAttribute        = 4,
		Color4DAttribute        = 5,
		BoneIDAttribute         = 6,
		BoneWeightsAttribute    = 7,
	};

	explicit MeshCacheOGL(Mesh*);
	~MeshCacheOGL();

	void update(CoreGLFunctions*);
	void render(CoreGLFunctions*);

private:
	Mesh::DrawMode m_drawMode;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_ibo;
	std::size_t m_elementCount;
	GLenum m_iboFormat;
};

}

#endif // A3DMESHCACHEOGL_H
