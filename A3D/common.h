#ifndef A3DCOMMON_H
#define A3DCOMMON_H

#include <QGenericMatrix>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QPointer>
#include <QtMath>
#include <QString>
#include <QStringView>
#include <QLatin1StringView>
#include <QVariant>
#include <QObject>

#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>

#include <memory>
#include <limits>
#include <map>
#include <vector>
#include <list>

namespace A3D {

enum LogChannel {
	LC_Debug = 0,
	LC_Info,
	LC_Warning,
	LC_Critical,
	LC_Fatal,
};

void log(LogChannel, QString);
void log(LogChannel, QStringView);

class NonCopyable {
	NonCopyable(NonCopyable const&)            = delete;
	NonCopyable& operator=(NonCopyable const&) = delete;

public:
	inline NonCopyable() {}
};

typedef QOpenGLFunctions_3_3_Core CoreGLFunctions;

template <typename T>
void cleanupQPointers(std::vector<QPointer<T>>& container) {
	container.erase(
		std::remove_if(
			container.begin(), container.end(),
			[](QPointer<T> const& e) -> bool {
				return e.isNull();
			}
		),
		container.end()
	);
}

template <typename K, typename T>
void cleanupQPointers(std::map<K, QPointer<T>>& container) {
	for(auto it = container.begin(); it != container.end();) {
		if(it->second.isNull())
			it = container.erase(it);
		else
			++it;
	}
}
}

#endif // A3DCOMMON_H
