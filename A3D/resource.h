#ifndef A3DRESOURCE_H
#define A3DRESOURCE_H

#include "common.h"
#include <QObject>
#include "resourcemanager.h"

namespace A3D {

class Resource : public QObject {
	Q_OBJECT
public:
	explicit Resource(ResourceManager*);

	ResourceManager* resourceManager() const;

private:
	ResourceManager* m_resourceManager;
};

}

#endif // A3DRESOURCE_H
