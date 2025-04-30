#ifndef SURFACECHARTENTITY_H
#define SURFACECHARTENTITY_H

#include "linegroup.h"
#include "entity.h"
#include "maps.h"
#include <QObject>
#include <vector>

namespace A3D {

class SurfaceChartEntity : public Entity {
	Q_OBJECT
public:
	SurfaceChartEntity(Entity* parent = nullptr);

	struct Axis {
		Axis(AxisData data, std::vector<float> normalizedData)
			: m_name(""),
			  m_axisData(std::move(data)),
			  m_normalizedData(std::move(normalizedData)) {}

		Axis() {}

		inline void setName(QString name) { m_name = name; }
		inline QString name() const { return m_name; }
		QString m_name;
		AxisData m_axisData;
		std::vector<float> m_normalizedData;
	};

	void setTickLength(float);

	bool setMap(ResourceManager* rm, Map map);

	inline bool setAxisName(Direction3D dir, QString name) {
		if(dir >= m_axes.size())
			return false;
		m_axes[dir].setName(name);
		return true;
	}

	void debug(QVector3D);

	void drawIntersect(QVector3D);

private:
	const std::array<QVector3D, D_Count> m_commonDirections = {
		QVector3D(1, 0, 0), QVector3D(0, 1, 0), QVector3D(0, 0, 1), QVector3D(-1, 0, 0), QVector3D(0, -1, 0), QVector3D(0, 0, -1),
	};
	bool addAxis(Direction3D direction, AxisData data);
	Axis getAxisFromAxisData(AxisData data);
	void addTickText(Direction3D dir, QString text, QVector3D textBasePosition);
	void addTick(LineGroup::Vertex base, LineGroup::Vertex target);
	float m_tickLength;
	LineGroup* m_lineGroup;

	LineGroup* m_smallerLineGroup;

	LineGroup* m_intersectLineGroup;

	LineGroup::Vertex m_origin;
	std::array<Axis, D_Positive_Count> m_axes;
};

}

#endif // SURFACECHARTENTITY_H
