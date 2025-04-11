#include "resource.h"

namespace A3D {

Resource::Resource(ResourceManager* resourceManager)
	: QObject{ resourceManager },
	  m_resourceManager(resourceManager) {}

ResourceManager* Resource::resourceManager() const {
	return m_resourceManager;
}

}
