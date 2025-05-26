#include "common.h"
#include <QDebug>
#include <QDateTime>

namespace A3D {

void log(LogChannel channel, QStringView text) {
	auto timestamp = []() -> QString {
		return QDateTime::currentDateTime().toString(u"[yyyy.MM.dd hh:mm:ss.zzz] ");
	};

	switch(channel) {
	default:
	case LC_Debug:
#ifdef _DEBUG
		qDebug().noquote() << timestamp() << u"[D] " << text;
#endif
		break;
	case LC_Info:
		qInfo().noquote() << timestamp() << u"[I] " << text;
		break;
	case LC_Warning:
		qWarning().noquote() << timestamp() << u"[W] " << text;
		break;
	case LC_Critical:
		qCritical().noquote() << timestamp() << u"[C] " << text;
		break;
	case LC_Fatal:
		qFatal().noquote() << timestamp() << u"[F] " << text;
		break;
	}
}

QVector3D axisVector(Axis3D axis) {
	static QVector3D axes[AXIS_COUNT + 1] = {
		QVector3D(1.f, 0.f, 0.f),
		QVector3D(0.f, 1.f, 0.f),
		QVector3D(0.f, 0.f, 1.f),
		QVector3D(0.f, 0.f, 0.f),
	};

	if(axis < AXIS_COUNT)
		return axes[axis];

	return axes[AXIS_COUNT];
}

void setVectorAxis(QVector3D& vector, Axis3D axis, float value) {
	if(axis < AXIS_COUNT)
		vector[axis] = value;
}

float getVectorAxis(QVector3D const& vector, Axis3D axis) {
	if(axis < AXIS_COUNT)
		return vector[axis];

	return 0.f;
}

RawMatrix4x4::RawMatrix4x4()
	: RawMatrix4x4(QMatrix4x4()) {}

RawMatrix4x4::RawMatrix4x4(QMatrix4x4 const& m) {
	std::memcpy(data, m.data(), sizeof(data));
}

RawMatrix4x4& RawMatrix4x4::operator=(QMatrix4x4 const& m) {
	std::memcpy(data, m.data(), sizeof(data));
	return *this;
}

bool RawMatrix4x4::operator==(QMatrix4x4 const& o) const {
	return std::memcmp(data, o.data(), sizeof(data)) == 0;
}

bool RawMatrix4x4::operator!=(QMatrix4x4 const& o) const {
	return !(*this == o);
}

void normalizeMinMax(std::vector<float>& data, float min, float max) {
	float const fInvFactor = 1.f / (max - min);

	for(float& val: data)
		val = (val - min) * fInvFactor;
}

void normalize(std::vector<float>& data) {
	auto itMin = std::min_element(data.begin(), data.end());
	if(itMin == data.end())
		return;

	auto itMax = std::max_element(data.begin(), data.end());
	if(itMax == data.end())
		return;

	normalizeMinMax(data, *itMin, *itMax);
}

QVector4D colorToVector(QColor color) {
	return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
	;
}

QImage const* imageWithFormat(QImage::Format format, QImage const& base, QImage& storage) {
	if(base.format() == format)
		return &base;
	storage = base.convertToFormat(format);
	return &storage;
}

// Möller–Trumbore algorithm
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool intersectTriangle(
	QVector3D const& orig, QVector3D const& dir,                   // Ray informations
	QVector3D const& v0, QVector3D const& v1, QVector3D const& v2, // Triangle vertices
	QVector3D& hitPoint, float const tolerance
) {
	QVector3D edge1   = v1 - v0;
	QVector3D edge2   = v2 - v0;
	QVector3D h       = QVector3D::crossProduct(dir, edge2);
	float determinant = QVector3D::dotProduct(edge1, h);

	// Check if the Ray is parallel to triangle
	if(fabs(determinant) < tolerance)
		return false;

	float inverseDeterminant = 1.0 / determinant;
	QVector3D s              = orig - v0;
	float u                  = inverseDeterminant * QVector3D::dotProduct(s, h);

	if((u < 0.f) || (u > 1.f))
		return false;

	QVector3D q = QVector3D::crossProduct(s, edge1);
	// Vertices u and v, the sum must be not negative and the must be around 1
	float v = inverseDeterminant * QVector3D::dotProduct(dir, q);
	if(v < 0.f || (u + v > 1.f))
		return false;

	float t = inverseDeterminant * QVector3D::dotProduct(edge2, q);

	bool ret = t > tolerance;
	if(ret)
		hitPoint = orig + dir * t;

	return ret;
}

std::pair<QVector3D, QVector3D> getClosestLinePoints(QVector3D positionA1, QVector3D positionA2, QVector3D positionB1, QVector3D positionB2, bool clampA, bool clampB) {
	QVector3D const p1 = positionA1;
	QVector3D const d1 = positionA2 - positionA1;

	QVector3D const p2 = positionB1;
	QVector3D const d2 = positionB2 - positionB1;

	QVector3D const w0 = (p1 - p2);

	float const a = QVector3D::dotProduct(d1, d1);
	float const b = QVector3D::dotProduct(d1, d2);
	float const c = QVector3D::dotProduct(d2, d2);
	float const d = QVector3D::dotProduct(d1, w0);
	float const e = QVector3D::dotProduct(d2, w0);

	float const invDenom = 1.f / ((a * c) - (b * b));

	float t = ((b * e) - (c * d)) * invDenom;
	float s = ((a * e) - (b * d)) * invDenom;

	if(clampA)
		t = std::clamp(t, 0.f, 1.f);
	if(clampB)
		s = std::clamp(s, 0.f, 1.f);

	return std::make_pair(p1 + (d1 * t), p2 + (d2 * s));
}

}
