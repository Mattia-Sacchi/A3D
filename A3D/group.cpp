#include "A3D/group.h"
#include "A3D/model.h"

namespace A3D {

Group::Group(Model* model)
	: QObject{ model },
	  m_renderOptions(NoOptions),
	  m_model(model),
	  m_matrixDirty(true),
	  m_scale(1.f, 1.f, 1.f) {
	log(LC_Debug, "Constructor: Group");
}

Group::~Group() {
	log(LC_Debug, "Destructor: Group");
}

Group* Group::clone(Model* m, bool deepClone) const {
	Group* newGroup         = new Group(m);
	newGroup->m_matrixDirty = m_matrixDirty;
	if(!m_matrixDirty)
		newGroup->m_matrix = m_matrix;
	newGroup->m_position           = m_position;
	newGroup->m_rotation           = m_rotation;
	newGroup->m_scale              = m_scale;
	newGroup->m_materialProperties = m_materialProperties;

	if(deepClone) {
		// ...
		if(m_mesh)
			newGroup->m_mesh = m_mesh->clone();
		if(m_material)
			newGroup->m_material = m_material->clone();
		for(std::size_t i = 0; i < MaxTextures; ++i) {
			if(m_textures[i])
				newGroup->m_textures[i] = m_textures[i]->clone();
		}
	}
	else {
		newGroup->m_mesh     = m_mesh;
		newGroup->m_material = m_material;
		for(std::size_t i = 0; i < MaxTextures; ++i) {
			newGroup->m_textures[i] = m_textures[i];
		}
	}

	return newGroup;
}

Group::RenderOptions Group::renderOptions() const {
	return m_renderOptions;
}
void Group::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

Model* Group::model() const {
	return m_model;
}

void Group::setPosition(QVector3D const& pos) {
	if(m_position == pos)
		return;
	m_position    = pos;
	m_matrixDirty = true;
}
QVector3D Group::position() const {
	return m_position;
}

void Group::setRotation(QQuaternion const& rot) {
	if(m_rotation == rot)
		return;
	m_rotation    = rot;
	m_matrixDirty = true;
}
QQuaternion Group::rotation() const {
	return m_rotation;
}

void Group::setScale(QVector3D const& scale) {
	if(m_scale == scale)
		return;
	m_scale       = scale;
	m_matrixDirty = true;
}
QVector3D Group::scale() const {
	return m_scale;
}

QMatrix4x4 const& Group::groupMatrix() const {
	if(m_matrixDirty) {
		m_matrixDirty = false;
		m_matrix.setToIdentity();
		m_matrix.rotate(m_rotation);
		m_matrix.scale(m_scale);
		m_matrix.translate(m_position);
	}
	return m_matrix;
}

Mesh* Group::mesh() const {
	return m_mesh;
}
Texture* Group::texture(std::size_t slot) const {
	if(slot >= MaxTextures)
		return nullptr;
	return m_textures[slot];
}
Material* Group::material() const {
	return m_material;
}
MaterialProperties& Group::materialProperties() {
	return m_materialProperties;
}
MaterialProperties const& Group::materialProperties() const {
	return m_materialProperties;
}

void Group::setMesh(Mesh* mesh) {
	if(mesh == m_mesh)
		return;
	if(m_mesh && m_mesh->parent() == this)
		delete m_mesh;
	m_mesh = mesh;
}

void Group::setTexture(Texture* texture, std::size_t slot) {
	if(slot >= MaxTextures)
		return;
	if(texture == m_textures[slot])
		return;
	if(m_textures[slot] && m_textures[slot]->parent() == this)
		delete m_textures[slot];
	m_textures[slot] = texture;
}

void Group::setMaterial(Material* material) {
	if(material == m_material)
		return;
	if(m_material && m_material->parent() == this)
		delete m_material;
	m_material = material;
}

void Group::setMaterialProperties(MaterialProperties properties) {
	m_materialProperties = std::move(properties);
}

}
