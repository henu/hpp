#ifndef HPP_CAMERA_H
#define HPP_CAMERA_H

#include "transform.h"
#include "matrix4.h"
#include "viewfrustum.h"
#include "inc_gl.h"

#include <cstdlib>

namespace Hpp
{

class Camera
{

public:

	virtual Camera* clone(void) const = 0;

	inline void setTransform(Transform const& transf);
	inline Transform getTransform(void) const { return transf; }

	inline void setNearAndFarPlanes(Real nearplane, Real farplane);
	inline void setViewport(size_t viewport_x, size_t viewport_y,
	                        size_t viewport_width, size_t viewport_height);

	inline Real getNear(void) const { return nearplane; }
	inline Real getFar(void) const { return farplane; }

	inline Vector3 getPosition(void) const { return transf.getPosition(); }
	inline Real getAspectratio(void) const { return aspectratio; }

	// Camera must be updated, when this is called.
	virtual Viewfrustum getViewfrustum(bool use_farplane = false) const = 0;

	inline Matrix4 getViewmatrix(void) const { return viewmat; }
	inline Matrix4 getProjectionmatrix(void) const { return projmat; }
	inline Matrix4 getProjectionviewmatrix(void) const { return projviewmat; }

	inline Vector3 getRight(void) const { return right; }
	inline Vector3 getUp(void) const { return up; }
	inline Vector3 getDir(void) const { return dir; }

	// Tells OpenGL to use viewport from this camera
	inline void setUpViewport(void) const;

	virtual void shootRay(Vector3& result_begin, Vector3& result_dir, Vector2 const& pos_rel) const = 0;

	// Updates precalculated stuff after transform or viewport is changed.
	virtual void update(void) = 0;

protected:

	inline Camera(void);
	inline Camera(Real nearplane, Real farplane,
	              size_t viewport_x, size_t viewport_y,
	              size_t viewport_width, size_t viewport_height);

	Transform transf;
	Vector3 right, up, dir;

	Real nearplane, farplane;

	size_t viewport_x, viewport_y;
	size_t viewport_width, viewport_height;

	Real aspectratio;

	// View, Projection and Projection * View matrix
	Matrix4 viewmat;
	Matrix4 projmat;
	Matrix4 projviewmat;

	// For clone(). This copies all information
	// from this Camera to another one.
	inline void copyDataTo(Camera* cam) const;

private:

};

inline void Camera::setTransform(Transform const& transf)
{
	this->transf = transf;
}

inline void Camera::setNearAndFarPlanes(Real nearplane, Real farplane)
{
	this->nearplane = nearplane;
	this->farplane = farplane;
}

inline void Camera::setViewport(size_t viewport_x, size_t viewport_y,
                                size_t viewport_width, size_t viewport_height)
{
	this->viewport_x = viewport_x;
	this->viewport_y = viewport_y;
	this->viewport_width = viewport_width;
	this->viewport_height = viewport_height;
}

inline void Camera::setUpViewport(void) const
{
	glViewport(viewport_x, viewport_y, viewport_width, viewport_height);
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

void Camera::copyDataTo(Camera* cam) const
{
	cam->transf = transf;
	cam->right = right;
	cam->up = up;
	cam->dir = dir;
	cam->nearplane = nearplane;
	cam->farplane = farplane;
	cam->viewport_x = viewport_x;
	cam->viewport_y = viewport_y;
	cam->viewport_width = viewport_width;
	cam->viewport_height = viewport_height;
	cam->aspectratio = aspectratio;
	cam->viewmat = viewmat;
	cam->projmat = projmat;
	cam->projviewmat = projviewmat;
}

}

#endif
