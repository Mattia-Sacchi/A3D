#ifndef A3DCOMMON_H
#define A3DCOMMON_H

#include <QGenericMatrix>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QPointF>
#include <QPointer>
#include <QtMath>
#include <QString>
#include <QStringView>
#include <QVariant>
#include <QObject>
#include <QImage>

#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>

#include <map>
#include <vector>

namespace A3D {

enum Direction3D {
    D_PositiveX = 0,
    D_PositiveY,
    D_PositiveZ,
    D_NegativeX,
    D_NegativeY,
    D_NegativeZ,
    D_Count,
};

enum Axis3D {
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
    AXIS_COUNT,
};

QVector3D axisVector(Axis3D axis);

class Entity;
class Model;
class Group;
class View;

struct IntersectionResult {
	Entity* m_resultingEntity;
	Model* m_resultingModel;
	Group* m_resultingGroup;
	QVector3D m_groupLocalHitPoint;
	QVector3D m_hitPoint;
};

void normalizeMinMax(std::vector<float>& data, float min, float max);
void normalize(std::vector<float>& data);

void setVectorAxis(QVector3D& vector, Axis3D axis, float value);
float getVectorAxis(QVector3D const& vector, Axis3D axis);

QVector4D colorToVector(QColor color);

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

bool intersectTriangle(
	QVector3D const& rayOrigin, QVector3D const& rayDirection, QVector3D const& triangle0, QVector3D const& triangle1, QVector3D const& triangle2, QVector3D& hitPoint,
	float const tolerance = 1e-6
);

}

#endif // A3DCOMMON_H
