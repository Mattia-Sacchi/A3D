#ifndef A3DTEXTURECACHEOGL_H
#define A3DTEXTURECACHEOGL_H

#include "common.h"
#include "texturecache.h"
#include <QOpenGLTexture>

namespace A3D {

class RendererOGL;
class TextureCacheOGL : public TextureCache {
	Q_OBJECT
public:
	explicit TextureCacheOGL(Texture*);
	~TextureCacheOGL();

	void update(RendererOGL*, CoreGLFunctions*);
	void applyToSlot(RendererOGL*, CoreGLFunctions*, GLuint slot);

private:
	std::unique_ptr<QOpenGLTexture> m_texture;
};

}

#endif // A3DTEXTURECACHEOGL_H
