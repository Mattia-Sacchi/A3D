#ifndef A3DMATERIALCACHEOGL_H
#define A3DMATERIALCACHEOGL_H

#include "A3D/common.h"
#include "A3D/materialcache.h"
#include <QOpenGLShaderProgram>

namespace A3D {

class MaterialCacheOGL : public MaterialCache
{
	Q_OBJECT
public:
	MaterialCacheOGL(Material*);
	
	void update(CoreGLFunctions*);
	void render(CoreGLFunctions*, QMatrix4x4 const& model, QMatrix4x4 const& view, QMatrix4x4 const& proj);
	
private:
	std::unique_ptr<QOpenGLShaderProgram> m_program;
	std::map<QString, QVariant> m_uniformValues;
	
	std::map<QString, int> m_uniformIDs;
	int m_uidMVP;
	int m_uidM;
	int m_uidV;
	int m_uidP;
};

}

#endif // A3DMATERIALCACHEOGL_H
