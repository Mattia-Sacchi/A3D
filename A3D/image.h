#ifndef A3DIMAGE_H
#define A3DIMAGE_H

#include "A3D/common.h"
#include <QImage>

namespace A3D {

class Image {
public:
	struct HDRData {
		inline HDRData()
			: w(0), h(0), nrComponents(0) {}

		HDRData(HDRData const& o) =default;
		HDRData(HDRData&& o) =default;
		HDRData& operator=(HDRData const& o) =default;
		HDRData& operator=(HDRData&& o) =default;

		std::vector<float> m_data;
		std::size_t w;
		std::size_t h;
		std::size_t nrComponents;
	};

	static Image HDR(QString path);

	Image();
	Image(QImage const& i);

	void setFromQImage(QImage const&);
	void setFromHDR(QIODevice&);
	void setFromHDR(HDRData);

	void saveToFile(QIODevice&);

	bool isQImage() const;
	bool isHDR() const;

	QSize size() const;
	bool isNull() const;
	bool hasAlphaChannel() const;

	QImage const& qimage() const;
	HDRData const& hdr() const;

	Image& operator= (QImage const&);

private:
	bool isCurrentlyTransparent() const;
	void checkTransparency() const;

	enum Type {
		T_QIMAGE,
		T_HDRDATA
	} m_type;

	enum TransparencyDetection {
		TD_NOT_DONE_YET = 0,
		TD_OPAQUE,
		TD_TRANSPARENT
	};
	mutable TransparencyDetection m_transparent;

	QImage m_qimage;
	HDRData m_hdr;
};

}

#endif // A3DIMAGE_H
