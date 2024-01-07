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
	void install(CoreGLFunctions*, MaterialPropertiesCacheOGL*, QMatrix4x4 const& model, QMatrix4x4 const& view, QMatrix4x4 const& proj);

private:
	int searchUniform(QString const& name);
	void applyUniform(QString const& name, QVariant const& value);
	void applyUniforms(std::map<QString, QVariant> const& uniforms);
	std::unique_ptr<QOpenGLShaderProgram> m_program;

	struct UniformCachedInfo {
		inline UniformCachedInfo()
			: m_uniformID(-1) {}

		int m_uniformID;
		QVariant m_lastValue;
	};

	std::map<QString, UniformCachedInfo> m_uniformCachedInfo;

	struct UBOShaderData {
		QVector4D diffuse;
		QVector4D ambient;
		QVector4D specular;
		QVector4D emissive;
		float opacity;
		float specularExponent;
		float paddingA;
		float paddingB;
	};

	MaterialProperties::HighLevelProperties m_lastUboReferenceData;
	QOpenGLBuffer m_uboBuffer;
	UBOShaderData m_uboData;
};

}

#endif // A3DMATERIALCACHEOGL_H
