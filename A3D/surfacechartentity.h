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

	enum Direction3D {
		X_Axis = 0,
		Y_Axis,
		Z_Axis,
		Negative_X,
		Negative_Y,
		Negative_Z,
		D_Count,
	};

	struct Axis {
		Axis(std::vector<float> data, std::vector<float> normalizedData)
			: 
			  m_data(data),
			  m_normalizedData(normalizedData) {}
		std::vector<float> m_data;
		std::vector<float> m_normalizedData;
	};

	void setTickLength(float);

	bool addNormalizedAxis(Direction3D direction, std::vector<float> data);
	bool addLinearAxis(Direction3D direction, float min, float max, unsigned int ticks = 10);

	void loadSurface(Mesh* mesh);

private:
	const std::array<QVector3D, D_Count> m_commonDirections = {
		QVector3D(1, 0, 0), QVector3D(0, 1, 0), QVector3D(0, 0, 1), QVector3D(-1, 0, 0), QVector3D(0, -1, 0), QVector3D(0, 0, -1),
	};
	bool addAxis(Direction3D direction,std::vector<float> data, std::vector<float> normalizedData);
	float m_tickLength;
	LineGroup* m_lineGroup;

	LineGroup* m_smallerLineGroup;

	LineGroup::Vertex m_origin;
	std::map<Direction3D,Axis> m_axes;
};

}

#endif // SURFACECHARTENTITY_H
