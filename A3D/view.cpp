#include "view.h"
#include "scene.h"
#include <QTimer>
#include <QKeyEvent>

#ifdef _DEBUG
#	include <QOpenGLDebugLogger>
#endif

namespace A3D {

View::View(QWidget* parent)
	: QOpenGLWidget{ parent },
	  m_initDoneGL(false),
	  m_renderer(nullptr),
	  m_scene(nullptr) {
	log(LC_Debug, u"Constructor: View");
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
#ifdef _DEBUG
	fmt.setOption(QSurfaceFormat::DebugContext);
#endif
	setFormat(fmt);
}

View::~View() {
	log(LC_Debug, u"Destructor: View");
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

Scene* View::scene() const {
	return m_scene;
}

void View::setScene(Scene* newScene) {
	if(m_sceneConnection) {
		disconnect(m_sceneConnection);
		m_sceneConnection = QMetaObject::Connection();
	}

	m_scene = newScene;

	if(m_scene)
		m_sceneConnection = connect(m_scene, &Scene::sceneUpdated, this, &View::sceneChanged);
}

QMatrix4x4 View::calculateFullMatrix(Entity const* targetEntity, Model const* targetModel, Group const* targetGroup) const {
	if(!scene() || !targetEntity)
		return QMatrix4x4();

	// Search for Scene within targetEntity's parents
	std::list<Entity const*> entityTree;
	do {
		entityTree.push_front(targetEntity);
		if(targetEntity == scene())
			break;
		targetEntity = targetEntity->parentEntity();
	}
	while(targetEntity);

	QMatrix4x4 compositeMatrix;
	for(auto it = entityTree.begin(); it != entityTree.end(); ++it) {
		compositeMatrix *= ((*it)->entityMatrix());
	}

	if(targetModel)
		compositeMatrix *= targetModel->modelMatrix();

	if(targetGroup)
		compositeMatrix *= targetGroup->groupMatrix();

	return compositeMatrix;
}

void View::addController(ViewController* viewController) {
	removeController(viewController);

	installEventFilter(viewController);
	m_viewControllers.emplace_back(viewController);
}

void View::removeController(ViewController* controller) {
	removeEventFilter(controller);

	for(auto it = m_viewControllers.begin(); it != m_viewControllers.end();) {
		if(!*it || *it == controller)
			it = m_viewControllers.erase(it);
		else
			++it;
	}
}

bool View::isAutoRefreshEnabled() const {
	return m_refreshTimer.isValid();
}

void View::setAutoRefreshEnabled(bool enabled) {
	if(m_refreshTimer.isValid() == enabled)
		return;

	if(enabled)
		m_refreshTimer.restart();
	else
		m_refreshTimer.invalidate();
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
		log(LC_Debug, u"initializeOpenGLFunctions failed.");
		log(LC_Fatal, u"OpenGL 3.3 Core is required to run this software.");
		return;
	}

#ifdef _DEBUG
	QOpenGLDebugLogger* logger = new QOpenGLDebugLogger(this);
	connect(logger, &QOpenGLDebugLogger::messageLogged, this, [](const QOpenGLDebugMessage& msg) {
		qWarning() << msg;
	});
	logger->initialize(); // starts logging
#endif

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

	m_renderer->DrawAll(m_scene, camera());
	m_renderer->CleanupRenderCache();

	emit frameRendered();
}

void View::updateView() {
	std::chrono::milliseconds t = std::chrono::milliseconds(m_refreshTimer.restart());

	bool hasChanges = false;
	for(auto it = m_viewControllers.begin(); it != m_viewControllers.end();) {
		if(!*it)
			it = m_viewControllers.erase(it);
		else {
			hasChanges = (*it)->update(t) || hasChanges;
			++it;
		}
	}

	Q_UNUSED(hasChanges)

	this->update(); // Redraw
}

QPointF View::toNormalizedPoint(QPointF viewLocalCoordinate) const {
	return QPointF(viewLocalCoordinate.x() / static_cast<float>(width()), viewLocalCoordinate.y() / static_cast<float>(height()));
}

}
