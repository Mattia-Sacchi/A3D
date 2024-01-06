#include "A3D/material.h"
#include "A3D/renderer.h"
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
		newMat.setShaderFile(GLSL, VertexShader, ":/A3D/Basic2DMaterial.vert");
		newMat.setShaderFile(GLSL, FragmentShader, ":/A3D/Basic2DMaterial.frag");
		break;
	case Basic3DMaterial:
		newMat.setShaderFile(GLSL, VertexShader, ":/A3D/Basic3DMaterial.vert");
		newMat.setShaderFile(GLSL, FragmentShader, ":/A3D/Basic3DMaterial.frag");
		break;
	case SampleTranslucentMaterial:
		newMat.setShaderFile(GLSL, VertexShader, ":/A3D/SampleTranslucentMaterial.vert");
		newMat.setShaderFile(GLSL, FragmentShader, ":/A3D/SampleTranslucentMaterial.frag");
		newMat.setRenderOptions(newMat.renderOptions() | Translucent);
		break;
	case TexturedTranslucentMaterial:
		newMat.setRenderOptions(Translucent);
		newMat.setShaderFile(GLSL, VertexShader, ":/A3D/TexturedMaterial.vert");
		newMat.setShaderFile(GLSL, FragmentShader, ":/A3D/TexturedMaterial.frag");
		break;
	case TexturedMaterial:
		newMat.setShaderFile(GLSL, VertexShader, ":/A3D/TexturedMaterial.vert");
		newMat.setShaderFile(GLSL, FragmentShader, ":/A3D/TexturedMaterial.frag");
		break;
	}
	newMat.invalidateCache();
	return &newMat;
}

Material::Material(ResourceManager* resourceManager)
	: Resource{ resourceManager },
	  m_renderOptions(NoOptions) {
	log(LC_Debug, "Constructor: Material");
}

Material::~Material() {
	log(LC_Debug, "Destructor: Material (start)");
	for(auto it = m_materialCache.begin(); it != m_materialCache.end(); ++it) {
		if(it->second.isNull())
			continue;

		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			log(LC_Info, "Material::~Material: Potential memory leak? Renderer not available.");
			continue;
		}

		r->Delete(it->second);
	}
	log(LC_Debug, "Destructor: Material (end)");
}

Material* Material::clone() const {
	Material* newMaterial        = new Material(resourceManager());
	newMaterial->m_renderOptions = m_renderOptions;
	newMaterial->m_shaders       = m_shaders;
	return newMaterial;
}

Material::RenderOptions Material::renderOptions() const {
	return m_renderOptions;
}
void Material::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

void Material::setShader(ShaderMode mode, ShaderType type, QString shaderContents) {
	m_shaders[mode][type] = std::move(shaderContents);
}

void Material::setShaderFile(ShaderMode mode, ShaderType type, QString shaderPath) {
	QFile f(shaderPath);
	if(!f.open(QFile::ReadOnly))
		return;
	setShader(mode, type, QString::fromUtf8(f.readAll()));
}

QString Material::shader(ShaderMode mode, ShaderType type) const {
	auto itSM = m_shaders.find(mode);
	if(itSM == m_shaders.end())
		return QString();

	std::map<ShaderType, QString> const& shaderCodes = itSM->second;

	auto itSD = shaderCodes.find(type);
	if(itSD == shaderCodes.end())
		return QString();

	return itSD->second;
}

void Material::invalidateCache(std::uintptr_t rendererID) {
	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		for(auto it = m_materialCache.begin(); it != m_materialCache.end();) {
			if(it->second.isNull())
				it = m_materialCache.erase(it);
			else {
				it->second->markDirty();
				++it;
			}
		}
	}
	else {
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
