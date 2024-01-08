#include "A3D/materialpropertiescacheogl.h"
#include "A3D/materialcacheogl.h"
#include "A3D/rendererogl.h"
#include <QOpenGLFunctions>

namespace A3D {

MaterialPropertiesCacheOGL::MaterialPropertiesCacheOGL(MaterialProperties* parent)
	: MaterialPropertiesCache{ parent } {
	log(LC_Debug, "Constructor: MaterialPropertiesCacheOGL");
}
MaterialPropertiesCacheOGL::~MaterialPropertiesCacheOGL() {
	log(LC_Debug, "Destructor: MaterialPropertiesCacheOGL");

	if(m_materialUBO != 0)
		QOpenGLContext::currentContext()->functions()->glDeleteBuffers(1, &m_materialUBO);
}

void MaterialPropertiesCacheOGL::install(CoreGLFunctions* gl, MaterialCacheOGL* materialCache) {
	if(!m_materialUBO)
		return;

	MaterialProperties* matProp = materialProperties();
	if(matProp && materialCache) {
		materialCache->applyUniforms(matProp->rawValues());
	}

	gl->glBindBufferBase(GL_UNIFORM_BUFFER, RendererOGL::UBO_MaterialPropertiesBinding, m_materialUBO);
}

void MaterialPropertiesCacheOGL::update(CoreGLFunctions* gl) {
	MaterialProperties* p = materialProperties();
	if(!p)
		return;

	MaterialProperties::HighLevelProperties& hlProps = p->highLevelProperties();
	m_materialUBO_data.opacity                       = hlProps.opacity;

	if(!m_materialUBO) {
		gl->glGenBuffers(1, &m_materialUBO);

		if(m_materialUBO) {
			gl->glBindBuffer(GL_UNIFORM_BUFFER, m_materialUBO);
			gl->glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUBO_Data), &m_materialUBO_data, GL_DYNAMIC_DRAW);
			gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}
	else {
		gl->glBindBuffer(GL_UNIFORM_BUFFER, m_materialUBO);
		gl->glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialUBO_Data), &m_materialUBO_data);
		gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	markClean();
}

}
