#include "A3D/mesh.h"
#include "A3D/renderer.h"
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

		newMesh.vertices().resize(3);
		newMesh.vertices()[0].Position3D = QVector3D(0.0f, 0.5f, 0.f);
		newMesh.vertices()[1].Position3D = QVector3D(-0.5f, -0.5f, 0.f);
		newMesh.vertices()[2].Position3D = QVector3D(0.5f, -0.5f, 0.f);

		newMesh.setContents(Position3D);
		break;
	case CubeIndexedMesh:
		newMesh.setDrawMode(IndexedTriangles);

		newMesh.vertices().resize(8);
		newMesh.vertices()[0].Position3D = QVector3D(-0.5f, -0.5f, +0.5f);
		newMesh.vertices()[1].Position3D = QVector3D(+0.5f, -0.5f, +0.5f);
		newMesh.vertices()[2].Position3D = QVector3D(+0.5f, +0.5f, +0.5f);
		newMesh.vertices()[3].Position3D = QVector3D(-0.5f, +0.5f, +0.5f);
		newMesh.vertices()[4].Position3D = QVector3D(-0.5f, -0.5f, -0.5f);
		newMesh.vertices()[5].Position3D = QVector3D(+0.5f, -0.5f, -0.5f);
		newMesh.vertices()[6].Position3D = QVector3D(+0.5f, +0.5f, -0.5f);
		newMesh.vertices()[7].Position3D = QVector3D(-0.5f, +0.5f, -0.5f);

		newMesh.setContents(Position3D);

		for(std::uint32_t i: { 0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3 })
			newMesh.indices().push_back(i);
		break;
	};
	newMesh.invalidateCache();
	return &newMesh;
}

Mesh::Mesh(QObject* parent)
	: QObject{ parent },
	  m_drawMode(Triangles),
	  m_renderOptions(NoOptions) {
	log(LC_Debug, "Constructor: Mesh");
}

Mesh::~Mesh() {
	log(LC_Debug, "Destructor: Mesh (begin)");
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
	log(LC_Debug, "Destructor: Mesh (end)");
}

Mesh::RenderOptions Mesh::renderOptions() const {
	return m_renderOptions;
}
void Mesh::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

Mesh::Contents Mesh::contents() const {
	return m_contents;
}
void Mesh::setContents(Contents contents) {
	if(m_contents == contents)
		return;

	m_contents = contents;
	m_packedVertices.clear();
	invalidateCache();
}

std::size_t Mesh::packedElementCount() const {
	std::size_t elementCount = 0;
	if(m_contents & Position2D)
		elementCount += 2;
	if(m_contents & Position3D)
		elementCount += 3;
	if(m_contents & TextureCoord2D)
		elementCount += 2;
	if(m_contents & Normal3D)
		elementCount += 3;
	if(m_contents & Color3D)
		elementCount += 3;
	if(m_contents & Color4D)
		elementCount += 4;

	return elementCount;
}

std::vector<float> const& Mesh::packedVertices() const {
	if(m_packedVertices.empty()) {
		std::size_t const elementCount = packedElementCount();

		m_packedVertices.reserve(elementCount * m_vertices.size());
		for(auto it = m_vertices.begin(); it != m_vertices.end(); ++it) {
			Vertex const& v = *it;

			if(m_contents & Position2D)
				pushToContainer(m_packedVertices, v.Position2D);
			if(m_contents & Position3D)
				pushToContainer(m_packedVertices, v.Position3D);
			if(m_contents & TextureCoord2D)
				pushToContainer(m_packedVertices, v.TextureCoord2D);
			if(m_contents & Normal3D)
				pushToContainer(m_packedVertices, v.Normal3D);
			if(m_contents & Color3D)
				pushToContainer(m_packedVertices, v.Color3D);
			if(m_contents & Color4D)
				pushToContainer(m_packedVertices, v.Color4D);
		}
	}
	return m_packedVertices;
}

void Mesh::setDrawMode(DrawMode drawMode) {
	m_drawMode = drawMode;
}
Mesh::DrawMode Mesh::drawMode() const {
	return m_drawMode;
}

std::vector<Mesh::Vertex>& Mesh::vertices() {
	return m_vertices;
}
std::vector<Mesh::Vertex> const& Mesh::vertices() const {
	return m_vertices;
}

std::vector<std::uint32_t>& Mesh::indices() {
	return m_indices;
}
std::vector<std::uint32_t> const& Mesh::indices() const {
	return m_indices;
}

void Mesh::invalidateCache(std::uintptr_t rendererID) {
	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		m_packedVertices.clear();
		for(auto it = m_meshCache.begin(); it != m_meshCache.end();) {
			if(it->second.isNull())
				it = m_meshCache.erase(it);
			else {
				it->second->markDirty();
				++it;
			}
		}
	}
	else {
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
