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

    void setBinding(Qt::Key key, std::function<void()> callback);
    void setBinding(Qt::MouseButton button, std::function<void()> callback);

    virtual bool eventFilter(QObject*, QEvent*) override;

private:
    struct Entry {
        bool m_wasPressed;
        std::function<void()> m_callback;
    };
    std::map<Qt::Key, Entry> m_keyBindings;
    std::map<Qt::MouseButton, Entry> m_mouseBindings;
};

#endif // KEYEVENTMANAGER_H
