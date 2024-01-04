#include "material.h"
#include "renderer.h"
#include <QDebug>
#include <QFile>
#include <utility>

namespace A3D {

Material* Material::standardMaterial(StandardMaterial stdMat) {
	static std::map<StandardMaterial, Material> standardMaterials;
	
	auto it = standardMaterials.find(stdMat);
	if(it != standardMaterials.end())
		return &it->second;
	
	Material& newMat = standardMaterials[stdMat];
	switch(stdMat) {
	case Basic2DMaterial:
		newMat.setShaderFile(GLSL, VertexShader,   ":/A3D/Basic2DMaterial.vert");
		newMat.setShaderFile(GLSL, FragmentShader, ":/A3D/Basic2DMaterial.frag");
		break;
	case Basic3DMaterial:
		newMat.setShaderFile(GLSL, VertexShader,   ":/A3D/Basic3DMaterial.vert");
		newMat.setShaderFile(GLSL, FragmentShader, ":/A3D/Basic3DMaterial.frag");
		break;
	}
	newMat.invalidateMaterialCache();
	return &newMat;
}

Material::Material(QObject *parent)
	: QObject{parent}
{
	dbgConstruct("Material")
}

Material::~Material() {
	dbgDestruct("Material started")
	for(auto it = m_materialCache.begin(); it != m_materialCache.end(); ++it) {
		if(it->second.isNull())
			continue;
		
		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			qDebug() << "Material::~Material: Potential memory leak? Renderer not available.";
			continue;
		}
		
		r->Delete(it->second);
	}
	dbgDestruct("Material finished")
}

void Material::setShader(ShaderMode mode, ShaderType type, QString shaderContents)
{
	m_shaders[mode].m_shaderCodes[type] = std::move(shaderContents);
}

void Material::setShaderFile(ShaderMode mode, ShaderType type, QString shaderPath)
{
	QFile f(shaderPath);
	if(!f.open(QFile::ReadOnly))
		return;
	setShader(mode, type, QString::fromUtf8(f.readAll()));
}

void Material::setShaderValue(ShaderMode mode, QString name, QVariant value)
{
	m_shaders[mode].m_values[std::move(name)] = std::move(value);
}

QString Material::shader(ShaderMode mode, ShaderType type) const
{
	auto itSM = m_shaders.find(mode);
	if(itSM == m_shaders.end())
		return QString();
	
	ShaderData const& sd = itSM->second;
	
	auto itSD = sd.m_shaderCodes.find(type);
	if(itSD == sd.m_shaderCodes.end())
		return QString();
	
	return itSD->second;
}

QVariant Material::shaderValue(ShaderMode mode, QString const& name) const
{
	auto itSM = m_shaders.find(mode);
	if(itSM == m_shaders.end())
		return QVariant();
	
	ShaderData const& sd = itSM->second;
	std::map<QString, QVariant> const& sv = sd.m_values;
	
	auto itSV = sv.find(name);
	if(itSV == sv.end())
		return QVariant();
	
	return itSV->second;
}

std::map<QString, QVariant> Material::shaderValues(ShaderMode mode) const {
	auto itSM = m_shaders.find(mode);
	if(itSM == m_shaders.end())
		return std::map<QString, QVariant>{};
	
	ShaderData const& sd = itSM->second;
	std::map<QString, QVariant> const& sv = sd.m_values;
	return sv;
}

std::map<QString, QVariant>& Material::shaderValues(ShaderMode mode) {
	return m_shaders[mode].m_values;
}

void Material::invalidateMaterialCache(std::uintptr_t rendererID) {
	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		for(auto it = m_materialCache.begin(); it != m_materialCache.end();) {
			if(it->second.isNull())
				it = m_materialCache.erase(it);
			else {
				it->second->markDirty();
				++it;
			}
		}
	} else {
		auto it = m_materialCache.find(rendererID);
		if(it == m_materialCache.end())
			return;
		if(it->second.isNull())
			m_materialCache.erase(it);
		else
			it->second->markDirty();
	}
}

}
