#ifndef A3DSCENE_H
#define A3DSCENE_H

#include "A3D/common.h"
#include <QObject>
#include "A3D/entity.h"
#include "A3D/resourcemanager.h"

namespace A3D {

class Scene : public Entity {
	Q_OBJECT
public:
	explicit Scene(QObject* parent = nullptr);
	~Scene();

	ResourceManager& resourceManager();
	ResourceManager const& resourceManager() const;

private:
	ResourceManager m_resourceManager;
};

}

#endif // A3DSCENE_H
