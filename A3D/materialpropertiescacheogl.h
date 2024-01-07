#ifndef A3DMATERIALPROPERTIESCACHEOGL_H
#define A3DMATERIALPROPERTIESCACHEOGL_H

#include "A3D/common.h"
#include "A3D/materialproperties.h"
#include "A3D/materialpropertiescache.h"
#include <QOpenGLBuffer>

namespace A3D {

class MaterialPropertiesCacheOGL : public MaterialPropertiesCache {
	Q_OBJECT
public:
	explicit MaterialPropertiesCacheOGL(MaterialProperties*);
	~MaterialPropertiesCacheOGL();

	void update(CoreGLFunctions*);
	void install(CoreGLFunctions*);
};

}

#endif // A3DMATERIALPROPERTIESCACHEOGL_H
