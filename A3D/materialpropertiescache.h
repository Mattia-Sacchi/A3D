#ifndef A3DMATERIALPROPERTIESCACHE_H
#define A3DMATERIALPROPERTIESCACHE_H

#include "A3D/common.h"
#include <QObject>
#include <QPointer>

namespace A3D {

class MaterialProperties;
class MaterialPropertiesCache : public QObject {
	Q_OBJECT
public:
	explicit MaterialPropertiesCache(MaterialProperties* parent);
	~MaterialPropertiesCache();
	MaterialProperties* materialProperties() const;

	void markDirty();
	bool isDirty() const;

protected:
	void markClean();

private:
	QPointer<MaterialProperties> m_materialProperties;
	bool m_isDirty;
};

}

#endif // A3DMATERIALPROPERTIESCACHE_H
