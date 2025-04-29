#include "mesh.h"
#include <algorithm>

namespace A3D {

static void getVertexFromCoordinate(
	A3D::Mesh::Vertex& vx, std::vector<float> const& horizontalAxis, std::vector<float> const& verticalAxis, std::vector<float> const& data, std::size_t x, std::size_t y
) {
	vx.Position3D     = QVector3D(horizontalAxis[x], data[x + (y * horizontalAxis.size())], verticalAxis[y]);
	vx.TextureCoord2D = QVector2D(vx.Position3D.x(), vx.Position3D.z());
}

static void generateNormal(A3D::Mesh::Vertex& triangleVertexA, A3D::Mesh::Vertex& triangleVertexB, A3D::Mesh::Vertex& triangleVertexC) {
	triangleVertexA.Normal3D = triangleVertexB.Normal3D = triangleVertexC.Normal3D
		= QVector3D::normal(triangleVertexA.Position3D, triangleVertexB.Position3D, triangleVertexC.Position3D);
}

Mesh* Mesh::generateSurfaceMesh(
	A3D::ResourceManager* parent, Map const& map
) {
	if(!map.isValid())
		return nullptr;
	std::vector<float> data = map.data();
	std::vector<float> xAxis = map.getAxis(D_X_Axis).toVector();
	std::vector<float> yAxis = map.getAxis(D_Y_Axis).toVector();
	std::vector<float> zAxis = map.getAxis(D_Z_Axis).toVector();

	bool xAxisFixed = map.getAxis(D_X_Axis).m_isFixed;
	bool yAxisFixed = map.getAxis(D_Y_Axis).m_isFixed;
	bool zAxisFixed = map.getAxis(D_Z_Axis).m_isFixed;

	float yMin = 0.f;
	float yMax = 0.f;

	auto itMin = std::min_element(yAxis.begin(), yAxis.end());
	auto itMax = std::max_element(yAxis.begin(), yAxis.end());
	yMin = *itMin;
	yMax = *itMax;
	// Sanity check already did in map isValid();

	if(xAxis.size() * zAxis.size() != data.size())
		return nullptr;

	A3D::Mesh* mesh = new A3D::Mesh(parent);
	mesh->setDrawMode(A3D::Mesh::IndexedTriangles);

	normalize(xAxis);
	normalize(zAxis);
	if(yAxisFixed)
		normalize(data);
	else
		normalizeMinMax(data,yMin,yMax);


	std::size_t const height = zAxis.size();
	std::size_t const width  = xAxis.size();

	float const xThickness = (1.f/width) * 0.9;
	float const zThickness = (1.f/height) * 0.9;

	for(size_t y = 0; y < height - 1 + zAxisFixed; ++y) {
		for(size_t x = 0; x < width - 1 + xAxisFixed; ++x) {
			A3D::Mesh::Vertex topLeft;
			A3D::Mesh::Vertex topRight;
			A3D::Mesh::Vertex bottomLeft;
			A3D::Mesh::Vertex bottomRight;

			getVertexFromCoordinate(topLeft, xAxis, zAxis, data, x, y);
			getVertexFromCoordinate(topRight, xAxis, zAxis, data, x + 1 - xAxisFixed, y);
			getVertexFromCoordinate(bottomLeft, xAxis, zAxis, data, x, y + 1 - zAxisFixed);
			getVertexFromCoordinate(bottomRight, xAxis, zAxis, data, x + 1 - xAxisFixed, y + 1 - zAxisFixed);

			if(xAxisFixed) {

				float leftThickness  = topLeft.Position3D.x() * xThickness;
				float rightThickness = (1 - topLeft.Position3D.x()) * xThickness;

				topLeft.Position3D.setX(topLeft.Position3D.x() - leftThickness);
				bottomLeft.Position3D.setX(bottomLeft.Position3D.x() - leftThickness);
				topRight.Position3D.setX(topRight.Position3D.x() + rightThickness);
				bottomRight.Position3D.setX(bottomRight.Position3D.x() + rightThickness);
			}
			if(zAxisFixed) {

				float leftThickness  = topLeft.Position3D.z() * zThickness;
				float rightThickness = (1 - topLeft.Position3D.z()) * zThickness;


				topLeft.Position3D.setZ(topLeft.Position3D.z()- leftThickness);
				topRight.Position3D.setZ(topRight.Position3D.z() -leftThickness);
				bottomLeft.Position3D.setZ(bottomLeft.Position3D.z() + rightThickness);
				bottomRight.Position3D.setZ(bottomRight.Position3D.z() + rightThickness);
			}

			A3D::Mesh::Vertex center;
			center.Position3D     = (topLeft.Position3D + topRight.Position3D + bottomLeft.Position3D + bottomRight.Position3D) * 0.25f;
			center.TextureCoord2D = (topLeft.TextureCoord2D + topRight.TextureCoord2D + bottomLeft.TextureCoord2D + bottomRight.TextureCoord2D) * 0.25f;

			generateNormal(topRight, topLeft, center);
			mesh->vertices().push_back(topRight);
			mesh->vertices().push_back(topLeft);
			mesh->vertices().push_back(center);

			generateNormal(topLeft, bottomLeft, center);
			mesh->vertices().push_back(topLeft);
			mesh->vertices().push_back(bottomLeft);
			mesh->vertices().push_back(center);

			generateNormal(bottomLeft, bottomRight, center);
			mesh->vertices().push_back(bottomLeft);
			mesh->vertices().push_back(bottomRight);
			mesh->vertices().push_back(center);

			generateNormal(bottomRight, topRight, center);
			mesh->vertices().push_back(bottomRight);
			mesh->vertices().push_back(topRight);
			mesh->vertices().push_back(center);
		}
	}

	mesh->setContents(A3D::Mesh::Position3D | A3D::Mesh::TextureCoord2D | A3D::Mesh::Normal3D);
	mesh->indices().reserve(mesh->vertices().size());

	for(uint32_t i = 0; i < mesh->vertices().size(); i++)
		mesh->indices().push_back(i);

	mesh->invalidateCache();
	return mesh;
}

}
