#ifndef A3DTEXTURECACHE_H
#define A3DTEXTURECACHE_H

#include "A3D/common.h"
#include <QObject>

namespace A3D {

class Texture;
class TextureCache : public QObject {
	Q_OBJECT
public:
	explicit TextureCache(Texture* parent = nullptr);
	~TextureCache();

	Texture* texture() const;

	void markDirty();
	bool isDirty() const;

protected:
	void markClean();

private:
	QPointer<Texture> m_texture;
	bool m_isDirty;
};

}

#endif // A3DTEXTURECACHE_H
