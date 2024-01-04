#ifndef A3DENTITY_H
#define A3DENTITY_H

#include "A3D/common.h"
#include <QObject>
#include <QPointer>
#include <QMatrix4x4>
#include <vector>
#include "A3D/mesh.h"
#include "A3D/material.h"

namespace A3D {

class Entity : public QObject
{
	Q_OBJECT
public:
	explicit Entity(Entity *parent);
	~Entity();
	
	Entity* parentEntity() const;
	
	// Retrieve a list of Entities.
	// Some of these might be nullptrs.
	std::vector<QPointer<Entity>> const& childrenEntities() const;
	
	// Constructs a new child Entity.
	// To remove the child Entity, delete the pointer.
	template <typename T, typename ...Args>
	T* emplaceChildEntity(Args&& ...args) {
		T* p = new T(std::forward<Args>(args)..., this);
		addChildEntity(p);
		return p;
	}
	
	QMatrix4x4 const& modelMatrix() const;
	QMatrix4x4& modelMatrix();
	void setModelMatrix(QMatrix4x4 const&);
	
	Mesh* mesh() const;
	Material* material() const;
	
	void setMesh(Mesh*);
	void setMaterial(Material*);
	
private:
	// Adds a child Entity
	void addChildEntity(Entity*);
	
	QPointer<Entity> m_parent;
	std::vector<QPointer<Entity>> m_entities;
	
	QMatrix4x4 m_matrix;
	Mesh* m_mesh;
	Material* m_material;
};

}

#endif // A3DENTITY_H
