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

	// Constructor. FOV means angle between opposite viewfrustum planes.
	inline Orthographiccamera(void);
	inline Orthographiccamera(Real size,
	                          Real nearplane, Real farplane,
	                          size_t viewport_x = 0, size_t viewport_y = 0,
	                          size_t viewport_width = 0, size_t viewport_height = 0);

	// Size means distance between top and bottom viewfrustum planes.
	inline void setSize(Real size);

	inline virtual Viewfrustum getViewfrustum(void) const;
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

inline void Orthographiccamera::setSize(Real size)
{
	this->size = size;
}

inline Viewfrustum Orthographiccamera::getViewfrustum(void) const
{
// TODO: Code this!
HppAssert(false, "Not implemented yet!");
return Viewfrustum();
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
	right = transf_rotscale.applyToPosition(Hpp::Vector3(1, 0, 0));
	up = transf_rotscale.applyToPosition(Hpp::Vector3(0, 1, 0));
	dir = transf_rotscale.applyToPosition(Hpp::Vector3(0, 0, -1));
}

}

#endif

