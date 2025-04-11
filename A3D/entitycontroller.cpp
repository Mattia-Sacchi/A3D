#include "entitycontroller.h"
#include "entity.h"

namespace A3D {

EntityController::EntityController(Entity* entity)
	: QObject{ entity },
	  m_entity(entity) {}

Entity* EntityController::entity() const {
	return m_entity;
}

}
