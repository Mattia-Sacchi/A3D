#ifndef A3DMATERIALPROPERTIES_H
#define A3DMATERIALPROPERTIES_H

#include "A3D/common.h"
#include <QObject>
#include "A3D/material.h"
#include "A3D/resource.h"
#include "A3D/materialpropertiescache.h"

namespace A3D {

class MaterialProperties : public Resource {
	Q_OBJECT
public:
	struct HighLevelProperties {
		inline HighLevelProperties()
			: opacity(1.f) {}
		float opacity;
	};

	enum TextureSlot {
		AlbedoTextureSlot,
		NormalTextureSlot,
		MetallicTextureSlot,
		RoughnessTextureSlot,
		AOTextureSlot,
		EnvMapTextureSlot,

		MaxTextures = 8,

		// Aliases
		CubeMapTextureSlot = EnvMapTextureSlot,
	};

	MaterialProperties(ResourceManager* = nullptr);
	~MaterialProperties();

	MaterialProperties* clone() const;

	Texture* texture(TextureSlot slot) const;
	void setTexture(Texture*, TextureSlot slot);

	HighLevelProperties& highLevelProperties();
	HighLevelProperties const& highLevelProperties() const;

	QVariant rawValue(QString name, QVariant fallback = QVariant());
	void setRawValue(QString name, QVariant value);

	std::map<QString, QVariant> const& rawValues() const;
	std::map<QString, QVariant>& rawValues();

	void setAlwaysTranslucent(bool always);
	bool isTranslucent() const;

	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	template <typename T>
	T* getMaterialPropertiesCacheT(std::uintptr_t rendererID) const {
		auto it = m_materialPropertiesCache.find(rendererID);
		if(it == m_materialPropertiesCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}
	template <typename T>
	std::pair<T*, bool> getOrEmplaceMaterialPropertiesCache(std::uintptr_t rendererID) {
		auto it = m_materialPropertiesCache.find(rendererID);
		if(it == m_materialPropertiesCache.end() || it->second.isNull()) {
			T* c                                  = new T(this);
			m_materialPropertiesCache[rendererID] = QPointer<MaterialPropertiesCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for MaterialProperties.");

		return std::make_pair(c, false);
	}

private:
	bool m_alwaysTranslucent;
	HighLevelProperties m_hlValues;
	std::map<QString, QVariant> m_rawValues;
	QPointer<Texture> m_textures[MaxTextures];

	std::map<std::uintptr_t, QPointer<MaterialPropertiesCache>> m_materialPropertiesCache;
};

}

#endif // A3DMATERIALPROPERTIES_H
