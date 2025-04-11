#include "materialpropertiescacheogl.h"
#include "materialcacheogl.h"
#include "rendererogl.h"
#include <QOpenGLFunctions>

namespace A3D {

MaterialPropertiesCacheOGL::MaterialPropertiesCacheOGL(MaterialProperties* parent)
	: MaterialPropertiesCache{ parent } {
	log(LC_Debug, "Constructor: MaterialPropertiesCacheOGL");
}
MaterialPropertiesCacheOGL::~MaterialPropertiesCacheOGL() {
	log(LC_Debug, "Destructor: MaterialPropertiesCacheOGL");

	if(m_materialUBO) {
		QOpenGLContext::currentContext()->functions()->glDeleteBuffers(1, &m_materialUBO);
		m_materialUBO = 0;
	}
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

void MaterialPropertiesCacheOGL::update(RendererOGL*, CoreGLFunctions* gl) {
	MaterialProperties* p = materialProperties();
	if(!p)
		return;

	m_materialUBO_data.placeholder = QVector4D();

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
