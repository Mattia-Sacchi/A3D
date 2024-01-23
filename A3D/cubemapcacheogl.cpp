#include "A3D/cubemapcacheogl.h"
#include "A3D/cubemap.h"
#include "A3D/material.h"
#include "A3D/rendererogl.h"
#include <QOpenGLFunctions>

namespace A3D {

CubemapCacheOGL::CubemapCacheOGL(Cubemap* parent)
	: CubemapCache{ parent },
	  m_cubemap(0),
	  m_cubemapIrradiance(0),
	  m_cubemapPrefilter(0) {
	log(LC_Debug, "Constructor: CubemapCacheOGL");
}
CubemapCacheOGL::~CubemapCacheOGL() {
	log(LC_Debug, "Destructor: CubemapCacheOGL");

	if(m_cubemap) {
		QOpenGLContext::currentContext()->functions()->glDeleteTextures(1, &m_cubemap);
		m_cubemap = 0;
	}

	if(m_cubemapIrradiance) {
		QOpenGLContext::currentContext()->functions()->glDeleteTextures(1, &m_cubemapIrradiance);
		m_cubemapIrradiance = 0;
	}

	if(m_cubemapPrefilter) {
		QOpenGLContext::currentContext()->functions()->glDeleteTextures(1, &m_cubemapPrefilter);
		m_cubemapPrefilter = 0;
	}
}

void CubemapCacheOGL::update(RendererOGL* renderer, CoreGLFunctions* gl) {
	Cubemap* c = cubemap();
	if(!c)
		return;

	if(c->nx().isNull() || c->ny().isNull() || c->nz().isNull() || c->px().isNull() || c->py().isNull() || c->pz().isNull())
		return;

	QSize s = c->nx().size();
	if(s.width() != s.height())
		return;

	if(s != c->ny().size() || s != c->nz().size() || s != c->px().size() || s != c->py().size() || s != c->pz().size())
		return;

	bool isqimage = c->nx().isQImage();
	if(isqimage != c->ny().isQImage() || isqimage != c->nz().isQImage() || isqimage != c->px().isQImage() || isqimage != c->py().isQImage() || isqimage != c->pz().isQImage())
		return;

	bool ishdr = c->nx().isHDR();
	if(ishdr != c->ny().isHDR() || ishdr != c->nz().isHDR() || ishdr != c->px().isHDR() || ishdr != c->py().isHDR() || ishdr != c->pz().isHDR())
		return;

	if(!m_cubemap)
		gl->glGenTextures(1, &m_cubemap);

	if(!m_cubemap)
		return;

	gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	static GLenum const FORMAT_FLOAT_R    = GL_R16F;
	static GLenum const FORMAT_FLOAT_RG   = GL_RG16F;
	static GLenum const FORMAT_FLOAT_RGB  = GL_RGB16F;
	static GLenum const FORMAT_FLOAT_RGBA = GL_RGBA16F;
	static GLenum const FORMAT_IRRADIANCE = GL_RGBA16F;
	static GLenum const FORMAT_PREFILTER  = GL_RGBA16F;

	auto setCubemapFace = [this, gl](Image const& i, QOpenGLTexture::CubeMapFace face) {
		if(i.isQImage()) {
			if(i.hasAlphaChannel()) {
				QImage temp;
				QImage const* glImage = imageWithFormat(QImage::Format_RGBA8888, i.qimage(), temp);

				gl->glTexImage2D(face, 0, GL_RGBA, glImage->width(), glImage->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, glImage->constBits());
			}
			else {
				QImage temp;
				QImage const* glImage = imageWithFormat(QImage::Format_RGB888, i.qimage(), temp);

				gl->glTexImage2D(face, 0, GL_RGB, glImage->width(), glImage->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, glImage->constBits());
			}
		}
		else if(i.isHDR()) {
			switch(i.hdr().nrComponents) {
			case 1:
				gl->glTexImage2D(face, 0, FORMAT_FLOAT_R, static_cast<GLsizei>(i.hdr().w), static_cast<GLsizei>(i.hdr().h), 0, GL_RED, GL_FLOAT, i.hdr().m_data.data());
				break;
			case 2:
				gl->glTexImage2D(face, 0, FORMAT_FLOAT_RG, static_cast<GLsizei>(i.hdr().w), static_cast<GLsizei>(i.hdr().h), 0, GL_RG, GL_FLOAT, i.hdr().m_data.data());
				break;
			case 3:
				gl->glTexImage2D(face, 0, FORMAT_FLOAT_RGB, static_cast<GLsizei>(i.hdr().w), static_cast<GLsizei>(i.hdr().h), 0, GL_RGB, GL_FLOAT, i.hdr().m_data.data());
				break;
			case 4:
				gl->glTexImage2D(face, 0, FORMAT_FLOAT_RGBA, static_cast<GLsizei>(i.hdr().w), static_cast<GLsizei>(i.hdr().h), 0, GL_RGBA, GL_FLOAT, i.hdr().m_data.data());
				break;
			}
		}
	};

	setCubemapFace(c->nx(), QOpenGLTexture::CubeMapNegativeX);
	setCubemapFace(c->ny(), QOpenGLTexture::CubeMapNegativeY);
	setCubemapFace(c->nz(), QOpenGLTexture::CubeMapNegativeZ);
	setCubemapFace(c->px(), QOpenGLTexture::CubeMapPositiveX);
	setCubemapFace(c->py(), QOpenGLTexture::CubeMapPositiveY);
	setCubemapFace(c->pz(), QOpenGLTexture::CubeMapPositiveZ);

	// Post-processing

	// Save the GL state that might change because of our post-processing phases
	renderer->pushState(true);
	calcIrradiance(FORMAT_IRRADIANCE, renderer, gl);
	calcPrefilter(FORMAT_PREFILTER, renderer, gl);
	renderer->popState();

	markClean();
}

void CubemapCacheOGL::calcPrefilter(GLenum format, RendererOGL* renderer, CoreGLFunctions* gl) {
	// Consider that calcIrradiance gets called with an active Framebuffer object.

	if(!m_cubemap)
		return;

	if(!m_cubemapPrefilter)
		gl->glGenTextures(1, &m_cubemapPrefilter);

	if(!m_cubemapPrefilter)
		return;

	Mesh* prefilterMesh    = Mesh::standardMesh(Mesh::CubeIndexedMesh);
	Material* prefilterMat = Material::standardMaterial(Material::PrefilterMaterial);

	MeshCacheOGL* meshCache    = renderer->buildMeshCache(prefilterMesh);
	MaterialCacheOGL* matCache = renderer->buildMaterialCache(prefilterMat);

	// Mip Levels = 5 -> Size = 128x128
	// Mip Levels = 4 -> Size =  64x64
	// Mip Levels = 3 -> Size =  32x32
	// Mip Levels = 2 -> Size =  16x16
	// Mip Levels = 1 -> Size =   8x8

	static int const maxMipLevels        = 5;
	static int const prefilterSideLength = 4 * static_cast<int>(std::powf(2.f, static_cast<float>(maxMipLevels)) + 0.1f);
	static QSize const prefilterSize(prefilterSideLength, prefilterSideLength);

	gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapPrefilter);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for(int i = 0; i < 6; ++i)
		gl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, prefilterSize.width(), prefilterSize.height(), 0, GL_RGB, GL_FLOAT, nullptr);

	gl->glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	auto lookAt = [](QVector3D const& target, QVector3D const& up) -> QMatrix4x4 {
		QMatrix4x4 mx;
		mx.lookAt(QVector3D(), target, up);
		return mx;
	};
	auto proj = []() -> QMatrix4x4 {
		QMatrix4x4 mx;
		mx.perspective(90.f, 1.f, 0.1f, 10.f);
		return mx;
	};

	static QMatrix4x4 const viewMatrices[6] = {
		lookAt(QVector3D(1.f, 0.f, 0.f), QVector3D(0.f, -1.f, 0.f)), lookAt(QVector3D(-1.f, 0.f, 0.f), QVector3D(0.f, -1.f, 0.f)),
		lookAt(QVector3D(0.f, 1.f, 0.f), QVector3D(0.f, 0.f, 1.f)),  lookAt(QVector3D(0.f, -1.f, 0.f), QVector3D(0.f, 0.f, -1.f)),
		lookAt(QVector3D(0.f, 0.f, 1.f), QVector3D(0.f, -1.f, 0.f)), lookAt(QVector3D(0.f, 0.f, -1.f), QVector3D(0.f, -1.f, 0.f)),
	};
	static QMatrix4x4 const projMatrix = proj();

	float resolution = 512.f;

	Cubemap* c = cubemap();
	if(c)
		resolution = c->nx().size().width();

	matCache->applyUniform("CubemapResolution", resolution);

	for(int i = maxMipLevels; i >= 1; --i) {
		int const glMipLevel = maxMipLevels - i;

		int const mipLevelSideLength = 4 * static_cast<int>(std::powf(2.f, static_cast<float>(i)) + 0.1f);
		QSize const mipLevelSize(mipLevelSideLength, mipLevelSideLength);

		gl->glViewport(0, 0, mipLevelSize.width(), mipLevelSize.height());

		float roughness = static_cast<float>(i - 1) / static_cast<float>(maxMipLevels - 1);
		matCache->applyUniform("Roughness", roughness);

		gl->glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(MaterialProperties::EnvironmentTextureSlot));
		gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

		matCache->install(gl);

		for(int j = 0; j < 6; ++j) {
			// Bind shader & set matrices
			gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, m_cubemapPrefilter, glMipLevel);
			gl->glClear(GL_COLOR_BUFFER_BIT);

			meshCache->render(gl, QMatrix4x4(), viewMatrices[j], projMatrix);
		}
	}
}

void CubemapCacheOGL::calcIrradiance(GLenum format, RendererOGL* renderer, CoreGLFunctions* gl) {
	// Consider that calcIrradiance gets called with an active Framebuffer object.

	if(!m_cubemap)
		return;

	if(!m_cubemapIrradiance)
		gl->glGenTextures(1, &m_cubemapIrradiance);

	if(!m_cubemapIrradiance)
		return;

	Mesh* irradianceMesh    = Mesh::standardMesh(Mesh::CubeIndexedMesh);
	Material* irradianceMat = Material::standardMaterial(Material::IrradianceMaterial);

	MeshCacheOGL* meshCache    = renderer->buildMeshCache(irradianceMesh);
	MaterialCacheOGL* matCache = renderer->buildMaterialCache(irradianceMat);

	gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapIrradiance);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	static QSize const irradianceSize(32, 32);

	for(int i = 0; i < 6; ++i)
		gl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, irradianceSize.width(), irradianceSize.height(), 0, GL_RGB, GL_FLOAT, nullptr);

	auto lookAt = [](QVector3D const& target, QVector3D const& up) -> QMatrix4x4 {
		QMatrix4x4 mx;
		mx.lookAt(QVector3D(), target, up);
		return mx;
	};
	auto proj = []() -> QMatrix4x4 {
		QMatrix4x4 mx;
		mx.perspective(90.f, 1.f, 0.1f, 10.f);
		return mx;
	};

	static QMatrix4x4 const viewMatrices[6] = {
		lookAt(QVector3D(1.f, 0.f, 0.f), QVector3D(0.f, -1.f, 0.f)), lookAt(QVector3D(-1.f, 0.f, 0.f), QVector3D(0.f, -1.f, 0.f)),
		lookAt(QVector3D(0.f, 1.f, 0.f), QVector3D(0.f, 0.f, 1.f)),  lookAt(QVector3D(0.f, -1.f, 0.f), QVector3D(0.f, 0.f, -1.f)),
		lookAt(QVector3D(0.f, 0.f, 1.f), QVector3D(0.f, -1.f, 0.f)), lookAt(QVector3D(0.f, 0.f, -1.f), QVector3D(0.f, -1.f, 0.f)),
	};
	static QMatrix4x4 const projMatrix = proj();

	gl->glViewport(0, 0, irradianceSize.width(), irradianceSize.height());
	gl->glDisable(GL_DEPTH_TEST);
	gl->glDisable(GL_CULL_FACE);

	gl->glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(MaterialProperties::EnvironmentTextureSlot));
	gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

	matCache->install(gl);

	for(int i = 0; i < 6; ++i) {
		// Bind shader & set matrices
		gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cubemapIrradiance, 0);
		gl->glClear(GL_COLOR_BUFFER_BIT);

		meshCache->render(gl, QMatrix4x4(), viewMatrices[i], projMatrix);
	}
}

void CubemapCacheOGL::applyToSlot(CoreGLFunctions* gl, GLint environmentSlot, GLint irradianceSlot, GLint prefilterSlot) {
	if(!m_cubemap)
		return;

	if(environmentSlot >= 0) {
		gl->glActiveTexture(GL_TEXTURE0 + environmentSlot);
		gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
	}

	if(irradianceSlot >= 0) {
		gl->glActiveTexture(GL_TEXTURE0 + irradianceSlot);
		gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapIrradiance);
	}

	if(prefilterSlot >= 0) {
		gl->glActiveTexture(GL_TEXTURE0 + prefilterSlot);
		gl->glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapPrefilter);
	}
}

}
