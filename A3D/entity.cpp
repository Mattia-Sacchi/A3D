#include "entity.h"

namespace A3D {

Entity::Entity(Entity *parent)
	: QObject{parent},
	  m_parent(parent),
	  m_mesh(nullptr),
	  m_material(nullptr)
{
	dbgConstruct("Entity")
}

Entity::~Entity() {
	dbgDestruct("Entity")
}

Entity* Entity::parentEntity() const {
	return m_parent.get();
}

std::vector<QPointer<Entity>> const& Entity::childrenEntities() const {
	return m_entities;
}

void Entity::addChildEntity(Entity* entity) {
	cleanupQPointers(m_entities);
	m_entities.emplace_back(entity);
}

QMatrix4x4 const& Entity::modelMatrix() const {
	return m_matrix;
}

QMatrix4x4& Entity::modelMatrix() {
	return m_matrix;
}

void Entity::setModelMatrix(QMatrix4x4 const& matrix) {
	m_matrix = matrix;
}

Mesh* Entity::mesh() const {
	return m_mesh;
}

Material* Entity::material() const {
	return m_material;
}

void Entity::setMesh(Mesh* mesh) {
	if(m_mesh && m_mesh->parent() == this)
		delete m_mesh;
	m_mesh = mesh;
}

void Entity::setMaterial(Material* material) {
	if(m_material && m_material->parent() == this)
		delete m_material;
	m_material = material;
}

}
