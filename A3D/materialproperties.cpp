#include "A3D/materialproperties.h"

namespace A3D {

MaterialProperties::MaterialProperties() {}

MaterialProperties& MaterialProperties::append(MaterialProperties const& other, bool overwrite) {
	if(overwrite) {
		for(auto it = other.m_values.begin(); it != other.m_values.end(); ++it) {
			m_values[it->first] = it->second;
		}
		for(auto itSM = other.m_modeSpecificValues.begin(); itSM != other.m_modeSpecificValues.end(); ++itSM) {
			std::map<QString, QVariant>& msv = m_modeSpecificValues[itSM->first];
			for(auto it = itSM->second.begin(); it != itSM->second.end(); ++it) {
				msv[it->first] = it->second;
			}
		}
	}
	else {
		for(auto it = other.m_values.begin(); it != other.m_values.end(); ++it) {
			auto existsIt = m_values.find(it->first);
			if(existsIt != m_values.end())
				continue;
			m_values[it->first] = it->second;
		}
		for(auto itSM = other.m_modeSpecificValues.begin(); itSM != other.m_modeSpecificValues.end(); ++itSM) {
			std::map<QString, QVariant>& msv = m_modeSpecificValues[itSM->first];
			for(auto it = itSM->second.begin(); it != itSM->second.end(); ++it) {
				auto existsIt = msv.find(it->first);
				if(existsIt != msv.end())
					continue;
				msv[it->first] = it->second;
			}
		}
	}
	return *this;
}

QVariant MaterialProperties::value(Material::ShaderMode targetMode, QString name, QVariant fallback) {
	auto msValues = m_modeSpecificValues.find(targetMode);
	if(msValues != m_modeSpecificValues.end()) {
		std::map<QString, QVariant> const& mValues = msValues->second;
		auto val                                   = mValues.find(name);
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
