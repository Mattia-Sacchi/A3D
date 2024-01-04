#ifndef A3DCAMERAVIEW_H
#define A3DCAMERAVIEW_H

#include "A3D/common.h"

namespace A3D {

class Camera
{
public:
	enum ProjectionMode {
		PM_PERSPECTIVE = 0,
		PM_ORTHOGONAL
	};
	
	Camera();
	~Camera();
	
	// View matrix
	
	// Retrieves the position of the camera
	QVector3D const& position() const;
	// Changes the position of the camera
	void setPosition(QVector3D const& pos);
	// Adds an offset to the current position of the camera
	inline void offsetPosition(QVector3D const& pos) {
		setPosition(position() + pos);
	}
	
	// Returns the view orientation matrix (rotation only)
	QMatrix4x4 orientation() const;
	// Returns the XYZ rotation angle
	QVector3D const& angle() const;
	
	// Changes the orientation so that the camera is pointing to a target point	
	// Assumes that the upVector is (0,1,0) (thus will cancel the Z component)
	void setOrientationTarget(QVector3D const& target);
	
	// Adds an offset to the current orientation of the camera.
	void offsetOrientation(QVector3D const& orientation);
	
	// Returns the forward component of the camera
	QVector3D forward() const;
	// Returns the right component of the camera
	QVector3D right() const;
	// Returns the up component of the camera
	QVector3D up() const;
	
	// Returns the View matrix
	QMatrix4x4 const& getView() const;
	
	// Projection matrix.
	
	// Current Projection Mode.
	// See setOrthogonal and setPerspective.
	ProjectionMode projectionMode() const;
	
	// Retrieves the near clipping plane of the frustum
	float nearPlane() const;
	// Sets the near clipping plane of the frustom
	void setNearPlane(float nearPlane);
	
	// Retrieves the far clipping plane of the frustum
	float farPlane() const;
	// Sets the far clipping plane of the frustom
	void setFarPlane(float farPlane);
	
	// Sets the near and far clipping planes of the frustum
	inline void setPlanes(float nearPlane, float farPlane) {
		setNearPlane(nearPlane);
		setFarPlane(farPlane);
	}
	
	// Sets the camera to be in orthogonal mode.
	// The display coordinates are mapped to the supplied rectangle.
	void setOrthogonal(QRectF const& rect);
	
	// Sets the camera to be in perspective mode.
	void setPerspective(float verticalFOV, float aspectRatio);
	
	QMatrix4x4 const& getProjection() const;
	
private:
	mutable bool m_viewMatrixIsDirty;
	mutable QMatrix4x4 m_viewMatrix;
	
	QVector3D m_position;
	QVector3D m_angle;
	
	mutable bool m_projMatrixIsDirty;
	mutable QMatrix4x4 m_projMatrix;
	ProjectionMode m_projectionMode;
	float m_nearPlane;
	float m_farPlane;
	QRectF m_orthoView;
	float m_perspVerticalFOV;
	float m_perspAspectRatio;
};

}
#endif // A3DCAMERAVIEW_H
