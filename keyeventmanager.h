#ifndef KEYEVENTMANAGER_H
#define KEYEVENTMANAGER_H
#include <map>
#include <functional>
#include <QEvent>
#include <QObject>


typedef std::function<void(QEvent::Type)> EmFunctor;
class KeyEventManager : public QObject {
    Q_OBJECT
public:
    KeyEventManager(QObject* parent = nullptr);

    void setBinding(Qt::Key key, EmFunctor callback);
    void setBinding(Qt::MouseButton button, EmFunctor callback);

    virtual bool eventFilter(QObject*, QEvent*) override;

private:
    struct Entry {
        bool m_wasPressed;
        EmFunctor m_callback;
    };
    std::map<Qt::Key, Entry> m_keyBindings;
    std::map<Qt::MouseButton, Entry> m_mouseBindings;
};

#endif // KEYEVENTMANAGER_H
