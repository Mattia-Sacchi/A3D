#ifndef A3DTEXTURECACHEOGL_H
#define A3DTEXTURECACHEOGL_H

#include "A3D/common.h"
#include "A3D/texturecache.h"
#include <QOpenGLTexture>

namespace A3D {

class TextureCacheOGL : public TextureCache {
	Q_OBJECT
public:
	explicit TextureCacheOGL(Texture*);
	~TextureCacheOGL();

	void update(CoreGLFunctions*);
	void applyToSlot(CoreGLFunctions*, GLuint slot);

private:
	std::unique_ptr<QOpenGLTexture> m_texture;
};

}

#endif // A3DTEXTURECACHEOGL_H
