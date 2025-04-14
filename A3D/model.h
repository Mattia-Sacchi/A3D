#ifndef A3DMODEL_H
#define A3DMODEL_H

#include "common.h"
#include <QObject>
#include "group.h"

namespace A3D {

class Model : public QObject {
	Q_OBJECT
public:
	enum RenderOption {
		NoOptions = 0x0,

		// Hide this object and all groups
		Hidden = 0x1,
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	explicit Model(QObject* parent = nullptr);
	~Model();

	Model* clone(bool deepClone) const;

	RenderOptions renderOptions() const;
	void setRenderOptions(RenderOptions);

	Group* addGroup(QString name);
	Group* getOrAddGroup(QString name);
	Group* getGroup(QString const& name);

	QStringList groupNames() const;
	std::map<QString, QPointer<Group>> const& groups() const;

	void setPosition(QVector3D const&);
	QVector3D position() const;

	void setRotation(QQuaternion const&);
	QQuaternion rotation() const;

	void setScale(QVector3D const& = QVector3D(1.f, 1.f, 1.f));
	QVector3D scale() const;

	QMatrix4x4 const& modelMatrix() const;

private:
	RenderOptions m_renderOptions;

	mutable bool m_matrixDirty;
	mutable QMatrix4x4 m_matrix;

	QVector3D m_position;
	QQuaternion m_rotation;
	QVector3D m_scale;

	std::map<QString, QPointer<Group>> m_groups;
};

}

#endif // A3DMODEL_H
