#ifndef A3DSCENECONTROLLER_H
#define A3DSCENECONTROLLER_H

#include "A3D/common.h"
#include <QObject>

namespace A3D {

class Scene;
class SceneController : public QObject {
	Q_OBJECT
public:
	explicit SceneController(Scene* scene);

	// Return true if anything changed.
	virtual bool update(std::chrono::milliseconds deltaT) = 0;

protected:
	Scene* scene() const;

private:
	QPointer<Scene> m_scene;
};

}

#endif // A3DSCENECONTROLLER_H
