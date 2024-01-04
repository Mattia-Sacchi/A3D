#include "materialcache.h"
#include "material.h"

namespace A3D {

MaterialCache::MaterialCache(Material *parent)
	: QObject{parent},
	  m_material(parent),
	  m_isDirty(true)
{
	dbgConstruct("MaterialCache")
}
MaterialCache::~MaterialCache() {
	dbgDestruct("MaterialCache")
}

Material* MaterialCache::material() const {
	return m_material.get();
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
