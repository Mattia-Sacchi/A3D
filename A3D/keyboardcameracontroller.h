#ifndef A3DKEYBOARDCAMERACONTROLLER_H
#define A3DKEYBOARDCAMERACONTROLLER_H

#include "A3D/common.h"
#include "A3D/viewcontroller.h"
#include <QObject>
#include <set>
#include <QKeyEvent>
#include <QFocusEvent>

namespace A3D {

class KeyboardCameraController : public ViewController {
	Q_OBJECT

public:
	enum Action {
		ACT_MOVE_FORWARD,
		ACT_MOVE_BACKWARD,
		ACT_MOVE_LEFT,
		ACT_MOVE_RIGHT,
		ACT_MOVE_UPWARD,
		ACT_MOVE_DOWNWARD,

		ACT_MOVE_PRECISE,
		ACT_MOVE_QUICK,

		ACT_LOOK_LEFT,
		ACT_LOOK_RIGHT,
		ACT_LOOK_UP,
		ACT_LOOK_DOWN,
		ACT_LOOK_TILTLEFT,
		ACT_LOOK_TILTRIGHT,

		ACT_LOOK_HOME,

		ACT_SHOOT_RAY,
		ACT_LOOK_MOUSE_POSITION,
		ACT_PRINT_DEBUG,

		//!
		ACT_COUNT
	};

	void lookAtMousePosition();

	explicit KeyboardCameraController(View* view);

	void setKeyBinding(Qt::Key, Action);
	void setKeyBindings(std::map<Qt::Key, Action> actions);

	void setButtonBinding(Qt::MouseButton, Action);
	void setButtonBindings(std::map<Qt::MouseButton, Action> actions);

	// When ACT_MOVE_PRECISE is pressed, any movement is multiplied by this factor
	void setPreciseMovementFactor(float = 0.50f);

	// When ACT_MOVE_QUICK is pressed, any movement is multiplied by this factor
	void setQuickMovementFactor(float = 2.0f);

	// Any movement is multiplied by this vector
	void setBaseMovementSpeed(QVector3D = QVector3D(1.f, 1.f, 1.f));

	// Any rotation is multiplied by this vector
	void setBaseRotationSpeed(QVector3D = QVector3D(1.f, 1.f, 1.f));

	void setHomePosition(QVector3D);

	virtual bool update(std::chrono::milliseconds deltaT) override;
	virtual bool eventFilter(QObject*, QEvent*) override;

private:
	void updateActions();

	bool m_actions[ACT_COUNT];
	std::map<Qt::Key, Action> m_keyBindings;
	std::map<Qt::Key, bool> m_keyStatus;

	std::map<Qt::MouseButton, Action> m_btnBindings;
	std::map<Qt::MouseButton, bool> m_btnStatus;
	QPointF m_mousePos;
	QPointF m_mouseClickPos;
	QMatrix4x4 m_cameraClickView;

	QVector3D m_movementBaseSpeed;
	float m_movementPreciseFactor;
	float m_movementQuickFactor;

	QVector3D m_rotationBaseSpeed;
	QVector3D m_homePosition;
};

}

#endif // A3DKEYBOARDCAMERACONTROLLER_H
