#ifndef HPP_CAMERA_H
#define HPP_CAMERA_H

#include "transform.h"
#include "matrix4.h"
#include "inc_gl.h"

#include <cstdlib>

namespace Hpp
{

class Camera
{

public:

	inline void setTransform(Transform const& transf);

	inline Matrix4 getViewmatrix(void) const;
	inline Matrix4 getProjectionmatrix(void) const;
	inline Matrix4 getProjectionviewmatrix(void) const;

	// Tells OpenGL to use viewport from this camera
	inline void setUpViewport(void) const;

	// Updates precalculated stuff after transform or viewport is changed.
	virtual void update(void) = 0;

protected:

	inline Camera(void);
	inline Camera(Real nearplane, Real farplane,
	              size_t viewport_x, size_t viewport_y,
	              size_t viewport_width, size_t viewport_height);

	Transform transf;

	Real nearplane, farplane;

	size_t viewport_x, viewport_y;
	size_t viewport_width, viewport_height;

	Real aspectratio;

	// View, Projection and Projection * View matrix
	Matrix4 viewmat;
	Matrix4 projmat;
	Matrix4 projviewmat;

private:

};

inline void Camera::setTransform(Transform const& transf)
{
	this->transf = transf;
}

inline void Camera::setUpViewport(void) const
{
	glViewport(viewport_x, viewport_y, viewport_width, viewport_height);
}

inline Matrix4 Camera::getViewmatrix(void) const
{
	return viewmat;
}

inline Matrix4 Camera::getProjectionmatrix(void) const
{
	return projmat;
}

inline Matrix4 Camera::getProjectionviewmatrix(void) const
{
	return projviewmat;
}

inline Camera::Camera(void) :
nearplane(0),
farplane(0),
viewport_x(0),
viewport_y(0),
viewport_width(0),
viewport_height(0),
aspectratio(1)
{
}

inline Camera::Camera(Real nearplane, Real farplane,
                      size_t viewport_x, size_t viewport_y,
                      size_t viewport_width, size_t viewport_height) :
nearplane(nearplane),
farplane(farplane),
viewport_x(viewport_x),
viewport_y(viewport_y),
viewport_width(viewport_width),
viewport_height(viewport_height),
aspectratio(viewport_width / Real(viewport_height))
{
}

}

#endif
