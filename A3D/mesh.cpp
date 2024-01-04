#include "mesh.h"
#include "renderer.h"
#include <QDebug>

namespace A3D {

Mesh* Mesh::standardMesh(StandardMesh stdMesh) {
	static std::map<StandardMesh, Mesh> standardMeshes;
	
	auto it = standardMeshes.find(stdMesh);
	if(it != standardMeshes.end())
		return &it->second;
	
	Mesh& newMesh = standardMeshes[stdMesh];
	switch(stdMesh) {
	case Triangle2DMesh:
		newMesh.setDrawMode(Triangles);
		newMesh.vertices().push_back(QVector3D( 0.0f,  0.5f, 0.f));
		newMesh.vertices().push_back(QVector3D(-0.5f, -0.5f, 0.f));
		newMesh.vertices().push_back(QVector3D( 0.5f, -0.5f, 0.f));
		break;
	case CubeIndexedMesh:
		newMesh.setDrawMode(IndexedTriangles);
		newMesh.vertices().push_back(QVector3D(-0.5f, -0.5f, +0.5f));
		newMesh.vertices().push_back(QVector3D(+0.5f, -0.5f, +0.5f));
		newMesh.vertices().push_back(QVector3D(+0.5f, +0.5f, +0.5f));
		newMesh.vertices().push_back(QVector3D(-0.5f, +0.5f, +0.5f));
		newMesh.vertices().push_back(QVector3D(-0.5f, -0.5f, -0.5f));
		newMesh.vertices().push_back(QVector3D(+0.5f, -0.5f, -0.5f));
		newMesh.vertices().push_back(QVector3D(+0.5f, +0.5f, -0.5f));
		newMesh.vertices().push_back(QVector3D(-0.5f, +0.5f, -0.5f));
		
		for (std::uint32_t i : {
			 0, 1, 2,
			 2, 3, 0,
			 1, 5, 6,
			 6, 2, 1,
			 7, 6, 5,
			 5, 4, 7,
			 4, 0, 3,
			 3, 7, 4,
			 4, 5, 1,
			 1, 0, 4,
			 3, 2, 6,
			 6, 7, 3
		})
			newMesh.indices().push_back(i);
	};
	newMesh.invalidateMeshCache();
	return &newMesh;
}

Mesh::Mesh(QObject *parent)
	: QObject{parent},
	  m_drawMode(Triangles)
{
	dbgConstruct("Mesh")
}

Mesh::~Mesh() {
	dbgDestruct("Mesh started")
	for(auto it = m_meshCache.begin(); it != m_meshCache.end(); ++it) {
		if(it->second.isNull())
			continue;
		
		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			qDebug() << "Mesh::~Mesh: Potential memory leak? Renderer not available.";
			continue;
		}
		
		r->Delete(it->second);
	}
	dbgDestruct("Mesh finished")
}

void Mesh::setDrawMode(DrawMode drawMode) {
	m_drawMode = drawMode;
}
Mesh::DrawMode Mesh::drawMode() const {
	return m_drawMode;
}

std::vector<QVector3D>& Mesh::vertices() {
	return m_vertices;
}
std::vector<QVector3D> const& Mesh::vertices() const {
	return m_vertices;
}

std::vector<std::uint32_t>& Mesh::indices() {
	return m_indices;
}
std::vector<std::uint32_t> const& Mesh::indices() const {
	return m_indices;
}

void Mesh::invalidateMeshCache(std::uintptr_t rendererID) {
	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		for(auto it = m_meshCache.begin(); it != m_meshCache.end();) {
			if(it->second.isNull())
				it = m_meshCache.erase(it);
			else {
				it->second->markDirty();
				++it;
			}
		}
	} else {
		auto it = m_meshCache.find(rendererID);
		if(it == m_meshCache.end())
			return;
		if(it->second.isNull())
			m_meshCache.erase(it);
		else
			it->second->markDirty();
	}
}

}
