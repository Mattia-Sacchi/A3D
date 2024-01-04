#ifndef A3DSCENE_H
#define A3DSCENE_H

#include "A3D/common.h"
#include <QObject>
#include "A3D/entity.h"

namespace A3D {

class Scene : public Entity
{
	Q_OBJECT
public:
	explicit Scene(QObject *parent = nullptr);
	~Scene();
};

}

#endif // A3DSCENE_H
