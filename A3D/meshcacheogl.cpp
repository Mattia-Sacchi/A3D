#include "meshcacheogl.h"

namespace A3D {

MeshCacheOGL::MeshCacheOGL(Mesh* parent)
	: MeshCache{parent},
	  m_drawMode(Mesh::Triangles),
	  m_vbo(QOpenGLBuffer::VertexBuffer),
	  m_ibo(QOpenGLBuffer::IndexBuffer),
	  m_elementCount(0)
{}

void MeshCacheOGL::render(CoreGLFunctions* gl) {
	if(!m_elementCount)
		return;
	
	m_vao.bind();
	
	switch(m_drawMode) {
	case Mesh::Triangles:
		gl->glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_elementCount));
		break;
	case Mesh::TriangleStrips:
		gl->glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_elementCount));
		break;
	case Mesh::IndexedTriangles:
		gl->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_elementCount), GL_UNSIGNED_INT, 0);
		break;
	case Mesh::IndexedTriangleStrips:
		gl->glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(m_elementCount), GL_UNSIGNED_INT, 0);
		break;
	}
	
	m_vao.release();
}

void MeshCacheOGL::update(CoreGLFunctions* gl) {
	Mesh* m = mesh();
	if(!m)
		return;
	
	bool isIndexed = (m->drawMode() == Mesh::IndexedTriangles
					  || m->drawMode() == Mesh::IndexedTriangleStrips);
	
	if(isIndexed)
		m_elementCount = m->indices().size();
	else
		m_elementCount = m->vertices().size();
	
	if(!m_elementCount)
		return;
	
	m_drawMode = m->drawMode();
	
	if(!m_vao.isCreated())
		m_vao.create();
	if(!m_vbo.isCreated())
		m_vbo.create();
	if(isIndexed && !m_ibo.isCreated())
		m_ibo.create();
	
	
	m_vao.bind();
	
	{
		std::vector<QVector3D> const& vertices = m->vertices();
		static_assert(sizeof(QVector3D) == sizeof(GLfloat)*3, "Invalid QVector3D size.");
		m_vbo.bind();
		m_vbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(*vertices.data())));
	}
	
	if(isIndexed) {
		// GLuint != uint32_t, but technically they should always be the same.
		// If that is not the case, change the GLxxxx type and update GL_UNSIGNED_INT in render().
		std::vector<GLuint> const& indices = m->indices();
		m_ibo.bind();
		m_ibo.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(*indices.data())));
	}
	
	gl->glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	gl->glEnableVertexAttribArray(0);
	
	m_vao.release();
	m_vbo.release();
	if(isIndexed)
		m_ibo.release();
	
	markClean();
}

}
