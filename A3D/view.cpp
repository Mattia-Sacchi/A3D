#include "A3D/view.h"
#include "A3D/scene.h"
#include <QDebug>
#include <QTimer>
#include <QKeyEvent>

namespace A3D {

View::View(QWidget* parent)
	: QOpenGLWidget{ parent },
	  m_initDoneGL(false),
	  m_renderer(nullptr),
	  m_scene(nullptr),
	  m_renderTimer(nullptr) {
	log(LC_Debug, "Constructor: View");
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_AlwaysStackOnTop);

	QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
	fmt.setSamples(4);
	fmt.setDepthBufferSize(24);
	fmt.setVersion(3, 3);
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setAlphaBufferSize(8);
	fmt.setSwapInterval(1);
	fmt.setSwapBehavior(QSurfaceFormat::TripleBuffer);
	setFormat(fmt);

	m_renderTimer = new QTimer(this);
	connect(m_renderTimer, &QTimer::timeout, this, [this]() {
		this->update();
	});
}

View::~View() {
	log(LC_Debug, "Destructor: View");
}

void View::setRenderTimerEnabled(bool enabled) {
	if(!enabled) {
		m_renderTimer->stop();
		return;
	}
	else {
		if(format().swapInterval() >= 1)
			m_renderTimer->setInterval(0);
		else
			m_renderTimer->setInterval(16);
		m_renderTimer->start();
	}
}

Camera& View::camera() {
	return m_camera;
}

Camera const& View::camera() const {
	return m_camera;
}

Renderer* View::renderer() {
	return m_renderer.get();
}

void View::setScene(Scene* newScene) {
	m_scene = newScene;
}

QSize View::minimumSizeHint() const {
	return QSize(50, 50);
}

QSize View::sizeHint() const {
	return QSize(240, 160);
}

void View::initializeGL() {
	if(m_initDoneGL)
		return;

	if(!initializeOpenGLFunctions()) {
		log(LC_Debug, "initializeOpenGLFunctions failed.");
		log(LC_Fatal, "OpenGL 3.3 Core is required to run this software.");
		return;
	}

	m_renderer   = std::make_unique<RendererOGL>(context(), this);
	m_initDoneGL = true;
}

void View::resizeGL(int w, int h) {
	if(!m_initDoneGL)
		return;

	// Update the Camera
	if(m_camera.projectionMode() == Camera::PM_PERSPECTIVE) {
		float fWidth  = static_cast<float>(w);
		float fHeight = static_cast<float>(h);
		if(fWidth <= 1.f)
			fWidth = 1.f;
		if(fHeight <= 1.f)
			fHeight = 1.f;
		m_camera.setPerspective(45.f, fWidth / fHeight);
	}
}

void View::sceneChanged() {
	update();
}

void View::paintGL() {
	if(!m_initDoneGL)
		return;

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	static float fVal = 0.f;
	fVal += 1.f / 60.f;
	if(fVal > 1.f)
		fVal = 0.f;
	glClearColor(1.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Don't get fooled by the X/Y names....
	// x is the pitch (vertical angle)
	// y is the yaw (horizontal angle)
	// z is the roll (tilt/peek angle)
	float const fRotSpeed = 0.6f;
	QVector3D orientation;
	if(m_keyPressed[Qt::Key_W])
		orientation.setX(orientation.x() - fRotSpeed);
	if(m_keyPressed[Qt::Key_S])
		orientation.setX(orientation.x() + fRotSpeed);
	if(m_keyPressed[Qt::Key_A])
		orientation.setY(orientation.y() - fRotSpeed);
	if(m_keyPressed[Qt::Key_D])
		orientation.setY(orientation.y() + fRotSpeed);

	camera().offsetOrientation(orientation);

	// Coordinate Convention is RHS/CCW:
	// X = -Left / +Right
	// Y = +Up / -Down
	// Z = -Further / +Closer

	QVector3D const forward = camera().forward();
	QVector3D const right   = camera().right();
	QVector3D const up      = camera().up();

	float const fMovSpeed = 0.2f;
	QVector3D movement;
	if(m_keyPressed[Qt::Key_Up] && m_keyPressed[Qt::Key_Shift])
		movement += up * fMovSpeed;
	if(m_keyPressed[Qt::Key_Down] && m_keyPressed[Qt::Key_Shift])
		movement -= up * fMovSpeed;
	if(m_keyPressed[Qt::Key_Up] && !m_keyPressed[Qt::Key_Shift])
		movement += forward * fMovSpeed;
	if(m_keyPressed[Qt::Key_Down] && !m_keyPressed[Qt::Key_Shift])
		movement -= forward * fMovSpeed;
	if(m_keyPressed[Qt::Key_Left])
		movement -= right * fMovSpeed;
	if(m_keyPressed[Qt::Key_Right])
		movement += right * fMovSpeed;

	camera().setPosition(camera().position() + movement);

	if(m_keyPressed[Qt::Key_H])
		camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));

	m_renderer->DrawAll(m_scene, camera());
	m_renderer->CleanupRenderCache();

	emit frameRendered();
}

void View::keyPressEvent(QKeyEvent* e) {
	QOpenGLWidget::keyPressEvent(e);
	m_keyPressed[static_cast<Qt::Key>(e->key())] = true;
}

void View::keyReleaseEvent(QKeyEvent* e) {
	QOpenGLWidget::keyPressEvent(e);
	m_keyPressed[static_cast<Qt::Key>(e->key())] = false;
}

}
