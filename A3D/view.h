#ifndef A3DVIEW_H
#define A3DVIEW_H

#include <QOpenGLWidget>
#include "camera.h"

namespace A3D {

class Scene;
class View : public QOpenGLWidget
{
	Q_OBJECT
public:
	explicit View(QWidget *parent = nullptr);
	~View();
	
	Camera const& camera() const;
	Camera& camera();
	void setScene(Scene*);
	
private:
	bool m_initDoneGL;
	bool m_initDoneScene;
	
	Camera m_camera;
	Scene* m_scene;
};

}

#endif // A3DVIEW_H
