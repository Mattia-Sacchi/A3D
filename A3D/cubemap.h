#ifndef A3DCUBEMAP_H
#define A3DCUBEMAP_H

#include "A3D/common.h"
#include <QObject>
#include <QImage>
#include "A3D/texture.h"
#include "A3D/cubemapcache.h"
#include "A3D/resource.h"

namespace A3D {

class Cubemap : public Resource {
	Q_OBJECT
public:
	explicit Cubemap(ResourceManager* = nullptr);
	~Cubemap();

	Cubemap* clone() const;

	void setNX(QImage const&);
	void setNY(QImage const&);
	void setNZ(QImage const&);
	void setPX(QImage const&);
	void setPY(QImage const&);
	void setPZ(QImage const&);

	QImage const& nx() const;
	QImage const& ny() const;
	QImage const& nz() const;
	QImage const& px() const;
	QImage const& py() const;
	QImage const& pz() const;

	bool isValid() const;

	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	template <typename T>
	T* getCubemapCacheT(std::uintptr_t rendererID) const {
		auto it = m_cubemapCache.find(rendererID);
		if(it == m_cubemapCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}
	template <typename T>
	std::pair<T*, bool> getOrEmplaceCubemapCache(std::uintptr_t rendererID) {
		auto it = m_cubemapCache.find(rendererID);
		if(it == m_cubemapCache.end() || it->second.isNull()) {
			T* c                       = new T(this);
			m_cubemapCache[rendererID] = QPointer<CubemapCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for Cubemap.");

		return std::make_pair(c, false);
	}

private:
	QImage m_nx;
	QImage m_ny;
	QImage m_nz;
	QImage m_px;
	QImage m_py;
	QImage m_pz;

	std::map<std::uintptr_t, QPointer<CubemapCache>> m_cubemapCache;
};

}

#endif // A3DCUBEMAP_H
