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

	inline Matrix4 getProjectionviewmatrix(void) const;

	// Tells OpenGL to use viewport from this camera
	inline void setUpViewport(void) const;

	// Updates precalculated stuff after transform or viewport is changed.
	inline virtual void update(void) = 0;

protected:

	inline Camera(Real near, Real far,
	              size_t viewport_x, size_t viewport_y,
	              size_t viewport_width, size_t viewport_height);

	Transform transf;

	Real near, far;

	size_t viewport_x, viewport_y;
	size_t viewport_width, viewport_height;

	Real aspectratio;

	// Projection * View matrix
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

inline Matrix4 Camera::getProjectionviewmatrix(void) const
{
	return projviewmat;
}

inline Camera::Camera(Real near, Real far,
                      size_t viewport_x, size_t viewport_y,
                      size_t viewport_width, size_t viewport_height) :
near(near),
far(far),
viewport_x(viewport_x),
viewport_y(viewport_y),
viewport_width(viewport_width),
viewport_height(viewport_height),
aspectratio(viewport_width / Real(viewport_height))
{
}

}

#endif
