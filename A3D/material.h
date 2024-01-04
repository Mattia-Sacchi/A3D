#ifndef A3DMATERIAL_H
#define A3DMATERIAL_H

#include "A3D/common.h"
#include <QObject>
#include <QVariant>
#include <map>
#include <cstdint>
#include <stdexcept>
#include "A3D/materialcache.h"

namespace A3D {

class Material : public QObject
{
	Q_OBJECT
public:
	enum StandardMaterial {
		Basic2DMaterial,
		Basic3DMaterial
	};
	static Material* standardMaterial(StandardMaterial);
	
	enum ShaderMode {
		GLSL
	};
	
	enum ShaderType {
		VertexShader,
		FragmentShader
	};
	
	explicit Material(QObject *parent = nullptr);
	~Material();
	
	void setShader(ShaderMode mode, ShaderType type, QString shaderContents);
	void setShaderFile(ShaderMode mode, ShaderType type, QString shaderPath);
	
	void setShaderValue(ShaderMode mode, QString name, QVariant value);
	
	QString shader(ShaderMode mode, ShaderType type) const;
	QVariant shaderValue(ShaderMode mode, QString const& name) const;
	
	std::map<QString, QVariant>& shaderValues(ShaderMode mode);	
	std::map<QString, QVariant> shaderValues(ShaderMode mode) const;
	
	void invalidateMaterialCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());
	
	template <typename T>
	T* getMaterialCacheT(std::uintptr_t rendererID) const {
		auto it = m_materialCache.find(rendererID);
		if(it == m_materialCache.end() || it->second.isNull())
			return nullptr;
		
		return qobject_cast<T*>(it->second.get());
	}
	template <typename T>
	std::pair<T*, bool> getOrEmplaceMaterialCache(std::uintptr_t rendererID) {
		auto it = m_materialCache.find(rendererID);
		if(it == m_materialCache.end() || it->second.isNull()) {
			T* c = new T(this);
			m_materialCache[rendererID] = QPointer<MaterialCache>(c);
			return std::make_pair(c, true);
		}
		
		T* c = qobject_cast<T*>(it->second.get());
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for Material.");
		
		return std::make_pair(c, false);
	}
	
private:
	struct ShaderData {
		std::map<ShaderType, QString> m_shaderCodes;
		std::map<QString, QVariant> m_values;
	};
	
	std::map<ShaderMode, ShaderData> m_shaders;
	std::map<std::uintptr_t, QPointer<MaterialCache>> m_materialCache;
};

}

#endif // A3DMATERIAL_H
