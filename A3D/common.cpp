#include "common.h"
#include <QDebug>
#include <QDateTime>

namespace A3D {

void log(LogChannel channel, QStringView text) {
	auto timestamp = []() -> QString {
		return QDateTime::currentDateTime().toString("[yyyy.MM.dd hh:mm:ss.zzz] ");
	};

	switch(channel) {
	default:
	case LC_Debug:
#ifdef _DEBUG
		qDebug().noquote() << timestamp() << "[D] " << text;
#endif
		break;
	case LC_Info:
		qInfo().noquote() << timestamp() << "[I] " << text;
		break;
	case LC_Warning:
		qWarning().noquote() << timestamp() << "[W] " << text;
		break;
	case LC_Critical:
		qCritical().noquote() << timestamp() << "[C] " << text;
		break;
	case LC_Fatal:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		qCritical().noquote() << timestamp() << "[C] " << text;
#else
		qFatal().noquote() << timestamp() << "[F] " << text;
#endif
		break;
	}
}

void log(LogChannel channel, QString text) {
	log(channel, QStringView(text));
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

void setVectorAxis(QVector3D& vector, Axis3D axis, float value) {
	if(axis < AXIS_COUNT)
		vector[axis] = value;
}

float getVectorAxis(QVector3D const& vector, Axis3D axis) {
	if(axis < AXIS_COUNT)
		return vector[axis];

	return 0.f;
}

QVector4D colorToVector(QColor color) {
	return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
	;
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
	if(u < 0.0 || u > 1.0)
		return false;

	QVector3D q = QVector3D::crossProduct(s, edge1);
	// Vertices u and v, the sum must be not negative and the must be around 1
	float v = inverseDeterminant * QVector3D::dotProduct(dir, q);
	if(v < 0.0 || u + v > 1.0)
		return false;

	float t = inverseDeterminant * QVector3D::dotProduct(edge2, q);

	bool ret = t > tolerance;
	if(ret)
		hitPoint = orig + dir * t;

	return ret;
}

}
