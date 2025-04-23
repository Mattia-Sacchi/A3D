#include "common.h"
#include <QDebug>
#include <QDateTime>
#include "A3D/view.h"

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

}
