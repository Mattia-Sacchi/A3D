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
		SmoothingGroupAttribute = 8,
	};

	explicit MeshCacheOGL(Mesh*);
	~MeshCacheOGL();

	void update(CoreGLFunctions*);
	void render(CoreGLFunctions*, QMatrix4x4 const& modelMatrix, QMatrix4x4 const& viewMatrix, QMatrix4x4 const& projMatrix);

private:
	Mesh::DrawMode m_drawMode;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_ibo;
	std::size_t m_elementCount;
	GLenum m_iboFormat;

	struct MeshUBO_Data {
		QMatrix4x4 pMatrix;
		QMatrix4x4 vMatrix;
		QMatrix4x4 mMatrix;

		QMatrix4x4 mvMatrix;
		QMatrix4x4 mvpMatrix;

		QMatrix4x4 mNormalMatrix;
		QMatrix4x4 mvNormalMatrix;
		QMatrix4x4 mvpNormalMatrix;
	} m_meshUBO_data;
	QOpenGLBuffer m_meshUBO;
};

}

#endif // A3DMESHCACHEOGL_H
