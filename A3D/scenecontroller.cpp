#include "scenecontroller.h"
#include "scene.h"

namespace A3D {

SceneController::SceneController(Scene* scene)
	: QObject{ scene },
	  m_scene(scene) {}

Scene* SceneController::scene() const {
	return m_scene;
}

}
