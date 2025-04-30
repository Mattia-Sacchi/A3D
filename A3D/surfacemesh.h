#ifndef A3DSURFACEMESH_H
#define A3DSURFACEMESH_H

#include "mesh.h"

namespace A3D {

class SurfaceMesh : public Mesh {
	Q_OBJECT
public:

	static SurfaceMesh* generateSurfaceMesh(ResourceManager* parent,Map const& map);
	explicit SurfaceMesh(ResourceManager* = nullptr);
	
private:
	struct ChartData{
		QVector3D m_realPoint;
		size_t m_index;
	};

	std::vector<ChartData> m_pointData;
};


}
#endif // A3DSURFACEMESH_H
