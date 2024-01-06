#ifndef A3DENTITY_H
#define A3DENTITY_H

#include "A3D/common.h"
#include <QObject>
#include <QPointer>
#include <QMatrix4x4>
#include <vector>
#include "A3D/mesh.h"
#include "A3D/material.h"
#include "A3D/texture.h"
#include "A3D/materialproperties.h"

namespace A3D {

class Entity : public QObject {
	Q_OBJECT
public:
	enum {
		MaxTextures = 8,
	};

	enum RenderOption {
		NoOptions = 0x0,

		// Hide this object and all connected children
		Hidden = 0x1,
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	explicit Entity(Entity* parent);
	~Entity();

	RenderOptions renderOptions() const;
	void setRenderOptions(RenderOptions);

	Entity* parentEntity() const;

	// Retrieve a list of Entities.
	// Some of these might be nullptrs.
	std::vector<QPointer<Entity>> const& childrenEntities() const;

	// Constructs a new child Entity.
	// To remove the child Entity, delete the pointer.
	template <typename T, typename... Args>
	T* emplaceChildEntity(Args&&... args) {
		T* p = new T(std::forward<Args>(args)..., this);
		addChildEntity(p);
		return p;
	}

	void setPosition(QVector3D const&);
	QVector3D position() const;

	void setRotation(QQuaternion const&);
	QQuaternion rotation() const;

	void setScale(QVector3D const& = QVector3D(1.f, 1.f, 1.f));
	QVector3D scale() const;

	QMatrix4x4 const& modelMatrix() const;

	Mesh* mesh() const;
	Texture* texture(std::size_t slot = 0) const;
	Material* material() const;
	MaterialProperties& materialProperties();
	MaterialProperties const& materialProperties() const;

	void setMesh(Mesh*);
	void setTexture(Texture*, std::size_t slot = 0);
	void setMaterial(Material*);
	void setMaterialProperties(MaterialProperties);

private:
	// Adds a child Entity
	void addChildEntity(Entity*);

	QPointer<Entity> m_parent;
	std::vector<QPointer<Entity>> m_entities;
	RenderOptions m_renderOptions;

	QVector3D m_position;
	QQuaternion m_rotation;
	QVector3D m_scale;
	mutable bool m_matrixDirty;
	mutable QMatrix4x4 m_matrix;

	Mesh* m_mesh;
	Material* m_material;
	Texture* m_textureSet[8];
	MaterialProperties m_materialProperties;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Entity::RenderOptions)

}

#endif // A3DENTITY_H
