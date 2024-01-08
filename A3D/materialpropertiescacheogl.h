#ifndef A3DMATERIALPROPERTIESCACHEOGL_H
#define A3DMATERIALPROPERTIESCACHEOGL_H

#include "A3D/common.h"
#include "A3D/materialproperties.h"
#include "A3D/materialpropertiescache.h"
#include <QOpenGLBuffer>

namespace A3D {

class MaterialCacheOGL;
class MaterialPropertiesCacheOGL : public MaterialPropertiesCache {
	Q_OBJECT
public:
	explicit MaterialPropertiesCacheOGL(MaterialProperties*);
	~MaterialPropertiesCacheOGL();

	void update(CoreGLFunctions*);
	void install(CoreGLFunctions*, MaterialCacheOGL*);

private:
	struct MaterialUBO_Data {
		QVector4D diffuse;
		QVector4D ambient;
		QVector4D specular;
		QVector4D emissive;
		float opacity;
		float specularExponent;
	} m_materialUBO_data;

	GLuint m_materialUBO;
};

}

#endif // A3DMATERIALPROPERTIESCACHEOGL_H
