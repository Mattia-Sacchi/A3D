#ifndef A3DCOMMON_H
#define A3DCOMMON_H

#include <QGenericMatrix>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QPointer>
#include <QtMath>
#include <QString>
#include <QStringView>
#include <QVariant>
#include <QObject>
#include <QImage>

#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>

#include <memory>
#include <limits>
#include <map>
#include <vector>
#include <list>
#include <chrono>
#include <stdexcept>
#include <cstdint>
#include <QPointF>

namespace A3D {

class Entity;
class Model;
class Group;

struct IntersectionResult {
	Entity* m_resultingEntity;
	Model* m_resultingModel;
	Group* m_resultingGroup;
	QVector3D m_groupLocalHitPoint;
	QVector3D m_hitPoint;
};

inline void normalizeMinMax(std::vector<float>& data, float min, float max) {
	float const fInvFactor = 1.f / (max - min);

	for(auto it = data.begin(); it != data.end(); ++it)
		*it = (*it - min) * fInvFactor;
}

inline void normalize(std::vector<float>& data) {
	auto itMin = std::min_element(data.begin(), data.end());
	if(itMin == data.end())
		return;

	auto itMax = std::max_element(data.begin(), data.end());
	if(itMax == data.end())
		return;

	float const min = *itMin;
	float const max = *itMax;
	normalizeMinMax(data, min, max);
}
class View;

QVector3D unprojectPointFrom2D(View* v, QPointF point);

QPointF getCurrentNormalizedPos(View* v);

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

inline QImage const* imageWithFormat(QImage::Format format, QImage const& base, QImage& storage) {
	if(base.format() == format)
		return &base;
	storage = base.convertToFormat(format);
	return &storage;
}

bool intersectTriangle(QVector3D const& rayOrigin, QVector3D const& rayDirection, QVector3D const& triangle0, QVector3D const& triangle1, QVector3D const& triangle2, QVector3D& hitPoint, float const tolerance = 1e-6);

}

#endif // A3DCOMMON_H
