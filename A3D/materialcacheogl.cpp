#include "materialcacheogl.h"
#include "material.h"
#include <QColor>
#include <QTransform>

namespace A3D {


MaterialCacheOGL::MaterialCacheOGL(Material* parent)
	: MaterialCache{parent}
{}

void MaterialCacheOGL::render(CoreGLFunctions*, QMatrix4x4 const& model, QMatrix4x4 const& view, QMatrix4x4 const& proj) {
	if(!m_program)
		return;
	
	Material* m = material();
	if(!m)
		return;
	
	m_program->bind();
	
	std::map<QString, QVariant> const& values = m->shaderValues(Material::GLSL);
	if(values != m_uniformValues) {
		m_uniformValues = values;
		for(auto it = m_uniformValues.begin(); it != m_uniformValues.end(); ++it) {
			QString const& name = it->first;
			QVariant const& v = it->second;
			
			int uniformLocation = -1;
			
			auto cachedEntry = m_uniformIDs.find(name);
			if(cachedEntry == m_uniformIDs.end()) {
				uniformLocation = m_program->uniformLocation(name);
				m_uniformIDs[name] = uniformLocation;
			} else {
				uniformLocation = cachedEntry->second;
			}
			
			if(uniformLocation == -1)
				continue;
			
			switch(v.userType()) {
			case QMetaType::Double:
			case QMetaType::Float:
				m_program->setUniformValue(uniformLocation, v.toFloat());
				break;
			case QMetaType::LongLong:
			case QMetaType::Long:
			case QMetaType::Int:
			case QMetaType::Short:
				m_program->setUniformValue(uniformLocation, static_cast<GLint>(v.toInt()));
				break;
			case QMetaType::ULongLong:
			case QMetaType::ULong:
			case QMetaType::UInt:
			case QMetaType::UShort:
				m_program->setUniformValue(uniformLocation, static_cast<GLuint>(v.toUInt()));
				break;
				
#define BIND_MT(metaType, cppType) case metaType: m_program->setUniformValue(uniformLocation, v.value<cppType>()); break
				
			BIND_MT(QMetaType::QColor, QColor);
			BIND_MT(QMetaType::QPoint, QPoint);
			BIND_MT(QMetaType::QPointF, QPointF);
			BIND_MT(QMetaType::QSize, QSize);
			BIND_MT(QMetaType::QSizeF, QSizeF);
			BIND_MT(QMetaType::QMatrix4x4, QMatrix4x4);
			BIND_MT(QMetaType::QTransform, QTransform);
			BIND_MT(QMetaType::QVector2D, QVector2D);
			BIND_MT(QMetaType::QVector3D, QVector3D);
			BIND_MT(QMetaType::QVector4D, QVector4D);
				
#undef BIND_MT
			}
		}
	}
	
	if(m_uidMVP != -1)
		m_program->setUniformValue(m_uidMVP, (proj * view * model));
	
	if(m_uidM != -1)
		m_program->setUniformValue(m_uidM, model);
	
	if(m_uidV != -1)
		m_program->setUniformValue(m_uidV, view);
	
	if(m_uidP != -1)
		m_program->setUniformValue(m_uidP, proj);
}

void MaterialCacheOGL::update(CoreGLFunctions*) {
	Material* m = material();
	if(!m)
		return;
	
	QString vxShader = m->shader(Material::GLSL, Material::VertexShader);
	QString fxShader = m->shader(Material::GLSL, Material::FragmentShader);
	
	if(vxShader.isEmpty() || fxShader.isEmpty())
		return;
	
	if(!m_program)
		m_program = std::make_unique<QOpenGLShaderProgram>();
	
	// TODO: Evaluate caching?
	
	if(!vxShader.isEmpty())
		m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vxShader);
	if(!fxShader.isEmpty())
		m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fxShader);
	m_program->link();
	
	m_uniformIDs.clear();
	m_uidMVP = m_program->uniformLocation("mvpMatrix");
	m_uidM = m_program->uniformLocation("modelMatrix");
	m_uidV = m_program->uniformLocation("viewMatrix");
	m_uidP = m_program->uniformLocation("projMatrix");
	
	markClean();
}

}
