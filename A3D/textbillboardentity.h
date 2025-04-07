#ifndef TEXTBILLBOARDENTITY_H
#define TEXTBILLBOARDENTITY_H
#include "A3D/common.h"
#include "A3D/entity.h"
#include "A3D/texture.h"
#include <QPainter>
#include <QFont>
#include <QColor>

namespace A3D {

class TextBillboardEntity : public Entity {
	Q_OBJECT
public:
	TextBillboardEntity(Entity* parent = nullptr);

	void setText(QString text);
	void setFont(QFont font);
	void setColor(QColor color);

	QString text() const;
	QFont font() const;
	QColor color() const;

private:
	void refresh();

	QString m_text;
	QFont m_font;
	QColor m_color;

	Group* m_group;
	Texture* m_texture;
	QFontMetrics m_fontMetrics;
	QPainter m_painter;
	QImage m_image;
};
}

#endif // TEXTBILLBOARDENTITY_H
