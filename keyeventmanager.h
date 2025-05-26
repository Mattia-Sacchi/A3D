#ifndef KEYEVENTMANAGER_H
#define KEYEVENTMANAGER_H
#include <map>
#include <functional>
#include <QEvent>
#include <QObject>

#include "A3D/viewcontroller.h"

class KeyEventManager : public A3D::ViewController {
	Q_OBJECT
public:
	typedef std::function<void(QEvent::Type)> EventFunctor;

	explicit KeyEventManager(A3D::View* parent);

	void setBinding(Qt::Key key, EventFunctor callback);
	void setBinding(Qt::MouseButton button, EventFunctor callback);

	virtual bool eventFilter(QObject*, QEvent*) override;

private:
	struct Entry {
		bool m_wasPressed;
		EventFunctor m_callback;
	};
	std::map<Qt::Key, Entry> m_keyBindings;
	std::map<Qt::MouseButton, Entry> m_mouseBindings;
};

#endif // KEYEVENTMANAGER_H
