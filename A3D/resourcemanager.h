#ifndef A3DRESOURCEMANAGER_H
#define A3DRESOURCEMANAGER_H

#include "A3D/common.h"
#include <QObject>

namespace A3D {

class Texture;
class Mesh;
class Material;

class ResourceManager : public QObject {
	Q_OBJECT
public:
	explicit ResourceManager(QObject* parent = nullptr);

	Mesh* getLoadedMesh(QString const& name) const;
	Material* getLoadedMaterial(QString const& name) const;
	Texture* getLoadedTexture(QString const& name) const;

	Mesh* registerMesh(QString name, Mesh* resource);
	Material* registerMaterial(QString name, Material* resource);
	Texture* registerTexture(QString name, Texture* resource);

	QStringList registeredMeshes() const;
	QStringList registeredMaterials() const;
	QStringList registeredTextures() const;

	// Mesh* loadMesh(QString name, QString const& path);
	// Material* loadMaterial(QString name, QString const& path);
	// Texture* loadTexture(QString name, QString const& path);

private:
	std::map<QString, QPointer<Mesh>> m_meshes;
	std::map<QString, QPointer<Material>> m_materials;
	std::map<QString, QPointer<Texture>> m_textures;
};

}

#endif // A3DRESOURCEMANAGER_H
