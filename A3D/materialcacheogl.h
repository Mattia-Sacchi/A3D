#ifndef A3DMATERIALCACHEOGL_H
#define A3DMATERIALCACHEOGL_H

#include "A3D/common.h"
#include "A3D/materialcache.h"
#include "A3D/materialproperties.h"
#include "A3D/texturecacheogl.h"
#include <QOpenGLShaderProgram>

namespace A3D {

class MaterialCacheOGL : public MaterialCache {
	Q_OBJECT
public:
	explicit MaterialCacheOGL(Material*);
	~MaterialCacheOGL();

	void update(CoreGLFunctions*);
	void install(CoreGLFunctions*, MaterialProperties const&, QMatrix4x4 const& model, QMatrix4x4 const& view, QMatrix4x4 const& proj);

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
};

}

#endif // A3DMATERIALCACHEOGL_H
