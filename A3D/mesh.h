#ifndef A3DMESH_H
#define A3DMESH_H

#include "A3D/common.h"
#include <QObject>
#include <QVector3D>
#include <vector>
#include <limits>
#include <cstdint>
#include "A3D/meshcache.h"

namespace A3D {

class Mesh : public QObject
{
	Q_OBJECT
public:
	enum StandardMesh {
		Triangle2DMesh,
		CubeIndexedMesh
	};
	static Mesh* standardMesh(StandardMesh);
	
	enum DrawMode {
		Triangles,
		IndexedTriangles,
		TriangleStrips,
		IndexedTriangleStrips
	};
	
	explicit Mesh(QObject *parent = nullptr);
	~Mesh();
	
	void setDrawMode(DrawMode mode);
	DrawMode drawMode() const;
	
	std::vector<QVector3D>& vertices();
	std::vector<QVector3D> const& vertices() const;
	
	std::vector<std::uint32_t>& indices();
	std::vector<std::uint32_t> const& indices() const;
	
	void invalidateMeshCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());
	
	template <typename T>
	T* getMeshCacheT(std::uintptr_t rendererID) const {
		auto it = m_meshCache.find(rendererID);
		if(it == m_meshCache.end() || it->second.isNull())
			return nullptr;
		
		return qobject_cast<T*>(it->second.get());
	}
	template <typename T>
	std::pair<T*, bool> getOrEmplaceMeshCache(std::uintptr_t rendererID) {
		auto it = m_meshCache.find(rendererID);
		if(it == m_meshCache.end() || it->second.isNull()) {
			T* c = new T(this);
			m_meshCache[rendererID] = QPointer<MeshCache>(c);
			return std::make_pair(c, true);
		}
		
		T* c = qobject_cast<T*>(it->second.get());
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for Mesh.");
		
		return std::make_pair(c, false);
	}
	
private:
	DrawMode m_drawMode;
	std::vector<QVector3D> m_vertices;
	std::vector<std::uint32_t> m_indices;
	std::map<std::uintptr_t, QPointer<MeshCache>> m_meshCache;
};

}
#endif // A3DMESH_H
