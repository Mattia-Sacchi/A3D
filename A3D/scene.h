#ifndef A3DSCENE_H
#define A3DSCENE_H

#include "A3D/common.h"
#include <QObject>
#include "A3D/entity.h"
#include "A3D/resourcemanager.h"

namespace A3D {

class Scene : public Entity {
	Q_OBJECT
public:
	struct PointLightInfo {
		QVector3D position;
		float brightness;
		QString brightnessPattern;

		// X = Constant attenuation (0.0-1.0)
		// Y = Linear falloff (
		QVector3D falloff_CLQ;
	};
	struct LightInfo {
		QVector3D position;
		QVector3D direction;
		QVector4D color;

		float pointDiameter;
		float coneDiameter;
		float coneLength;
		enum Type { PointLight, ConeLight } type;
	};

	explicit Scene(QObject* parent = nullptr);
	~Scene();

	ResourceManager& resourceManager();
	ResourceManager const& resourceManager() const;

	void setAmbientLightColor(QVector4D const&);
	QVector4D ambientLightColor() const;

	void setDirectionalLightVector(QVector3D const&);
	QVector3D directionalLightVector() const;

	void setDirectionalLightColor(QVector4D const&);
	QVector4D directionalLightColor() const;

private:
	ResourceManager m_resourceManager;

	QVector4D m_ambientLightColor;

	QVector4D m_directionalLightVector;
	QVector4D m_directionalLightColor;
};

}

#endif // A3DSCENE_H
