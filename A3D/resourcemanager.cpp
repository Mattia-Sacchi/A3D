#include "A3D/resourcemanager.h"
#include "A3D/mesh.h"
#include "A3D/material.h"
#include "A3D/texture.h"

namespace A3D {

ResourceManager::ResourceManager(QObject* parent)
	: QObject{ parent } {}

Mesh* ResourceManager::getLoadedMesh(QString const& name) const {
	auto it = m_meshes.find(name);
	if(it == m_meshes.end() || !it->second)
		return Mesh::standardMesh(Mesh::CubeIndexedMesh);
	return it->second;
}

Material* ResourceManager::getLoadedMaterial(QString const& name) const {
	auto it = m_materials.find(name);
	if(it == m_materials.end() || !it->second)
		return Material::standardMaterial(Material::Basic3DMaterial);
	return it->second;
}

Texture* ResourceManager::getLoadedTexture(QString const& name) const {
	auto it = m_textures.find(name);
	if(it == m_textures.end() || !it->second)
		return Texture::standardTexture(Texture::MissingTexture);
	return it->second;
}

Mesh* ResourceManager::registerMesh(QString name, Mesh* resource) {
	QPointer<Mesh>& m = m_meshes[std::move(name)];
	if(m)
		delete m;
	return m = resource;
}

Material* ResourceManager::registerMaterial(QString name, Material* resource) {
	QPointer<Material>& m = m_materials[std::move(name)];
	if(m)
		delete m;
	return m = resource;
}

Texture* ResourceManager::registerTexture(QString name, Texture* resource) {
	QPointer<Texture>& t = m_textures[std::move(name)];
	if(t)
		delete t;
	return t = resource;
}

QStringList ResourceManager::registeredMeshes() const {
	QStringList sl;
	sl.reserve(m_meshes.size());
	for(auto it = m_meshes.begin(); it != m_meshes.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

QStringList ResourceManager::registeredMaterials() const {
	QStringList sl;
	sl.reserve(m_materials.size());
	for(auto it = m_materials.begin(); it != m_materials.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

QStringList ResourceManager::registeredTextures() const {
	QStringList sl;
	sl.reserve(m_textures.size());
	for(auto it = m_textures.begin(); it != m_textures.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

}
