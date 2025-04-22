#ifndef KEYEVENTMANAGER_H
#define KEYEVENTMANAGER_H
#include <map>
#include <functional>
#include <QEvent>
#include <QObject>

class KeyEventManager : public QObject {
	Q_OBJECT
public:
	KeyEventManager(QObject* parent = nullptr);

	inline void setKeyBinding(Qt::Key key, std::function<void()> fcn) { m_keyBindings[key] = { false, fcn }; }

	inline void setMouseBinding(Qt::MouseButton button, std::function<void()> fcn) { m_mouseBindings[button] = { false, fcn }; }

	virtual bool eventFilter(QObject*, QEvent*) override;

private:
	struct D {
		bool m_state;
		std::function<void()> m_function;
	};
	std::map<Qt::Key, D> m_keyBindings;
	std::map<Qt::MouseButton, D> m_mouseBindings;
};

#endif // KEYEVENTMANAGER_H
