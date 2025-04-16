#include "surfacechartentity.h"
#include "textbillboardentity.h"

namespace A3D {

SurfaceChartEntity::SurfaceChartEntity(Entity* parent)
	: Entity(parent),
	  m_tickLength(0.025f) {

	A3D::Model* m = new A3D::Model(this);

	m_origin.Position3D = QVector3D(0.f, 0.f, 0.f);
	m_origin.Color4D    = QVector4D(0.f, 0.f, 0.f, 1.f);

	{
		A3D::Group* meshGroup = m->getOrAddGroup("Mesh");
		meshGroup->setMaterial(A3D::Material::standardMaterial(A3D::Material::ColoredSurfaceMaterial));
		meshGroup->setMaterialProperties(new MaterialProperties);
	}

	{
		A3D::Group* group = m->getOrAddGroup("BiggerLines");

		m_lineGroup = new LineGroup();
		group->setLineGroup(m_lineGroup);
		m_lineGroup->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
		m_lineGroup->setThickness(0.01f);
	}

	{
		A3D::Group* group = m->getOrAddGroup("SmallerLines");

		m_smallerLineGroup = new LineGroup();
		m_smallerLineGroup->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
		m_smallerLineGroup->setThickness(0.003f);
		group->setLineGroup(m_smallerLineGroup);
	}

	setModel(m);
}

void SurfaceChartEntity::setTickLength(float length) {
	m_tickLength = length;
}

void SurfaceChartEntity::loadSurface(Mesh* mesh) {
	Group* g = model()->getOrAddGroup("Mesh");
	g->setMesh(mesh);
	g->setRotation(QQuaternion::fromAxisAndAngle(0, 1, 0, 180));
	g->setPosition(QVector3D(-1.f, 0.f, -1.f));
}

bool SurfaceChartEntity::addNormalizedAxis(Direction3D direction, std::vector<float> data) {

	auto itMin = std::min_element(data.begin(), data.end());
	if(itMin == data.end())
		return false;

	auto itMax = std::max_element(data.begin(), data.end());
	if(itMax == data.end())
		return false;

	std::vector normalizedData = data;

	normalizeMinMax(normalizedData, *itMin, *itMax);

	if(direction >= Negative_X)
		return false;

	return addAxis(Axis(m_commonDirections[direction], data, normalizedData));
}

bool SurfaceChartEntity::addLinearAxis(Direction3D direction, float min, float max, unsigned int ticks) {
	std::vector<float> normalizedData;
	normalizedData.clear();
	normalizedData.reserve(ticks);
	std::vector<float> data;
	data.clear();
	data.reserve(ticks);

	float const multiplier       = 1 / static_cast<float>(ticks);
	float const strokeMultiplier = (max - min) / ticks;

	for(size_t i = 0; i < ticks; i++) {

		normalizedData.push_back(multiplier * i);
		data.push_back((strokeMultiplier * i) + min);
	}

	if(direction >= Negative_X)
		return false;
	return addAxis(Axis(m_commonDirections[direction], data, normalizedData));
}

bool isSameDirection(const QVector3D& a, const QVector3D& b, float tolerance = 1e-5f) {
	QVector3D aNorm = a.normalized();
	QVector3D bNorm = b.normalized();

	float dot = QVector3D::dotProduct(aNorm, bNorm);

	return std::abs(dot - 1.0f) <= tolerance;
}

bool areOrthogonal(const QVector3D& a, const QVector3D& b, float tolerance = 1e-5f) {
	float dot = QVector3D::dotProduct(a, b);
	return std::abs(dot) <= tolerance;
}

bool SurfaceChartEntity::addAxis(Axis axis) {

	// Draw the main axis
	m_lineGroup->vertices().push_back(m_origin);
	LineGroup::Vertex axisTarget = m_origin;
	axisTarget.Position3D += axis.m_direction;
	m_lineGroup->vertices().push_back(axisTarget);

	// Draw the ticks
	for(size_t i = 0; i < axis.m_data.size(); ++i) {

		LineGroup::Vertex base = m_origin;
		base.Position3D += axis.m_direction * (1.f - axis.m_normalizedData.at(i));

		// Text
		{
			TextBillboardEntity* text = emplaceChildEntity<TextBillboardEntity>();
			text->setText(QString::number(axis.m_data.at(axis.m_data.size() - 1 - i)));
			text->setColor(Qt::white);
			text->setFont(QFont("Arial", 64));
			text->setScale(QVector3D(0.05, 0.05, 0.05));
			text->setPosition(base.Position3D);
		}

		{
			LineGroup::Vertex vxUp = base;
			vxUp.Position3D += QVector3D(0, m_tickLength, 0);
			if(!isSameDirection(vxUp.Position3D, axisTarget.Position3D)) {
				m_smallerLineGroup->vertices().push_back(base);
				m_smallerLineGroup->vertices().push_back(vxUp);
			}
		}

		{
			LineGroup::Vertex vxDirection = base;
			vxDirection.Position3D -= QVector3D(-m_tickLength, 0, 0);
			if(!isSameDirection(vxDirection.Position3D, axisTarget.Position3D)) {
				m_smallerLineGroup->vertices().push_back(base);
				m_smallerLineGroup->vertices().push_back(vxDirection);
			}
		}

		{
			LineGroup::Vertex vxDirection = base;
			vxDirection.Position3D += QVector3D(m_tickLength, 0, 0);
			if(!isSameDirection(vxDirection.Position3D, axisTarget.Position3D)) {
				m_smallerLineGroup->vertices().push_back(base);
				m_smallerLineGroup->vertices().push_back(vxDirection);
			}
		}

		{
			LineGroup::Vertex vxDirection = base;
			vxDirection.Position3D -= QVector3D(0, 0, -m_tickLength);
			if(!isSameDirection(vxDirection.Position3D, axisTarget.Position3D)) {
				m_smallerLineGroup->vertices().push_back(base);
				m_smallerLineGroup->vertices().push_back(vxDirection);
			}
		}

		{
			LineGroup::Vertex vxDirection = base;
			vxDirection.Position3D += QVector3D(0, 0, m_tickLength);
			if(!isSameDirection(vxDirection.Position3D, axisTarget.Position3D)) {
				m_smallerLineGroup->vertices().push_back(base);
				m_smallerLineGroup->vertices().push_back(vxDirection);
			}
		}
	}

	m_axes.push_back(axis);
	return true;
}

}
