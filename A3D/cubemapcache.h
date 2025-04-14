#ifndef A3DCUBEMAPCACHE_H
#define A3DCUBEMAPCACHE_H

#include "common.h"
#include <QObject>

namespace A3D {

class Cubemap;
class CubemapCache : public QObject {
	Q_OBJECT
public:
	explicit CubemapCache(Cubemap* parent = nullptr);
	~CubemapCache();

	Cubemap* cubemap() const;

	void markDirty();
	bool isDirty() const;

protected:
	void markClean();

private:
	QPointer<Cubemap> m_cubemap;
	bool m_isDirty;
};

}

#endif // A3DCUBEMAPCACHE_H
