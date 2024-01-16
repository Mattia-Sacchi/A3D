#include "A3D/cubemap.h"
#include "A3D/renderer.h"

namespace A3D {

Cubemap::Cubemap(ResourceManager* resourceManager)
	: Resource{ resourceManager } {
	log(LC_Debug, "Constructor: Cubemap");
}
Cubemap::~Cubemap() {
	log(LC_Debug, "Destructor: Cubemap (start)");
	for(auto it = m_cubemapCache.begin(); it != m_cubemapCache.end(); ++it) {
		if(it->second.isNull())
			continue;

		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			log(LC_Info, "Cubemap::~Cubemap: Potential memory leak? Renderer not available.");
			continue;
		}

		r->Delete(it->second);
	}
	log(LC_Debug, "Destructor: Cubemap (end)");
}

Cubemap* Cubemap::clone() const {
	Cubemap* newCubemap = new Cubemap(resourceManager());
	newCubemap->m_nx = m_nx;
	newCubemap->m_ny = m_ny;
	newCubemap->m_nz = m_nz;
	newCubemap->m_px = m_px;
	newCubemap->m_py = m_py;
	newCubemap->m_pz = m_pz;
	return newCubemap;
}

void Cubemap::setNX(QImage const& img) { m_nx = img; }
void Cubemap::setNY(QImage const& img) { m_ny = img; }
void Cubemap::setNZ(QImage const& img) { m_nz = img; }
void Cubemap::setPX(QImage const& img) { m_px = img; }
void Cubemap::setPY(QImage const& img) { m_py = img; }
void Cubemap::setPZ(QImage const& img) { m_pz = img; }

QImage const& Cubemap::nx() const { return m_nx; }
QImage const& Cubemap::ny() const { return m_ny; }
QImage const& Cubemap::nz() const { return m_nz; }
QImage const& Cubemap::px() const { return m_px; }
QImage const& Cubemap::py() const { return m_py; }
QImage const& Cubemap::pz() const { return m_pz; }

bool Cubemap::isValid() const {
	if(m_nx.isNull()
	   || m_ny.isNull()
	   || m_nz.isNull()
	   || m_px.isNull()
	   || m_py.isNull()
	   || m_pz.isNull())
		return false;

	QSize s = nx().size();
	if(s != ny().size()
	   || s != nz().size()
	   || s != px().size()
	   || s != py().size()
	   || s != pz().size()) {
		return false;
	}

	return true;
}

void Cubemap::invalidateCache(std::uintptr_t rendererID) {
	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		for(auto it = m_cubemapCache.begin(); it != m_cubemapCache.end();) {
			if(it->second.isNull())
				it = m_cubemapCache.erase(it);
			else {
				it->second->markDirty();
				++it;
			}
		}
	}
	else {
		auto it = m_cubemapCache.find(rendererID);
		if(it == m_cubemapCache.end())
			return;
		if(it->second.isNull())
			m_cubemapCache.erase(it);
		else
			it->second->markDirty();
	}
}

}
