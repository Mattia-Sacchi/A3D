#ifndef A3DCUBEMAPCACHEOGL_H
#define A3DCUBEMAPCACHEOGL_H

#include "A3D/common.h"
#include "A3D/cubemapcache.h"
#include <QOpenGLTexture>

namespace A3D {

class RendererOGL;
class CubemapCacheOGL : public CubemapCache {
	Q_OBJECT
public:
	explicit CubemapCacheOGL(Cubemap*);
	~CubemapCacheOGL();

	void update(RendererOGL*, CoreGLFunctions*);
	void applyToSlot(CoreGLFunctions*, GLint environmentSlot, GLint irradianceSlot, GLint prefilterSlot);

private:
	void calcIrradiance(GLenum format, RendererOGL*, CoreGLFunctions*);
	void calcPrefilter(GLenum format, RendererOGL*, CoreGLFunctions*);

	GLuint m_cubemap;
	GLuint m_cubemapIrradiance;
	GLuint m_cubemapPrefilter;
};

}

#endif // A3DCUBEMAPCACHEOGL_H
