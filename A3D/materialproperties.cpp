#include "materialproperties.h"

namespace A3D {

MaterialProperties::MaterialProperties() {}

QVariant MaterialProperties::value(Material::ShaderMode targetMode, QString name, QVariant fallback) {
	auto msValues = m_modeSpecificValues.find(targetMode);
	if(msValues != m_modeSpecificValues.end()) {
		std::map<QString, QVariant> const& mValues = msValues->second;
		auto val = mValues.find(name);
		if(val != mValues.end())
			return val->second;
	}
	
	auto val = m_values.find(name);
	if(val != m_values.end())
		return val->second;
	
	return fallback;
}

void MaterialProperties::setValue(QString name, QVariant value) {
	m_values[std::move(name)] = std::move(value);
}

void MaterialProperties::setValue(Material::ShaderMode targetMode, QString name, QVariant value) {
	m_modeSpecificValues[std::move(targetMode)][std::move(name)] = std::move(value);
}

std::map<QString, QVariant> const& MaterialProperties::values() const {
	return m_values;
}
std::map<QString, QVariant>& MaterialProperties::values() {
	return m_values;
}

std::map<QString, QVariant> const& MaterialProperties::modeSpecificValues(Material::ShaderMode targetMode) const {
	static std::map<QString, QVariant> const emptyReference;
	auto it = m_modeSpecificValues.find(targetMode);
	if(it == m_modeSpecificValues.end())
		return emptyReference;
	
	return it->second;
}

std::map<QString, QVariant>& MaterialProperties::modeSpecificValues(Material::ShaderMode targetMode) {
	return m_modeSpecificValues[targetMode];
}

}
