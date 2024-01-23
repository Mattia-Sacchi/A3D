#ifndef A3DENTITYCONTROLLER_H
#define A3DENTITYCONTROLLER_H

#include "A3D/common.h"
#include <QObject>

namespace A3D {

class Entity;
class EntityController : public QObject {
	Q_OBJECT
public:
	explicit EntityController(Entity* entity);

	// Return true if anything changed.
	virtual bool update(std::chrono::milliseconds deltaT) = 0;

protected:
	Entity* entity() const;

private:
	QPointer<Entity> m_entity;
};

}

#endif // A3DENTITYCONTROLLER_H
