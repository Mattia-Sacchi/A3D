#ifndef A3DRESOURCEMANAGER_H
#define A3DRESOURCEMANAGER_H

#include "A3D/common.h"
#include <QObject>

namespace A3D {

class Texture;
class Mesh;
class Material;
class Model;

class ResourceManager : public QObject {
	Q_OBJECT
public:
	enum InputFormat {
		IF_AutoDetect,
		IF_OBJ,
	};
	explicit ResourceManager(QObject* parent = nullptr);

	Model* getLoadedModel(QString const& name) const;
	Mesh* getLoadedMesh(QString const& name) const;
	Material* getLoadedMaterial(QString const& name) const;
	Texture* getLoadedTexture(QString const& name) const;

	Model* registerModel(QString name, Model* model);
	Mesh* registerMesh(QString name, Mesh* resource);
	Material* registerMaterial(QString name, Material* resource);
	Texture* registerTexture(QString name, Texture* resource);

	QStringList registeredModels() const;
	QStringList registeredMeshes() const;
	QStringList registeredMaterials() const;
	QStringList registeredTextures() const;

	Model* loadModel(QString name, QString const& path, InputFormat fmt = IF_AutoDetect);

private:
	struct OpenFileResult {
		std::unique_ptr<QIODevice> stream;
		QString name;
		QString uri;
		QString basePath;
	};
	OpenFileResult openFile(QString name, QString const& path) const;
	OpenFileResult openFile(OpenFileResult const& parent, QString const& relativePath) const;
	QString locateFile(OpenFileResult const& parent, QString const& relativePath) const;

	Model* loadModel_OBJ(OpenFileResult);

	std::map<QString, QPointer<Model>> m_models;
	std::map<QString, QPointer<Mesh>> m_meshes;
	std::map<QString, QPointer<Material>> m_materials;
	std::map<QString, QPointer<Texture>> m_textures;
};

}

#endif // A3DRESOURCEMANAGER_H
