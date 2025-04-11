#include "A3D/keyboardcameracontroller.h"
#include "A3D/view.h"

namespace A3D {

KeyboardCameraController::KeyboardCameraController(View* view)
	: ViewController{ view },
	  m_keyBindings{
		{Qt::Key_W, ACT_MOVE_FORWARD},
		{Qt::Key_S, ACT_MOVE_BACKWARD},
		{Qt::Key_A, ACT_MOVE_LEFT},
		{Qt::Key_D, ACT_MOVE_RIGHT},
		{Qt::Key_Q, ACT_MOVE_UPWARD},
		{Qt::Key_Z, ACT_MOVE_DOWNWARD},
		  
		{Qt::Key_Shift, ACT_MOVE_QUICK},
		{Qt::Key_Control, ACT_MOVE_PRECISE},
		  
		{Qt::Key_Left, ACT_LOOK_LEFT},		  
		{Qt::Key_Right, ACT_LOOK_RIGHT},		  
		{Qt::Key_Up, ACT_LOOK_UP},		  
		{Qt::Key_Down, ACT_LOOK_DOWN},
		  
		{Qt::Key_H, ACT_LOOK_HOME}
	  },
	  m_movementBaseSpeed(1.f, 1.f, 1.f),
	  m_movementPreciseFactor(0.2f),
	  m_movementQuickFactor(5.f),
	  m_rotationBaseSpeed(60.f, 60.f, 60.f)
{
	std::memset(m_actions, 0, sizeof(m_actions));
}

void KeyboardCameraController::setKeyBinding(Qt::Key k, Action a) {
	m_keyBindings[k] = a;
	updateActions();
}

void KeyboardCameraController::setKeyBindings(std::map<Qt::Key, Action> actions) {
	m_keyBindings = std::move(actions);
	updateActions();
}

void KeyboardCameraController::setButtonBinding(Qt::MouseButton b, Action a) {
	m_btnBindings[b] = a;
	updateActions();
}

void KeyboardCameraController::setButtonBindings(std::map<Qt::MouseButton, Action> actions) {
	m_btnBindings = std::move(actions);
	updateActions();
}

void KeyboardCameraController::setPreciseMovementFactor(float factor) {
	m_movementPreciseFactor = factor;
}

void KeyboardCameraController::setQuickMovementFactor(float factor) {
	m_movementQuickFactor = factor;
}

void KeyboardCameraController::setBaseMovementSpeed(QVector3D speed) {
	m_movementBaseSpeed = speed;
}

void KeyboardCameraController::setBaseRotationSpeed(QVector3D speed) {
	m_rotationBaseSpeed = speed;
}

bool KeyboardCameraController::update(std::chrono::milliseconds deltaT) {
	if(!view())
		return false;

	QVector3D movement;
	QVector3D rotation;

	if(m_actions[ACT_MOVE_FORWARD])
		movement.setZ(movement.z() + 1.f);
	if(m_actions[ACT_MOVE_BACKWARD])
		movement.setZ(movement.z() - 1.f);

	if(m_actions[ACT_MOVE_RIGHT])
		movement.setX(movement.x() + 1.f);
	if(m_actions[ACT_MOVE_LEFT])
		movement.setX(movement.x() - 1.f);

	if(m_actions[ACT_MOVE_UPWARD])
		movement.setY(movement.y() + 1.f);
	if(m_actions[ACT_MOVE_DOWNWARD])
		movement.setY(movement.y() - 1.f);

	if(m_actions[ACT_MOVE_PRECISE])
		movement *= m_movementPreciseFactor;
	if(m_actions[ACT_MOVE_QUICK])
		movement *= m_movementQuickFactor;

	if(m_actions[ACT_LOOK_LEFT])
		rotation.setY(rotation.y() - 1.f);
	if(m_actions[ACT_LOOK_RIGHT])
		rotation.setY(rotation.y() + 1.f);

	if(m_actions[ACT_LOOK_UP])
		rotation.setX(rotation.x() - 1.f);
	if(m_actions[ACT_LOOK_DOWN])
		rotation.setX(rotation.x() + 1.f);

	if(m_actions[ACT_LOOK_TILTLEFT])
		rotation.setZ(rotation.z() - 1.f);
	if(m_actions[ACT_LOOK_TILTRIGHT])
		rotation.setZ(rotation.z() + 1.f);

	if(m_actions[ACT_SHOOT_RAY]) {
		// Work in progress
	}
	if(m_actions[ACT_LOOK_MOUSE_POS]) {
		// Work in progress
	}

	if(m_actions[ACT_PRINT_DEBUG]) {
		QPoint pos = QCursor::pos();

		qDebug() << pos.x();
		qDebug() << pos.y();
	}

	movement *= m_movementBaseSpeed;
	rotation *= m_rotationBaseSpeed;

	if(movement.isNull() && rotation.isNull())
		return false;

	float const deltaT_Factor = static_cast<float>(deltaT.count()) / 1000.f;
	movement *= deltaT_Factor;
	rotation *= deltaT_Factor;

	Camera& c = view()->camera();

	c.offsetOrientation(rotation);

	QVector3D const forward = c.forward();
	QVector3D const right   = c.right();
	QVector3D const up      = c.up();

	c.offsetPosition((forward * movement.z()) + (right * movement.x()) + (up * movement.y()));

	if(m_actions[ACT_LOOK_HOME])
		c.setOrientationTarget(m_homePosition);

	return true;
}

bool KeyboardCameraController::eventFilter(QObject* o, QEvent* e) {
	QEvent::Type eType = e->type();
	if(eType == QEvent::KeyPress || eType == QEvent::KeyRelease) {
		QKeyEvent* ke                                = static_cast<QKeyEvent*>(e);
		m_keyStatus[static_cast<Qt::Key>(ke->key())] = eType == QEvent::KeyPress;
		updateActions();
		if(view())
			view()->updateView();
	}
	else if(eType == QEvent::MouseButtonPress || eType == QEvent::MouseButtonRelease) {
		QMouseEvent* me             = static_cast<QMouseEvent*>(e);
		Qt::MouseButton mouseButton = me->button();

		if(m_btnBindings.find(mouseButton) == m_btnBindings.end())
			return QObject::eventFilter(o, e);

		//if(view() && !view()->hasFocus())
		//return QObject::eventFilter(o, e);

		// Single Shot Click evaluation
		bool currentIsRelease = eType == QEvent::MouseButtonRelease;
		bool lastIsPress      = m_btnLastEvent[mouseButton] == QEvent::MouseButtonPress;
		if(lastIsPress)
			qDebug() << "Press";
		if(currentIsRelease)
			qDebug() << "Release";

		if(lastIsPress && currentIsRelease) {
			m_btnStatus[mouseButton] = true;
			updateActions();
			if(view())
				view()->updateView();
			m_btnStatus[mouseButton] = false;
			updateActions();
		}

		// Set the last event
		m_btnLastEvent[mouseButton] = eType;
	}
	else if(e->type() == QEvent::FocusOut) {
		m_btnStatus.clear();
		m_keyStatus.clear();
		std::memset(m_actions, 0, sizeof(m_actions));
	}

	return QObject::eventFilter(o, e);
}

void KeyboardCameraController::updateActions() {
	std::memset(m_actions, 0, sizeof(m_actions));

	for(auto it = m_keyBindings.begin(); it != m_keyBindings.end(); ++it) {
		auto keyStatus = m_keyStatus.find(it->first);
		if(keyStatus == m_keyStatus.end() || !keyStatus->second)
			continue;

		m_actions[it->second] = true;
	}

	for(auto it = m_btnBindings.begin(); it != m_btnBindings.end(); ++it) {
		auto btnStatus = m_btnStatus.find(it->first);
		if(btnStatus == m_btnStatus.end() || !btnStatus->second)
			continue;

		m_actions[it->second] = true;
	}
}

}
