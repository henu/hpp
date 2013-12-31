#ifndef HPP_ORTHOGRAPHICCAMERA_H
#define HPP_ORTHOGRAPHICCAMERA_H

#include "camera.h"
#include "angle.h"
#include "viewfrustum.h"
#include "3dutils.h"
#include "display.h"

namespace Hpp
{

class Orthographiccamera : public Camera
{

public:

	// Constructor. "size" means distance between top and bottom edges of screen.
	inline Orthographiccamera(void);
	inline Orthographiccamera(Real size,
	                          Real nearplane, Real farplane,
	                          size_t viewport_x = 0, size_t viewport_y = 0,
	                          size_t viewport_width = 0, size_t viewport_height = 0);

	inline virtual Camera* clone(void) const;

	// Size means distance between top and bottom viewfrustum planes.
	inline void setSize(Real size);

	inline virtual Viewfrustum getViewfrustum(bool use_farplane = false) const;
	inline virtual void shootRay(Vector3& result_begin, Vector3& result_dir, Vector2 const& pos_rel) const;

	// Updates precalculated stuff after transform,
	// size, nearplane, farplane or viewport is changed.
	inline virtual void update(void);

private:

	Real size;

};

inline Orthographiccamera::Orthographiccamera(void) :
size(0)
{
}

inline Orthographiccamera::Orthographiccamera(Real size,
                                              Real nearplane, Real farplane,
                                              size_t viewport_x, size_t viewport_y,
                                              size_t viewport_width, size_t viewport_height) :
Camera(nearplane, farplane, viewport_x, viewport_y, viewport_width, viewport_height),
size(size)
{
	if (viewport_width == 0 && Display::getWidth() > viewport_x) {
		viewport_width = Display::getWidth() - viewport_x;
	}
	if (viewport_height == 0 && Display::getHeight() > viewport_y) {
		viewport_height = Display::getHeight() - viewport_y;
	}
	update();
}

Camera* Orthographiccamera::clone(void) const
{
	Orthographiccamera* result = new Orthographiccamera;
	result->size = size;
	copyDataTo(result);
	return result;
}

inline void Orthographiccamera::setSize(Real size)
{
	this->size = size;
}

inline Viewfrustum Orthographiccamera::getViewfrustum(bool use_farplane) const
{
	Plane::Vec cutplanes;

	Vector3 pos = transf.getPosition();

	Real size_x = (size / viewport_width) * viewport_height;

	// Top plane
	cutplanes.push_back(Plane(-up, pos + up.normalized() * size / 2));
	// Bottom plane
	cutplanes.push_back(Plane(up, pos - up.normalized() * size / 2));
	// Right plane
	cutplanes.push_back(Plane(-right, pos + right.normalized() * size_x / 2));
	// Left plane
	cutplanes.push_back(Plane(right, pos - right.normalized() * size_x / 2));
	// Near plane
	cutplanes.push_back(Plane(dir, pos + dir.normalized() * nearplane));
	// Far plane
	if (use_farplane) {
		cutplanes.push_back(Plane(-dir, pos + dir.normalized() * farplane));
	}

	return Viewfrustum(pos, Boundingconvex(cutplanes));
}

inline void Orthographiccamera::shootRay(Vector3& result_begin, Vector3& result_dir, Vector2 const& pos_rel) const
{
	Real size_x = (size / viewport_width) * viewport_height;
	result_begin = transf.getPosition();
	result_begin += getRight() * size_x / 2 * pos_rel.x;
	result_begin += getUp() * size / 2 * pos_rel.y;
	result_dir = getDir();
}

inline void Orthographiccamera::update(void)
{
	aspectratio = viewport_width / Real(viewport_height);

	viewmat = transf.getMatrix().inverse();
	projmat = Matrix4::orthographicProjectionMatrix(size, aspectratio, nearplane, farplane);
	projviewmat = projmat * viewmat;

	Transform transf_rotscale = transf.getRotScale();
	right = transf_rotscale.applyToPosition(Vector3(1, 0, 0));
	up = transf_rotscale.applyToPosition(Vector3(0, 1, 0));
	dir = transf_rotscale.applyToPosition(Vector3(0, 0, -1));
}

}

#endif

