#ifndef A3DMESHCACHEOGL_H
#define A3DMESHCACHEOGL_H

#include "A3D/common.h"
#include "A3D/meshcache.h"
#include "A3D/mesh.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

namespace A3D {

class MeshCacheOGL : public MeshCache
{
	Q_OBJECT
public:
	MeshCacheOGL(Mesh* parent);
	
	void update(CoreGLFunctions*);
	void render(CoreGLFunctions*);
	
private:
	Mesh::DrawMode m_drawMode;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_ibo;
	std::size_t m_elementCount;
};

}

#endif // A3DMESHCACHEOGL_H
