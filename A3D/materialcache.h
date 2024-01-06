#ifndef A3DMATERIALCACHE_H
#define A3DMATERIALCACHE_H

#include "A3D/common.h"
#include <QObject>
#include <QPointer>

namespace A3D {

class Material;
class MaterialCache : public QObject {
	Q_OBJECT
public:
	explicit MaterialCache(Material* parent);
	~MaterialCache();
	Material* material() const;

	void markDirty();
	bool isDirty() const;

protected:
	void markClean();

private:
	QPointer<Material> m_material;
	bool m_isDirty;
};

}
#endif // A3dMATERIALCACHE_H
