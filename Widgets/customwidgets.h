#ifndef CUSTOMWIDGETS_H
#define CUSTOMWIDGETS_H

#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QLayout>
#include <QFormLayout>
#include "chartaxisgeneralsettings.h"

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

        size_t px = borderSize * 4;
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

        // Apply padding by adjusting the rect
        //int padding = m_borderSize * 2; // or customize left/right/top/bottom individually
        //option.rect = option.rect.adjusted(padding, padding, -padding, -padding);

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

class CustomFrame : public QFrame {
    Q_OBJECT
public:
    inline explicit CustomFrame(QWidget* parent = nullptr, QColor borderColor = Qt::black, size_t borderSize = 2)
        : QFrame(parent),
          m_borderColor(borderColor),
          m_borderSize(borderSize) {

        QFont font;
        font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(FR_High));
        m_numberLabel.setFont(font);
        QFormLayout* lay = new QFormLayout;
        setLayout(lay);
        layout()->setSpacing(0);
        size_t px = borderSize * 2;
        layout()->setContentsMargins(px, px, px, px);
        lay->addRow(&m_numberLabel, &m_label);
    }

    inline void setBorder(QColor color, size_t borderSize) {
        size_t px = borderSize * 2;
        layout()->setContentsMargins(px, px, px, px);
        m_borderColor = color;
        m_borderSize  = borderSize;
        update();
    }

    inline void setText(QString text) { m_label.setText(text); }
    inline void setNumber(size_t n) { m_numberLabel.setText(QString::number(n, 10).append(":  ")); }

    inline QLabel& label() { return m_label; }

    inline void setValues(QString text, size_t n) {
        setText(text);
        setNumber(n);
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
    QLabel m_numberLabel;
    QLabel m_label;
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
