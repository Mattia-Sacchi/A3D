#ifndef SURFACECHARTENTITY_H
#define SURFACECHARTENTITY_H

#include "linegroup.h"
#include "entity.h"
#include "mesh.h"
#include <QObject>

namespace A3D {

class SurfaceChartEntity : public Entity {
	Q_OBJECT
public:
	SurfaceChartEntity(Entity* parent = nullptr);

	struct Axis {
		Axis(QVector3D dir, std::vector<float> data, std::vector<float> normalizedData)
			: m_direction(dir),
			  m_data(data),
			  m_normalizedData(normalizedData) {}
		QVector3D m_direction;
		std::vector<float> m_data;
		std::vector<float> m_normalizedData;
	};

	void setTickLength(float);

	void addNormalizedAxis(QVector3D direction, std::vector<float> data);
	void addLinearAxis(QVector3D direction, float min, float max, unsigned int ticks = 10);

	void loadSurface(Mesh* mesh);

private:
	void addAxis(Axis axis);
	float m_tickLength;
	LineGroup* m_lineGroup;

	LineGroup* m_smallerLineGroup;

	LineGroup::Vertex m_origin;
	std::vector<Axis> m_axes;
};

}

#endif // SURFACECHARTENTITY_H
