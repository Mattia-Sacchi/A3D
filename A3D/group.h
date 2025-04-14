#ifndef A3DGROUP_H
#define A3DGROUP_H

#include "common.h"
#include "mesh.h"
#include "material.h"
#include "materialproperties.h"
#include "texture.h"
#include "linegroup.h"

namespace A3D {

class Model;
class Group : public QObject {
	Q_OBJECT
public:
	enum RenderOption {
		NoOptions = 0x0,

		// Hide this object and all children
		Hidden = 0x1,
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	explicit Group(Model* model);
	~Group();

	Group* clone(Model* m, bool deepClone) const;

	RenderOptions renderOptions() const;
	void setRenderOptions(RenderOptions);

	Model* model() const;

	void setPosition(QVector3D const&);
	QVector3D position() const;

	void setRotation(QQuaternion const&);
	QQuaternion rotation() const;

	void setScale(QVector3D const& = QVector3D(1.f, 1.f, 1.f));
	QVector3D scale() const;

	QMatrix4x4 const& groupMatrix() const;

	LineGroup* lineGroup() const;
	Mesh* mesh() const;
	Material* material() const;
	MaterialProperties* materialProperties() const;

	void setLineGroup(LineGroup*);
	void setMesh(Mesh*);
	void setMaterial(Material*);
	void setMaterialProperties(MaterialProperties*);

private:
	RenderOptions m_renderOptions;

	QPointer<Model> m_model;
	mutable bool m_matrixDirty;
	mutable QMatrix4x4 m_matrix;

	QVector3D m_position;
	QQuaternion m_rotation;
	QVector3D m_scale;

	QPointer<LineGroup> m_lineGroup;
	QPointer<Mesh> m_mesh;
	QPointer<Material> m_material;
	QPointer<MaterialProperties> m_materialProperties;
};

}

#endif // A3DGROUP_H
