#include "common.h"
#include <QDebug>
#include <QDateTime>
#include "view.h"

namespace A3D {

QVector3D unprojectPointFrom2D(View* v, QPointF point) {
	QMatrix4x4 invProj = v->camera().getProjection().inverted();
	QMatrix4x4 invView = v->camera().getView().inverted();
	float newX         = 2.f * point.x() - 1.f;
	float newY         = -2.f * point.y() + 1.f;
	float newZ         = 1.f;

	QVector4D clip(newX, newY, newZ, 1.f);

	QVector4D eye = invProj * clip;
	eye.setW(1.f);

	QVector4D world = invView * eye;
	QVector3D end   = (world.toVector3D());
	return end;
}

QPointF getCurrentNormalizedPos(View* v) {
	if(!v)
		return QPointF(0, 0);
	QPointF cursorPos = v->mapFromGlobal(QCursor::pos());
	QSize size        = v->window()->size();
	QPointF point     = QPointF(cursorPos.x() / size.width(), cursorPos.y() / size.height());
	return point;
}

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
