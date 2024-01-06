#include "A3D/materialcache.h"
#include "A3D/material.h"

namespace A3D {

MaterialCache::MaterialCache(Material* parent)
	: QObject{ parent },
	  m_material(parent),
	  m_isDirty(true) {
	log(LC_Debug, "Constructor: MaterialCache");
}
MaterialCache::~MaterialCache() {
	log(LC_Debug, "Destructor: MaterialCache");
}

Material* MaterialCache::material() const {
	return m_material;
}

void MaterialCache::markDirty() {
	m_isDirty = true;
}
void MaterialCache::markClean() {
	m_isDirty = false;
}
bool MaterialCache::isDirty() const {
	return m_isDirty;
}

}
