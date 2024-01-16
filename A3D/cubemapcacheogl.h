#ifndef A3DCUBEMAPCACHEOGL_H
#define A3DCUBEMAPCACHEOGL_H

#include "A3D/common.h"
#include "A3D/cubemapcache.h"
#include <QOpenGLTexture>

namespace A3D {

class CubemapCacheOGL : public CubemapCache {
	Q_OBJECT
public:
	explicit CubemapCacheOGL(Cubemap*);
	~CubemapCacheOGL();

	void update(CoreGLFunctions*);
	void applyToSlot(CoreGLFunctions*, GLuint slot);

private:
	std::unique_ptr<QOpenGLTexture> m_cubemap;
};

}

#endif // A3DCUBEMAPCACHEOGL_H
