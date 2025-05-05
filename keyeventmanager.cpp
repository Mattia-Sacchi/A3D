#include "keyeventmanager.h"
#include <QMouseEvent>
#include <QKeyEvent>

KeyEventManager::KeyEventManager(QObject* parent)
	: QObject(parent),
	  m_keyBindings({}),
	  m_mouseBindings({}) {
	parent->installEventFilter(this);
}

void KeyEventManager::setBinding(Qt::Key key, std::function<void()> callback) {
	m_keyBindings[key] = Entry{ false, callback };
}

void KeyEventManager::setBinding(Qt::MouseButton button, std::function<void()> callback) {
	m_mouseBindings[button] = Entry{ false, callback };
}

bool KeyEventManager::eventFilter(QObject* o, QEvent* e) {
	QEvent::Type eType = e->type();

	if(eType == QEvent::KeyPress || eType == QEvent::KeyRelease) {
		QKeyEvent* ke = static_cast<QKeyEvent*>(e);

		Qt::Key const currentKey = static_cast<Qt::Key>(ke->key());
		auto itFunctor           = m_keyBindings.find(currentKey);

		if(itFunctor == m_keyBindings.end() || !itFunctor->second.m_callback)
			return QObject::eventFilter(o, e);

		if(eType == QEvent::KeyPress && !itFunctor->second.m_wasPressed) {
			itFunctor->second.m_wasPressed = true;
			itFunctor->second.m_callback();
		}
		else {
			itFunctor->second.m_wasPressed = false;
		}
	}
	else if(eType == QEvent::MouseButtonPress || eType == QEvent::MouseButtonRelease) {
		QMouseEvent* me = static_cast<QMouseEvent*>(e);

		Qt::MouseButton const currentButton = static_cast<Qt::MouseButton>(me->button());

		auto itFunctor = m_mouseBindings.find(currentButton);

		if(itFunctor == m_mouseBindings.end() || !itFunctor->second.m_callback)
			return QObject::eventFilter(o, e);

		if(eType == QEvent::MouseButtonPress && !itFunctor->second.m_wasPressed) {
			itFunctor->second.m_wasPressed = true;
			itFunctor->second.m_callback();
		}
		else {
			itFunctor->second.m_wasPressed = false;
		}
	}

	return QObject::eventFilter(o, e);
}
