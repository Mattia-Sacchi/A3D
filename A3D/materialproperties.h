#ifndef A3DMATERIALPROPERTIES_H
#define A3DMATERIALPROPERTIES_H

#include "A3D/common.h"
#include "A3D/material.h"

namespace A3D {

class MaterialProperties {
public:
	MaterialProperties();
	MaterialProperties(MaterialProperties const&)            = default;
	MaterialProperties(MaterialProperties&&)                 = default;
	MaterialProperties& operator=(MaterialProperties const&) = default;
	MaterialProperties& operator=(MaterialProperties&&)      = default;

	QVariant value(Material::ShaderMode targetMode, QString name, QVariant fallback = QVariant());
	void setValue(QString name, QVariant value);
	void setValue(Material::ShaderMode targetMode, QString name, QVariant value);

	std::map<QString, QVariant> const& values() const;
	std::map<QString, QVariant>& values();

	std::map<QString, QVariant> const& modeSpecificValues(Material::ShaderMode targetMode) const;
	std::map<QString, QVariant>& modeSpecificValues(Material::ShaderMode targetMode);

private:
	std::map<QString, QVariant> m_values;
	std::map<Material::ShaderMode, std::map<QString, QVariant>> m_modeSpecificValues;
};

}

#endif // A3DMATERIALPROPERTIES_H
