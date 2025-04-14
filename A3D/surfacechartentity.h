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

	enum AxisType { Axis_linear, Axis_normalized };
	struct Axis {
		QVector3D m_direction;
		std::vector<float> m_data;
		uint8_t m_ticksCount;
		float m_min;
		float m_max;
		AxisType m_type;
	};

	void loadSurface(Mesh* mesh);

	void addAxis(Axis axis);

private:
	LineGroup* m_lineGroup;

	LineGroup* m_smallerLineGroup;

	LineGroup::Vertex m_origin;
	std::vector<Axis> m_axes;
};

}

#endif // SURFACECHARTENTITY_H
