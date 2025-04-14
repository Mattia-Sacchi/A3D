#ifndef A3DMESHCACHE_H
#define A3DMESHCACHE_H

#include "common.h"
#include <QObject>

namespace A3D {

class Mesh;
class MeshCache : public QObject {
	Q_OBJECT
public:
	explicit MeshCache(Mesh* parent);
	~MeshCache();

	Mesh* mesh() const;

	void markDirty();
	bool isDirty() const;

protected:
	void markClean();

private:
	QPointer<Mesh> m_mesh;
	bool m_isDirty;
};

}

#endif // A3DMESHCACHE_H
