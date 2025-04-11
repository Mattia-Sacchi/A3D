#ifndef A3DLINEGROUPCACHE_H
#define A3DLINEGROUPCACHE_H

#include "common.h"
#include <QObject>

namespace A3D {

class LineGroup;
class LineGroupCache : public QObject {
	Q_OBJECT
public:
	explicit LineGroupCache(LineGroup* parent = nullptr);
	~LineGroupCache();

	LineGroup* lineGroup() const;

	void markDirty();
	bool isDirty() const;

protected:
	void markClean();

private:
	QPointer<LineGroup> m_lineGroup;
	bool m_isDirty;
};

}

#endif // A3DLINEGROUPCACHE_H
