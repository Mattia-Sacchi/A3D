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
			: m_name(""),
			  m_data(data),
			  m_normalizedData(normalizedData) {}

		inline void setName(QString name) { m_name = name; }
		inline QString name() const { return m_name; }
		QString m_name;
		QStringList m_labels;
		std::vector<float> m_data;
		std::vector<float> m_normalizedData;
	};

	void setTickLength(float);

	bool addNormalizedAxis(Direction3D direction, std::vector<float> data);
	bool addLinearAxis(Direction3D direction, float min, float max, unsigned int ticks = 10);
	bool addLinearAxis(Direction3D direction, QStringList);

	inline bool setAxisName(Direction3D dir, QString name) {
		if(m_axes.find(dir) == m_axes.end())
			return false;
		m_axes.at(dir).setName(name);
		return true;
	}

	void debug(QVector3D);

	void drawIntersect(QVector3D);

	void loadSurface(Mesh* mesh);

private:
	const std::array<QVector3D, D_Count> m_commonDirections = {
		QVector3D(1, 0, 0), QVector3D(0, 1, 0), QVector3D(0, 0, 1), QVector3D(-1, 0, 0), QVector3D(0, -1, 0), QVector3D(0, 0, -1),
	};
	bool addAxis(Direction3D direction, std::vector<float> data, std::vector<float> normalizedData);
	float m_tickLength;
	LineGroup* m_lineGroup;

	LineGroup* m_smallerLineGroup;

	LineGroup* m_intersectLineGroup;

	LineGroup::Vertex m_origin;
	std::map<Direction3D, Axis> m_axes;
};

}

#endif // SURFACECHARTENTITY_H
