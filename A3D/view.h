#ifndef A3DVIEW_H
#define A3DVIEW_H

#include "A3D/common.h"
#include <QOpenGLWidget>
#include "A3D/camera.h"
#include "A3D/rendererogl.h"
#include "A3D/scene.h"

namespace A3D {

class View : public QOpenGLWidget, public CoreGLFunctions {
	Q_OBJECT
public:
	explicit View(QWidget* parent = nullptr);
	~View();

	void setRenderTimerEnabled(bool enabled);

	Camera const& camera() const;
	Camera& camera();
	void setScene(Scene*);

	Renderer* renderer();

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

signals:
	void frameRendered();

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void keyPressEvent(QKeyEvent* e) override;
	void keyReleaseEvent(QKeyEvent* e) override;

private slots:
	void sceneChanged();

private:
	void a3dInitialize();

	bool m_initDoneGL;

	Camera m_camera;
	std::unique_ptr<RendererOGL> m_renderer;
	Scene* m_scene;
	QTimer* m_renderTimer;

	std::map<Qt::Key, bool> m_keyPressed;
};

}

#endif // A3DVIEW_H
