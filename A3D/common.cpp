#include "A3D/common.h"
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
		qFatal().noquote() << timestamp() << "[F] " << text;
		break;
	}
}

void log(LogChannel channel, QString text) {
	log(channel, QStringView(text));
}

}
