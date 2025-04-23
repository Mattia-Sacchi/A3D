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
	mesh->setRenderOptions(Mesh::DisableCulling);
	g->setMesh(mesh);
	g->setRotation(QQuaternion::fromAxisAndAngle(0, 1, 0, 180));
	g->setPosition(QVector3D(-1.f, 0.f, -1.f));
}
void SurfaceChartEntity::debug(QVector3D d) {
	for(size_t i = 0; i < Negative_X; i++) {
		Axis& a                 = m_axes.at(static_cast<Direction3D>(i));
		std::vector<float> data = a.m_data;
		auto itMin              = std::min_element(data.begin(), data.end());
		if(itMin == data.end())
			return;

		auto itMax = std::max_element(data.begin(), data.end());
		if(itMax == data.end())
			return;
		if(i == Z_Axis)
			qDebug() << a.name() << ": " << static_cast<size_t>(((*itMax - *itMin) * d[i]) + *itMin);
		else
			qDebug() << a.name() << ": " << ((*itMax - *itMin) * d[i]) + *itMin;
	}
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

	return addAxis(direction, data, normalizedData);
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

	for(size_t i = 0; i <= ticks; i++) {

		normalizedData.push_back(multiplier * i);
		data.push_back((strokeMultiplier * i) + min);
	}

	if(direction >= Negative_X)
		return false;
	return addAxis(direction, data, normalizedData);
}

bool isSameDirection(const QVector3D& a, const QVector3D& b, float tolerance = 1e-5f) {
	QVector3D aNorm = a.normalized();
	QVector3D bNorm = b.normalized();

	float dot = QVector3D::dotProduct(aNorm, bNorm);

	return std::abs(dot - 1.0f) <= tolerance;
}

void setupTextInfo(TextBillboardEntity* text, float value) {
	text->setText(QString::number(value));
	text->setColor(Qt::white);
	text->setFont(QFont("Arial", 64));
	text->setScale(QVector3D(0.05f, 0.05f, 0.05f));
}

bool SurfaceChartEntity::addAxis(Direction3D direction, std::vector<float> data, std::vector<float> normalizedData) {

	QVector3D directionVector = m_commonDirections[direction];
	// Draw the main axis
	m_lineGroup->vertices().push_back(m_origin);
	LineGroup::Vertex axisTarget = m_origin;
	axisTarget.Position3D += directionVector;
	axisTarget.Color4D = QVector4D(direction == X_Axis, direction == Y_Axis, direction == Z_Axis, 1);
	m_lineGroup->vertices().push_back(axisTarget);
	float const minimunTextOffset = -0.1f;

	// Draw the ticks
	for(size_t i = 0; i < data.size(); ++i) {

		LineGroup::Vertex base = m_origin;
		base.Position3D += directionVector * (1.f - normalizedData.at(i));

		// Text
		{
			TextBillboardEntity* textA = emplaceChildEntity<TextBillboardEntity>();
			TextBillboardEntity* textB = emplaceChildEntity<TextBillboardEntity>();
			if(direction == Y_Axis) {
				float value = data.at(data.size() - 1 - i);
				setupTextInfo(textA, value);
				setupTextInfo(textB, value);
			}
			else {
				float value = data.at(i);
				setupTextInfo(textA, value);
				setupTextInfo(textB, value);
			}
			QVector3D textBasePosition = base.Position3D;

			switch(direction) {
			case Y_Axis:
				{
					QVector3D textPositionA = textBasePosition;
					QVector3D textPositionB = textBasePosition;
					textPositionA.setX(1.1f);
					textPositionA.setZ(minimunTextOffset);

					textPositionB.setX(minimunTextOffset);
					textPositionB.setZ(1.1f);

					textA->setPosition(textPositionA);
					textB->setPosition(textPositionB);
				}
				break;
			case X_Axis:
				{
					QVector3D textPositionA = textBasePosition;
					QVector3D textPositionB = textBasePosition;
					textPositionA.setZ(minimunTextOffset);
					textPositionB.setZ(minimunTextOffset);

					textPositionB.setY(1.1f);
					textA->setPosition(textPositionA);
					textB->setPosition(textPositionB);
				}
				break;
			case Z_Axis:
				{
					QVector3D textPositionA = textBasePosition;
					QVector3D textPositionB = textBasePosition;
					textPositionA.setX(minimunTextOffset);
					textPositionB.setX(minimunTextOffset);

					textPositionB.setY(1.1f);
					textA->setPosition(textPositionA);
					textB->setPosition(textPositionB);
				}
				break;
			}
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

	m_axes.emplace(direction, Axis(data, normalizedData));
	return true;
}

}
