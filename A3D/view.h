#ifndef A3DVIEW_H
#define A3DVIEW_H

#include "A3D/common.h"
#include <QOpenGLWidget>
#include "A3D/camera.h"
#include "A3D/rendererogl.h"
#include "A3D/scene.h"
#include "A3D/viewcontroller.h"

namespace A3D {

class View : public QOpenGLWidget, public CoreGLFunctions {
	Q_OBJECT
public:
	explicit View(QWidget* parent = nullptr);
	~View();

	Camera const& camera() const;
	Camera& camera();

	Scene* scene() const;
	void setScene(Scene*);

	ViewController* controller() const;
	void setController(ViewController*);

	float runTimeMultiplier() const;
	void setRunTimeMultiplier(float);

	bool isRunning() const;
	void setRunning(bool running);

	inline void run() { setRunning(true); }
	inline void stop() { setRunning(false); }

	Renderer* renderer();

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

public slots:
	void updateView();

signals:
	void frameRendered();

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

private slots:
	void sceneChanged();

private:
	void a3dInitialize();

	bool m_initDoneGL;

	QElapsedTimer m_viewRunTimer;
	float m_runTimeMultiplier;

	QPointer<ViewController> m_viewController;
	Camera m_camera;
	std::unique_ptr<RendererOGL> m_renderer;
	QPointer<Scene> m_scene;
	QMetaObject::Connection m_sceneConnection;
};

}

#endif // A3DVIEW_H
