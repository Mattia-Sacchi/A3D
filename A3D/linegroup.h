#ifndef A3DLINEGROUP_H
#define A3DLINEGROUP_H

#include "common.h"
#include "linegroupcache.h"
#include "resource.h"

namespace A3D {

class LineGroup : public Resource {
	Q_OBJECT
public:
	enum Content {
		Position2D = 0x0001,
		Position3D = 0x0002,
		Color3D    = 0x0004,
		Color4D    = 0x0008,
	};
	Q_DECLARE_FLAGS(Contents, Content)

	enum DrawMode {
		Lines,
		IndexedLines,
		LineStrips,
		IndexedLineStrips,
	};

	struct Vertex {
		QVector2D Position2D;
		QVector3D Position3D;
		QVector3D Color3D;
		QVector4D Color4D;

		bool Equals(Vertex const& o, Contents c) const;
	};

	explicit LineGroup(ResourceManager* = nullptr);
	~LineGroup();

	LineGroup* clone() const;

	void setThickness(float value);
	float thickness() const;

	void setContents(Contents);
	Contents contents() const;

	void setDrawMode(DrawMode mode);
	DrawMode drawMode() const;

	void optimizeIndices();

	std::vector<Vertex>& vertices();
	std::vector<Vertex> const& vertices() const;
	std::vector<std::uint8_t> const& packedData() const;

	std::vector<std::uint32_t>& indices();
	std::vector<std::uint32_t> const& indices() const;

	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	template <typename T>
	T* getLineGroupCacheT(std::uintptr_t rendererID) const {
		auto it = m_lineGroupCache.find(rendererID);
		if(it == m_lineGroupCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}
	template <typename T>
	std::pair<T*, bool> getOrEmplaceLineGroupCache(std::uintptr_t rendererID) {
		auto it = m_lineGroupCache.find(rendererID);
		if(it == m_lineGroupCache.end() || it->second.isNull()) {
			T* c                         = new T(this);
			m_lineGroupCache[rendererID] = QPointer<LineGroupCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for LineGroup.");

		return std::make_pair(c, false);
	}

	static std::size_t packedVertexSize(Contents);

private:
	DrawMode m_drawMode;
	std::vector<Vertex> m_vertices;
	std::vector<std::uint32_t> m_indices;
	float m_thickness;

	Contents m_contents;
	mutable std::vector<std::uint8_t> m_packedData;
	std::map<std::uintptr_t, QPointer<LineGroupCache>> m_lineGroupCache;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(LineGroup::Contents)

}

#endif // A3DLINEGROUP_H
