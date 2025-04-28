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
	A3D::ResourceManager* parent, std::vector<float> horizontalAxis, std::vector<float> verticalAxis, std::vector<float> data, bool horizontalAxisFixed, bool verticalAxisFixed
) {
	if(horizontalAxis.size() * verticalAxis.size() != data.size())
		return nullptr;

	A3D::Mesh* mesh = new A3D::Mesh(parent);
	mesh->setDrawMode(A3D::Mesh::IndexedTriangles);

	normalize(horizontalAxis);
	normalize(verticalAxis);
	normalize(data);

	std::size_t const height = verticalAxis.size();
	std::size_t const width  = horizontalAxis.size();

	float const xThickness = (1.f/width) * 0.9;
	float const yThickness = (1.f/height) * 0.9;

	for(size_t y = 0; y < height - 1 + verticalAxisFixed; ++y) {
		for(size_t x = 0; x < width - 1 + horizontalAxisFixed; ++x) {
			A3D::Mesh::Vertex topLeft;
			A3D::Mesh::Vertex topRight;
			A3D::Mesh::Vertex bottomLeft;
			A3D::Mesh::Vertex bottomRight;

			getVertexFromCoordinate(topLeft, horizontalAxis, verticalAxis, data, x, y);
			getVertexFromCoordinate(topRight, horizontalAxis, verticalAxis, data, x + 1 - horizontalAxisFixed, y);
			getVertexFromCoordinate(bottomLeft, horizontalAxis, verticalAxis, data, x, y + 1 - verticalAxisFixed);
			getVertexFromCoordinate(bottomRight, horizontalAxis, verticalAxis, data, x + 1 - horizontalAxisFixed, y + 1 - verticalAxisFixed);

			if(horizontalAxisFixed) {

				float leftThickness  = topLeft.Position3D.x() * yThickness;
				float rightThickness = (1 - topLeft.Position3D.x()) * yThickness;

				topLeft.Position3D.setX(topLeft.Position3D.x() - leftThickness);
				bottomLeft.Position3D.setX(bottomLeft.Position3D.x() - leftThickness);
				topRight.Position3D.setX(topRight.Position3D.x() + rightThickness);
				bottomRight.Position3D.setX(bottomRight.Position3D.x() + rightThickness);
			}
			if(verticalAxisFixed) {

				float leftThickness  = topLeft.Position3D.z() * yThickness;
				float rightThickness = (1 - topLeft.Position3D.z()) * yThickness;


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
