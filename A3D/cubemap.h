#ifndef A3DCUBEMAP_H
#define A3DCUBEMAP_H

#include "common.h"
#include <QObject>
#include "texture.h"
#include "cubemapcache.h"
#include "resource.h"
#include "image.h"

namespace A3D {

class Cubemap : public Resource {
	Q_OBJECT
public:
	explicit Cubemap(ResourceManager* = nullptr);
	~Cubemap();

	Cubemap* clone() const;

	void setNX(Image const&);
	void setNY(Image const&);
	void setNZ(Image const&);
	void setPX(Image const&);
	void setPY(Image const&);
	void setPZ(Image const&);

	Image const& nx() const;
	Image const& ny() const;
	Image const& nz() const;
	Image const& px() const;
	Image const& py() const;
	Image const& pz() const;

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
	Image m_nx;
	Image m_ny;
	Image m_nz;
	Image m_px;
	Image m_py;
	Image m_pz;

	std::map<std::uintptr_t, QPointer<CubemapCache>> m_cubemapCache;
};

}

#endif // A3DCUBEMAP_H
