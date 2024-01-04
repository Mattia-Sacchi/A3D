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
	enum RenderOption {
		NoOptions = 0x0,
		
		// Disable back-face culling for this entity
		DisableCulling = 0x1
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)
	
	enum Content {
		Position2D     = 0x0001,
		Position3D     = 0x0002,
		TextureCoord2D = 0x0004,
		Normal3D       = 0x0008,
		Color3D        = 0x0010,
		Color4D        = 0x0020,
	};
	Q_DECLARE_FLAGS(Contents, Content)
	
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
	
	struct Vertex {
		QVector2D Position2D;
		QVector3D Position3D;
		QVector2D TextureCoord2D;
		QVector3D Normal3D;
		QVector3D Color3D;
		QVector4D Color4D;
	};
	
	explicit Mesh(QObject *parent = nullptr);
	~Mesh();
	
	RenderOptions renderOptions() const;
	void setRenderOptions(RenderOptions);
	
	Contents contents() const;
	void setContents(Contents);
	
	void setDrawMode(DrawMode mode);
	DrawMode drawMode() const;

	std::vector<Vertex>& vertices();
	std::vector<Vertex> const& vertices() const;
	
	std::vector<float> const& packedVertices() const;
	
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
	
	std::size_t packedElementCount() const;
	
private:
	DrawMode m_drawMode;
	std::vector<Vertex> m_vertices;
	std::vector<std::uint32_t> m_indices;
	RenderOptions m_renderOptions;
	
	Contents m_contents;
	mutable std::vector<float> m_packedVertices;
	std::map<std::uintptr_t, QPointer<MeshCache>> m_meshCache;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Mesh::RenderOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(Mesh::Contents)

}
#endif // A3DMESH_H
