#include "surfacechartentity.h"
namespace A3D {

SurfaceChartEntity::SurfaceChartEntity(Entity* parent)
	: Entity(parent) {

	A3D::Model* m = new A3D::Model(this);

	m_origin.Position3D = QVector3D(1.f, 0.f, 1.f);
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

void SurfaceChartEntity::loadSurface(Mesh* mesh) {
	Group* g = model()->getOrAddGroup("Mesh");
	g->setMesh(mesh);
}

void SurfaceChartEntity::addAxis(Axis axis) {
	switch(axis.m_type) {
	default:
	case Axis_linear:

		if(!axis.m_ticksCount)
			axis.m_ticksCount = 10;
		axis.m_data.clear();
		axis.m_data.reserve(axis.m_ticksCount);

		static float const multiplier = 1 / static_cast<float>(axis.m_ticksCount);

		for(size_t i = 0; i < axis.m_ticksCount; i++)
			axis.m_data.push_back(multiplier * i);

		{
			m_lineGroup->vertices().push_back(m_origin);

			LineGroup::Vertex axisTarget = m_origin;
			axisTarget.Position3D += axis.m_direction;
			m_lineGroup->vertices().push_back(axisTarget);
		}

		break;
	case Axis_normalized:
		std::vector<float>& data = axis.m_data;
		auto itMin               = std::min_element(data.begin(), data.end());
		if(itMin == data.end())
			return;

		auto itMax = std::max_element(data.begin(), data.end());
		if(itMax == data.end())
			return;

		{
			m_lineGroup->vertices().push_back(m_origin);

			LineGroup::Vertex axisTarget = m_origin;
			axisTarget.Position3D += axis.m_direction;
			m_lineGroup->vertices().push_back(axisTarget);
		}

		normalizeMinMax(axis.m_data, *itMin, *itMax);

		break;
	}

	for(float axisValue: std::as_const(axis.m_data)) {
		LineGroup::Vertex base = m_origin;
		base.Position3D += axis.m_direction * (1.f - axisValue);

		{
			LineGroup::Vertex vxUp = base;
			vxUp.Position3D += QVector3D(0, 0.025f, 0);
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxUp);
		}

		{
			LineGroup::Vertex vxDirection = base;
			vxDirection.Position3D -= QVector3D(-0.025f, 0, 0);
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxDirection);
		}

		{
			LineGroup::Vertex vxDirection = base;
			vxDirection.Position3D += QVector3D(0.025f, 0, 0);
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxDirection);
		}

		{
			LineGroup::Vertex vxDirection = base;
			vxDirection.Position3D -= QVector3D(0, 0, -0.025f);
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxDirection);
		}

		{
			LineGroup::Vertex vxDirection = base;
			vxDirection.Position3D += QVector3D(0, 0, 0.025f);
			m_smallerLineGroup->vertices().push_back(base);
			m_smallerLineGroup->vertices().push_back(vxDirection);
		}
	}

	m_axes.push_back(axis);

	/*if(horizontalAxis.size() * verticalAxis.size() != data.size())
		return nullptr;

	A3D::Mesh* mesh = new A3D::Mesh(parent);
	mesh->setDrawMode(A3D::Mesh::IndexedTriangles);

	normalize(horizontalAxis);
	normalize(verticalAxis);
	normalize(data);

	std::size_t const height = verticalAxis.size();
	std::size_t const width  = horizontalAxis.size();

	mesh->setContents(A3D::Mesh::Position3D | A3D::Mesh::TextureCoord2D | A3D::Mesh::Normal3D);
	mesh->indices().reserve(mesh->vertices().size());

	for(size_t i = 0; i < mesh->vertices().size(); i++)
		mesh->indices().push_back(i);

	mesh->optimizeIndices();
	mesh->invalidateCache();
	return mesh;*/
}

}
