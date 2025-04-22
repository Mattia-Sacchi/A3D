#include "keyeventmanager.h"
#include <QMouseEvent>
#include <QKeyEvent>

KeyEventManager::KeyEventManager(QObject* parent)
	: QObject(parent),
	  m_keyBindings({}),
	  m_mouseBindings({}) {
	parent->installEventFilter(this);
}

bool KeyEventManager::eventFilter(QObject* o, QEvent* e) {
	QEvent::Type eType = e->type();

	if(eType == QEvent::KeyPress || eType == QEvent::KeyRelease) {
		QKeyEvent* ke = static_cast<QKeyEvent*>(e);

		Qt::Key currentKey = static_cast<Qt::Key>(ke->key());
		if(m_keyBindings.find(currentKey) == m_keyBindings.end())
			return QObject::eventFilter(o, e);
		if(eType == QEvent::KeyRelease && m_keyBindings[currentKey].m_state)
			m_keyBindings[currentKey].m_function();

		m_keyBindings[currentKey].m_state = eType == QEvent::KeyPress;
	}
	else if(eType == QEvent::MouseButtonPress || eType == QEvent::MouseButtonRelease) {
		QMouseEvent* me               = static_cast<QMouseEvent*>(e);
		Qt::MouseButton currentButton = static_cast<Qt::MouseButton>(me->button());
		if(m_mouseBindings.find(currentButton) == m_mouseBindings.end())
			return QObject::eventFilter(o, e);
		if(eType == QEvent::MouseButtonRelease && m_mouseBindings[currentButton].m_state)
			m_mouseBindings[currentButton].m_function();

		m_mouseBindings[currentButton].m_state = eType == QEvent::MouseButtonPress;
	}

	return QObject::eventFilter(o, e);
}
