#ifndef A3DMATERIALCACHEOGL_H
#define A3DMATERIALCACHEOGL_H

#include "A3D/common.h"
#include "A3D/materialcache.h"
#include "A3D/materialproperties.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

namespace A3D {

class MaterialPropertiesCacheOGL;
class MaterialCacheOGL : public MaterialCache {
	Q_OBJECT
public:
	explicit MaterialCacheOGL(Material*);
	~MaterialCacheOGL();

	void update(CoreGLFunctions*);
	void install(CoreGLFunctions*);

	int searchUniform(QString const& name);
	void applyUniform(QString const& name, QVariant const& value);
	void applyUniforms(std::map<QString, QVariant> const& uniforms);

private:
	std::unique_ptr<QOpenGLShaderProgram> m_program;

	struct UniformCachedInfo {
		inline UniformCachedInfo()
			: m_uniformID(-1) {}

		int m_uniformID;
		QVariant m_lastValue;
	};

	std::map<QString, UniformCachedInfo> m_uniformCachedInfo;

	GLuint m_meshUBO_index;
	GLuint m_matpropUBO_index;
};

}

#endif // A3DMATERIALCACHEOGL_H
