#ifndef A3DMESHCACHEOGL_H
#define A3DMESHCACHEOGL_H

#include "common.h"
#include "meshcache.h"
#include "mesh.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

namespace A3D {
class RendererOGL;
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

	void update(RendererOGL*, CoreGLFunctions*);
	void render(RendererOGL*, CoreGLFunctions*, QMatrix4x4 const& modelMatrix, QMatrix4x4 const& viewMatrix, QMatrix4x4 const& projMatrix);

private:
	Mesh::DrawMode m_drawMode;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_ibo;
	std::size_t m_elementCount;
	GLenum m_iboFormat;

	struct RawMatrix4x4 {
		inline RawMatrix4x4()
			: RawMatrix4x4(QMatrix4x4()) {}
		inline RawMatrix4x4(QMatrix4x4 const& m) { *this = m; }
		inline RawMatrix4x4& operator=(QMatrix4x4 const& m) {
			std::memcpy(data, m.data(), sizeof(data));
			return *this;
		}
		inline bool operator==(QMatrix4x4 const& o) const { return std::memcmp(data, o.data(), sizeof(data)) == 0; }
		inline bool operator!=(QMatrix4x4 const& o) const { return !(*this == o); }
		float data[16];
	};
	struct MeshUBO_Data {
		RawMatrix4x4 pMatrix;
		RawMatrix4x4 vMatrix;
		RawMatrix4x4 mMatrix;

		RawMatrix4x4 mvMatrix;
		RawMatrix4x4 mvpMatrix;

		RawMatrix4x4 mNormalMatrix;
		RawMatrix4x4 mvNormalMatrix;
		RawMatrix4x4 mvpNormalMatrix;
	} m_meshUBO_data;
	GLuint m_meshUBO;
};

}

#endif // A3DMESHCACHEOGL_H
