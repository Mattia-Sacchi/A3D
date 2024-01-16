#include "A3D/cubemapcacheogl.h"
#include "A3D/cubemap.h"
#include <QOpenGLPixelTransferOptions>

namespace A3D {

CubemapCacheOGL::CubemapCacheOGL(Cubemap* parent)
	: CubemapCache{ parent } {
	log(LC_Debug, "Constructor: CubemapCacheOGL");
}
CubemapCacheOGL::~CubemapCacheOGL() {
	log(LC_Debug, "Destructor: CubemapCacheOGL");
}

void CubemapCacheOGL::update(CoreGLFunctions*) {
	Cubemap* c = cubemap();
	if(!c) {
		m_cubemap.reset();
		return;
	}

	if(c->nx().isNull() || c->ny().isNull() || c->nz().isNull() || c->px().isNull() || c->py().isNull() || c->pz().isNull()) {
		m_cubemap.reset();
		return;
	}

	QSize s = c->nx().size();
	if(s != c->ny().size()
	   || s != c->nz().size()
	   || s != c->px().size()
	   || s != c->py().size()
	   || s != c->pz().size()) {
		m_cubemap.reset();
		return;
	}

	m_cubemap = std::make_unique<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);
	m_cubemap->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
	m_cubemap->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);

	m_cubemap->setMaximumAnisotropy(4.f);
	m_cubemap->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);

	m_cubemap->setFormat(QOpenGLTexture::RGBA8_UNorm);
	m_cubemap->setSize(s.width(), s.height());
	m_cubemap->setMipLevels(m_cubemap->maximumMipLevels());
	m_cubemap->allocateStorage();

	auto setCubemapFace = [this](QImage const& i, QOpenGLTexture::CubeMapFace face) {
		QImage glImage = i.convertToFormat(QImage::Format_RGBA8888);
		QOpenGLPixelTransferOptions uploadOptions;
		uploadOptions.setAlignment(1);
		m_cubemap->setData(0, 0, face, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage.bits(), &uploadOptions);
	};

	setCubemapFace(c->nx(), QOpenGLTexture::CubeMapNegativeX);
	setCubemapFace(c->ny(), QOpenGLTexture::CubeMapNegativeY);
	setCubemapFace(c->nz(), QOpenGLTexture::CubeMapNegativeZ);
	setCubemapFace(c->px(), QOpenGLTexture::CubeMapPositiveX);
	setCubemapFace(c->py(), QOpenGLTexture::CubeMapPositiveY);
	setCubemapFace(c->pz(), QOpenGLTexture::CubeMapPositiveZ);

	markClean();
}

void CubemapCacheOGL::applyToSlot(CoreGLFunctions* gl, GLuint slot) {
	if(!m_cubemap)
		return;

	gl->glActiveTexture(GL_TEXTURE0 + slot);
	gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap->textureId());
}

}
