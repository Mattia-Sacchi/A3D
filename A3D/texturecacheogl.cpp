#include "A3D/texturecacheogl.h"
#include "A3D/texture.h"

namespace A3D {

TextureCacheOGL::TextureCacheOGL(Texture* parent)
	: TextureCache{ parent } {
	log(LC_Debug, "Constructor: TextureCacheOGL");
}

TextureCacheOGL::~TextureCacheOGL() {
	log(LC_Debug, "Destructor: TextureCacheOGL");
}

inline QOpenGLTexture::WrapMode translateWrapMode(Texture::WrapMode wm) {
	switch(wm) {
	default:
	case Texture::Repeat:
		return QOpenGLTexture::Repeat;
	case Texture::MirroredRepeat:
		return QOpenGLTexture::MirroredRepeat;
	case Texture::Clamp:
		return QOpenGLTexture::ClampToEdge;
	}
}
inline QOpenGLTexture::Filter translateFilter(Texture::Filter f) {
	switch(f) {
	default:
	case Texture::Nearest:
		return QOpenGLTexture::Nearest;
	case Texture::Linear:
		return QOpenGLTexture::Linear;
	case Texture::NearestMipMapNearest:
		return QOpenGLTexture::NearestMipMapNearest;
	case Texture::NearestMipMapLinear:
		return QOpenGLTexture::NearestMipMapLinear;
	case Texture::LinearMipMapNearest:
		return QOpenGLTexture::LinearMipMapNearest;
	case Texture::LinearMipMapLinear:
		return QOpenGLTexture::LinearMipMapLinear;
	}
}
void TextureCacheOGL::update(CoreGLFunctions*) {
	Texture* t = texture();
	if(!t || t->image().isNull()) {
		m_texture.reset();
		return;
	}

	m_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
	m_texture->setWrapMode(QOpenGLTexture::DirectionS, translateWrapMode(t->wrapMode(Texture::WrapDirectionX)));
	m_texture->setWrapMode(QOpenGLTexture::DirectionT, translateWrapMode(t->wrapMode(Texture::WrapDirectionY)));
	// m_texture->setWrapMode(QOpenGLTexture::DirectionR, translateWrapMode(t->wrapMode(Texture::WrapDirectionZ)));

	m_texture->setMaximumAnisotropy(t->maxAnisotropy());
	m_texture->setMinMagFilters(translateFilter(t->minFilter()), translateFilter(t->magFilter()));
	m_texture->setLevelofDetailBias(t->lodBias());

	m_texture->setData(t->image(), (t->renderOptions() & Texture::GenerateMipMaps) ? QOpenGLTexture::GenerateMipMaps : QOpenGLTexture::DontGenerateMipMaps);

	markClean();
}

void TextureCacheOGL::applyToSlot(CoreGLFunctions* gl, GLuint slot) {
	if(!m_texture)
		return;
	gl->glActiveTexture(GL_TEXTURE0 + slot);
	gl->glBindTexture(GL_TEXTURE_2D, m_texture->textureId());
}

}
