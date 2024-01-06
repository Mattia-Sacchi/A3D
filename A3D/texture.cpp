#include "A3D/texture.h"
#include "A3D/renderer.h"

namespace A3D {

static QImage const& missingTexture() {
	static QImage imgMissingTexture;
	if(imgMissingTexture.isNull()) {
		imgMissingTexture      = QImage(8, 8, QImage::Format_RGBA8888);
		QColor const colorEven = QColor(255, 0, 255, 255);
		QColor const colorOdd  = QColor(0, 0, 0, 255);

		for(int y = 0; y < 8; ++y) {
			for(int x = 0; x < 8; ++x) {
				imgMissingTexture.setPixelColor(x, y, ((x + y) & 1) ? colorOdd : colorEven);
			}
		}
	}
	return imgMissingTexture;
}

Texture* Texture::standardTexture(StandardTexture stdTex) {
	static std::map<StandardTexture, Texture> standardTextures;

	auto it = standardTextures.find(stdTex);
	if(it != standardTextures.end())
		return &it->second;

	Texture& newTex = standardTextures[stdTex];
	switch(stdTex) {
	case MissingTexture:
	{
		newTex.setImage(missingTexture());
		newTex.setMinFilter(Nearest);
		newTex.setMagFilter(Nearest);
		newTex.setRenderOptions(NoOptions);
		newTex.setWrapMode(WrapDirectionX, Repeat);
		newTex.setWrapMode(WrapDirectionY, Repeat);
		newTex.setWrapMode(WrapDirectionZ, Repeat);
		break;
	}
	default:
		break;
	}
	newTex.invalidateCache();
	return &newTex;
}

Texture::Texture(ResourceManager* resourceManager)
	: Texture(QImage(), resourceManager) {
	log(LC_Debug, "Constructor: Texture");
}

Texture::Texture(QImage image, ResourceManager* resourceManager)
	: Resource{ resourceManager },
	  m_image(std::move(image)),
	  m_minFilter(LinearMipMapLinear),
	  m_magFilter(Linear),
	  m_lodBias(-1.f),
	  m_maxAnisotropy(8.f),
	  m_renderOptions(GenerateMipMaps) {
	log(LC_Debug, "Constructor: Texture");
}

Texture::~Texture() {
	log(LC_Debug, "Destructor: Texture (start)");
	for(auto it = m_textureCache.begin(); it != m_textureCache.end(); ++it) {
		if(it->second.isNull())
			continue;

		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			log(LC_Info, "Texture::~Texture: Potential memory leak? Renderer not available.");
			continue;
		}

		r->Delete(it->second);
	}
	log(LC_Debug, "Destructor: Texture (end)");
}

Texture::RenderOptions Texture::renderOptions() const {
	return m_renderOptions;
}
void Texture::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

QImage const& Texture::image() const {
	return m_image;
}
void Texture::setImage(QImage image) {
	m_image = std::move(image);
}

Texture::WrapMode Texture::wrapMode(WrapDirection dir) const {
	auto it = m_wrapMode.find(dir);
	if(it == m_wrapMode.end())
		return Repeat;
	return it->second;
}
void Texture::setWrapMode(WrapDirection dir, WrapMode mode) {
	m_wrapMode[dir] = mode;
}

float Texture::maxAnisotropy() const {
	return m_maxAnisotropy;
}
void Texture::setMaxAnisotropy(float maxAnisotropy) {
	m_maxAnisotropy = maxAnisotropy;
}

Texture::Filter Texture::minFilter() const {
	return m_minFilter;
}
void Texture::setMinFilter(Filter minFilter) {
	m_minFilter = minFilter;
}

Texture::Filter Texture::magFilter() const {
	return m_magFilter;
}
void Texture::setMagFilter(Filter magFilter) {
	m_magFilter = magFilter;
}

float Texture::lodBias() const {
	return m_lodBias;
}
void Texture::setLodBias(float bias) {
	m_lodBias = bias;
}

void Texture::invalidateCache(std::uintptr_t rendererID) {
	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		for(auto it = m_textureCache.begin(); it != m_textureCache.end();) {
			if(it->second.isNull())
				it = m_textureCache.erase(it);
			else {
				it->second->markDirty();
				++it;
			}
		}
	}
	else {
		auto it = m_textureCache.find(rendererID);
		if(it == m_textureCache.end())
			return;
		if(it->second.isNull())
			m_textureCache.erase(it);
		else
			it->second->markDirty();
	}
}

}
