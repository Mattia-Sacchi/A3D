#include "A3D/cubemapcache.h"
#include "A3D/cubemap.h"

namespace A3D {

CubemapCache::CubemapCache(Cubemap* parent)
	: QObject{ parent },
	  m_cubemap(parent),
	  m_isDirty(true) {
	log(LC_Debug, "Constructor: CubemapCache");
}
CubemapCache::~CubemapCache() {
	log(LC_Debug, "Destructor: CubemapCache");
}

Cubemap* CubemapCache::cubemap() const {
	return m_cubemap;
}

void CubemapCache::markDirty() {
	m_isDirty = true;
}

void CubemapCache::markClean() {
	m_isDirty = false;
}

bool CubemapCache::isDirty() const {
	return m_isDirty;
}

}
