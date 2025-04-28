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

	{
		A3D::Group* group = m->getOrAddGroup("IntersectLine");

		m_intersectLineGroup = new LineGroup();
		m_intersectLineGroup->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
		m_intersectLineGroup->setThickness(0.01f);
		group->setLineGroup(m_intersectLineGroup);
	}

	setModel(m);
}

void SurfaceChartEntity::setTickLength(float length) {
	m_tickLength = length;
}

void SurfaceChartEntity::drawIntersect(QVector3D d) {
	QVector3D startVector = QVector3D(1 - d.x(), 0.f, 1 - d.z());
	QVector3D endVector   = QVector3D(1 - d.x(), 1.f, 1 - d.z());

	m_intersectLineGroup->vertices().clear();
	m_intersectLineGroup->indices().clear();

	LineGroup::Vertex start;
	LineGroup::Vertex end;
	start.Position3D = startVector;
	start.Color4D    = QVector4D(0.5, 0, 1, 1);
	end.Position3D   = endVector;
	end.Color4D      = QVector4D(0.5, 0, 1, 1);

	m_intersectLineGroup->vertices().push_back(start);
	m_intersectLineGroup->vertices().push_back(end);
	m_intersectLineGroup->invalidateCache();
}

bool SurfaceChartEntity::setMap(ResourceManager* rm, Map map) {
	if(!map.isValid())
		return false;

	Group* g = model()->getOrAddGroup("Mesh");

	AxisData xAxis = map.getAxis(D_X_Axis);
	AxisData zAxis = map.getAxis(D_Z_Axis);
	AxisData yAxis = map.getAxis(D_Y_Axis);

	Mesh* mesh = Mesh::generateSurfaceMesh(rm, xAxis.toVector(), zAxis.toVector(), map.data(), xAxis.m_isFixed, zAxis.m_isFixed);

	mesh->setRenderOptions(Mesh::DisableCulling);
	g->setRotation(QQuaternion::fromAxisAndAngle(0, 1, 0, 180));
	g->setPosition(QVector3D(-1.f, 0.f, -1.f));
	g->setMesh(mesh);

	if(!addAxis(D_X_Axis, xAxis))
		return false;

	if(!addAxis(D_Z_Axis, zAxis))
		return false;

	if(!addAxis(D_Y_Axis, yAxis))
		return false;
	return true;
}

void SurfaceChartEntity::debug(QVector3D d) {
	for(size_t i = 0; i < D_Positive_Count; i++) {
		Axis& a                 = m_axes[i];
		std::vector<float> data = a.m_axisData.toVector();
		auto itMin              = std::min_element(data.begin(), data.end());
		if(itMin == data.end())
			return;

		auto itMax = std::max_element(data.begin(), data.end());
		if(itMax == data.end())
			return;
		float pos = d[i];
		if(a.m_axisData.m_isFixed) {
			// I need to denormalize it and obtain back the string
			std::size_t c = static_cast<std::size_t>(pos * (a.m_axisData.size()));
			c             = std::clamp<size_t>(c, 0, a.m_axisData.size() - 1);
			qDebug() << a.name() << ": " << a.m_axisData.m_data[c].text;
		}
		else
			qDebug() << a.name() << ": " << ((*itMax - *itMin) * pos) + *itMin;
	}
}

bool isSameDirection(const QVector3D& a, const QVector3D& b, float tolerance = 1e-5f) {
	QVector3D aNorm = a.normalized();
	QVector3D bNorm = b.normalized();

	float dot = QVector3D::dotProduct(aNorm, bNorm);

	return std::abs(dot - 1.0f) <= tolerance;
}

inline void setupTextInfo(TextBillboardEntity* text, QString textValue) {
	text->setText(textValue);
	text->setColor(Qt::white);
	text->setFont(QFont("Arial", 64));
	text->setScale(QVector3D(0.05f, 0.05f, 0.05f));
}

SurfaceChartEntity::Axis SurfaceChartEntity::getAxisFromAxisData(AxisData axisData) {
	std::vector<float> data;
	data.clear();
	float min = 0.f;
	float max = 0.f;

	if(!axisData.m_isFixed) {

		data.reserve(axisData.size());
		for(size_t i = 0; i < axisData.size(); i++)
			data.push_back(axisData.m_data[i].value);

		// Sanity check already did in Map
		auto itMin = std::min_element(data.begin(), data.end());
		min        = *itMin;

		// Sanity check already did in Map
		auto itMax = std::max_element(data.begin(), data.end());
		max        = *itMax;
	}
	else {
		data.reserve(axisData.size());
		for(size_t i = 0; i < axisData.size(); i++)
			data.push_back(i);

		min = 0;
		max = axisData.size() - 1;
	}

	std::vector<float> normalizedData = data;

	normalizeMinMax(normalizedData, min, max);

	return Axis(axisData, normalizedData);
}

void SurfaceChartEntity::addTickText(Direction3D dir, QString text, QVector3D textBasePosition) {

	TextBillboardEntity* textA = emplaceChildEntity<TextBillboardEntity>();
	TextBillboardEntity* textB = emplaceChildEntity<TextBillboardEntity>();

	setupTextInfo(textA, text);
	setupTextInfo(textB, text);

	float const minimunTextOffset = 0.05f;
	switch(dir) {
	case D_Y_Axis:
		{
			QVector3D textPositionA = textBasePosition;
			QVector3D textPositionB = textBasePosition;
			textPositionA.setX(1.f + minimunTextOffset);
			textPositionA.setZ(0.f);

			textPositionB.setX(0.f);
			textPositionB.setZ(1.f + minimunTextOffset);

			textA->setPosition(textPositionA);
			textB->setPosition(textPositionB);
		}
		break;
	case D_X_Axis:
		{
			QVector3D textPositionA = textBasePosition;
			QVector3D textPositionB = textBasePosition;
			textPositionA.setZ(textPositionA.z());
			textPositionB.setZ(textPositionB.z() - minimunTextOffset);

			textPositionB.setY(1.f + minimunTextOffset);
			textA->setPosition(textPositionA);
			textB->setPosition(textPositionB);
		}
		break;
	case D_Z_Axis:
		{
			QVector3D textPositionA = textBasePosition;
			QVector3D textPositionB = textBasePosition;
			textPositionA.setX(textPositionA.x());
			textPositionB.setX(textPositionB.x() - minimunTextOffset);

			textPositionB.setY(1.f + minimunTextOffset);
			textA->setPosition(textPositionA);
			textB->setPosition(textPositionB);
		}
		break;
	}
}

void SurfaceChartEntity::addTick(LineGroup::Vertex base, LineGroup::Vertex target) {

	{
		LineGroup::Vertex vxUp = base;
		vxUp.Position3D += QVector3D(0, m_tickLength, 0);
		if(!isSameDirection(vxUp.Position3D, target.Position3D)) {
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxUp);
		}
	}

	{
		LineGroup::Vertex vxDirection = base;
		vxDirection.Position3D -= QVector3D(-m_tickLength, 0, 0);
		if(!isSameDirection(vxDirection.Position3D, target.Position3D)) {
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxDirection);
		}
	}

	{
		LineGroup::Vertex vxDirection = base;
		vxDirection.Position3D += QVector3D(m_tickLength, 0, 0);
		if(!isSameDirection(vxDirection.Position3D, target.Position3D)) {
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxDirection);
		}
	}

	{
		LineGroup::Vertex vxDirection = base;
		vxDirection.Position3D -= QVector3D(0, 0, -m_tickLength);
		if(!isSameDirection(vxDirection.Position3D, target.Position3D)) {
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxDirection);
		}
	}

	{
		LineGroup::Vertex vxDirection = base;
		vxDirection.Position3D += QVector3D(0, 0, m_tickLength);
		if(!isSameDirection(vxDirection.Position3D, target.Position3D)) {
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxDirection);
		}
	}
}

bool SurfaceChartEntity::addAxis(Direction3D direction, AxisData axisData) {

	Axis axis               = getAxisFromAxisData(axisData);
	std::vector<float> data = axis.m_axisData.toVector();

	// Draw the main axis
	QVector3D directionVector = m_commonDirections[direction];
	m_lineGroup->vertices().push_back(m_origin);
	LineGroup::Vertex axisTarget = m_origin;
	axisTarget.Position3D += directionVector;
	axisTarget.Color4D = QVector4D(direction == D_X_Axis, direction == D_Y_Axis, direction == D_Z_Axis, 1);
	m_lineGroup->vertices().push_back(axisTarget);

	// Draw the ticks
	for(size_t i = 0; i < data.size(); ++i) {

		LineGroup::Vertex base = m_origin;
		base.Position3D += directionVector * (1.f - axis.m_normalizedData.at(i));

		size_t index = direction == D_Y_Axis ? data.size() - 1 - i : i;

		// Text

		QVector3D textBasePosition = base.Position3D;
		QString text               = axisData.m_isFixed ? axisData.m_data[index].text : QString::number(data.at(index));

		addTickText(direction, text, textBasePosition);
		addTick(base, axisTarget);
	}

	m_axes[direction] = axis;
	return true;
}
}