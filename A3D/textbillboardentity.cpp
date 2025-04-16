#include "textbillboardentity.h"

namespace A3D {

TextBillboardEntity::TextBillboardEntity(Entity* parent)
	: Entity(parent),
	  m_text(),
	  m_font(),
	  m_color(Qt::black),
	  m_textureDirty(true),
	  m_group(nullptr),
	  m_texture(nullptr),
	  m_fontMetrics(m_font) {

	setModel(new Model(this));

	m_group = model()->getOrAddGroup("Text");
	m_group->setMesh(Mesh::standardMesh(Mesh::UnitQuadMesh));
	m_group->setMaterial(Material::standardMaterial(Material::BillboardMaterial));

	m_texture = new Texture(nullptr);
	m_texture->setParent(this);

	MaterialProperties* materialProperties = new MaterialProperties(nullptr);
	materialProperties->setParent(this);
	m_group->setMaterialProperties(materialProperties);

	materialProperties->setTexture(m_texture, MaterialProperties::AlbedoTextureSlot);
}

void TextBillboardEntity::setText(QString text) {
	if(m_text != text) {
		m_text         = std::move(text);
		m_textureDirty = true;
	}
}

void TextBillboardEntity::setFont(QFont font) {
	if(m_font != font) {
		m_font         = std::move(font);
		m_fontMetrics  = QFontMetrics(m_font);
		m_textureDirty = true;
	}
}

void TextBillboardEntity::setColor(QColor color) {
	if(color != m_color) {
		m_color        = std::move(color);
		m_textureDirty = true;
	}
}

QString TextBillboardEntity::text() const {
	return m_text;
}

QFont TextBillboardEntity::font() const {
	return m_font;
}

QColor TextBillboardEntity::color() const {
	return m_color;
}

bool TextBillboardEntity::updateEntity(std::chrono::milliseconds ms) {
	if(m_textureDirty) {
		refresh();
		Entity::updateEntity(ms);
		return true;
	}
	else {
		return Entity::updateEntity(ms);
	}
}

void TextBillboardEntity::refresh() {
	if(!m_textureDirty)
		return;

	m_textureDirty = false;

	if(m_text.isEmpty()) {
		m_image = QImage(1, 1, QImage::Format_RGBA8888);
		m_image.fill(Qt::transparent);
		m_texture->setImage(std::move(m_image));
		m_texture->invalidateCache();
		return;
	}

	qreal const pixelsWide = std::max<qreal>(m_fontMetrics.horizontalAdvance(m_text), 1.0);
	qreal const pixelsTall = std::max<qreal>(m_fontMetrics.height(), 1.0);

	m_image = QImage(qCeil(pixelsWide), qCeil(pixelsTall), QImage::Format_RGBA8888);
	m_image.fill(Qt::transparent);

	m_painter.begin(&m_image);
	m_painter.setFont(m_font);
	m_painter.setPen(m_color);
	m_painter.drawText(QRect(0, 0, m_image.width(), m_image.height()), Qt::AlignCenter, m_text);
	m_painter.end();

	m_texture->setImage(std::move(m_image));

	m_group->setScale(QVector3D(pixelsWide / pixelsTall, 1.f, 1.f));

	m_texture->invalidateCache();
}
}
