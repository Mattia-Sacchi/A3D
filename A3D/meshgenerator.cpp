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

Mesh* Mesh::generateSurfaceMesh(A3D::ResourceManager* parent, std::vector<float> horizontalAxis, std::vector<float> verticalAxis, std::vector<float> data) {
	if(horizontalAxis.size() * verticalAxis.size() != data.size())
		return nullptr;

	A3D::Mesh* mesh = new A3D::Mesh(parent);
	mesh->setDrawMode(A3D::Mesh::IndexedTriangles);

	normalize(horizontalAxis);
	normalize(verticalAxis);
	normalize(data);

	std::size_t const height = verticalAxis.size();
	std::size_t const width  = horizontalAxis.size();

	for(size_t y = 0; y < height - 1; ++y) {
		for(size_t x = 0; x < width - 1; ++x) {
			A3D::Mesh::Vertex topLeft;
			A3D::Mesh::Vertex topRight;
			A3D::Mesh::Vertex bottomLeft;
			A3D::Mesh::Vertex bottomRight;

			getVertexFromCoordinate(topLeft, horizontalAxis, verticalAxis, data, x, y);
			getVertexFromCoordinate(topRight, horizontalAxis, verticalAxis, data, x + 1, y);
			getVertexFromCoordinate(bottomLeft, horizontalAxis, verticalAxis, data, x, y + 1);
			getVertexFromCoordinate(bottomRight, horizontalAxis, verticalAxis, data, x + 1, y + 1);

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

	for(size_t i = 0; i < mesh->vertices().size(); i++)
		mesh->indices().push_back(i);

	mesh->optimizeIndices();
	mesh->invalidateCache();
	return mesh;
}

}
