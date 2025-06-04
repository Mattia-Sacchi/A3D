#ifndef CUSTOMWIDGETS_H
#define CUSTOMWIDGETS_H

#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QLayout>

enum {
    MajorWidth = 4,
    MinorWidth = 2,
};

class CustomButton : public QPushButton {
    Q_OBJECT
public:
    inline explicit CustomButton(QWidget* parent = nullptr, QColor borderColor = Qt::black, size_t borderSize = 2)
        : QPushButton(parent),
          m_borderColor(borderColor),
          m_borderSize(borderSize) {

        size_t px = borderSize * 2;
        setContentsMargins(px, px, px, px);
    }

    inline void setText(QString text, QColor color) {
        QPalette pal = palette();
        pal.setColor(QPalette::WindowText, color);
        setPalette(pal);
        QPushButton::setText(text);
    }

protected:
    void paintEvent(QPaintEvent*) override {
        // Disegna prima il pulsante standard
        QStyleOptionButton option;
        initStyleOption(&option);
        QPainter painter(this);
        style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);

        // Poi disegna il bordo personalizzato sopra
        painter.setRenderHint(QPainter::Antialiasing);
        QPen pen(m_borderColor, m_borderSize);
        painter.setPen(pen);
        painter.drawRect(rect().adjusted(m_borderSize / 2, m_borderSize / 2, -m_borderSize / 2, -m_borderSize / 2));
    }

private:
    QColor m_borderColor;
    size_t m_borderSize;
};

class ColoredFrame : public QFrame {
public:
    inline explicit ColoredFrame(QWidget* parent = nullptr, QColor borderColor = Qt::black, size_t borderSize = 2)
        : QFrame(parent),
          m_borderColor(borderColor),
          m_borderSize(borderSize),
          m_label(nullptr) {
        setLayout(new QVBoxLayout);
        layout()->setSpacing(0);
        size_t px = borderSize * 2;
        layout()->setContentsMargins(px, px, px, px);
    }

    inline void setLabel(QLabel* label) {
        if(m_label) {
            layout()->removeWidget(m_label);
            delete m_label;
        }
        layout()->addWidget(m_label = label);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QFrame::paintEvent(event);

        // Disegna il bordo
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(m_borderColor, m_borderSize)); // Spessore 2px
        painter.drawRect(rect().adjusted(1, 1, -2, -2));
    }

private:
    QColor m_borderColor;
    size_t m_borderSize;
    QLabel* m_label;
};

#endif // CUSTOMWIDGETS_H
