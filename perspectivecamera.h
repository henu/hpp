#ifndef HPP_PERSPECTIVECAMERA_H
#define HPP_PERSPECTIVECAMERA_H

#include "camera.h"
#include "angle.h"
#include "viewfrustum.h"
#include "3dutils.h"
#include "display.h"

namespace Hpp
{

class Perspectivecamera : public Camera
{

public:

	// Constructor. FOV means angle between opposite viewfrustum planes.
	inline Perspectivecamera(void);
	inline Perspectivecamera(Angle const& fov_y,
	                         Real nearplane, Real farplane,
	                         size_t viewport_x = 0, size_t viewport_y = 0,
	                         size_t viewport_width = 0, size_t viewport_height = 0);

	// FOV means angle between opposite viewfrustum planes.
	inline void setFovY(Angle const& fov_y);

	inline virtual void shootRay(Vector3& result_begin, Vector3& result_dir, Vector2 const& pos_rel) const;

	// Updates precalculated stuff after transform,
	// fov, nearplane, farplane or viewport is changed.
	inline virtual void update(void);

	inline Viewfrustum getViewfrustum(void) const;

private:

	Angle fov_y;

};

inline Perspectivecamera::Perspectivecamera(void) :
fov_y(Angle(0))
{
}

inline Perspectivecamera::Perspectivecamera(Angle const& fov_y,
                                            Real nearplane, Real farplane,
                                            size_t viewport_x, size_t viewport_y,
                                            size_t viewport_width, size_t viewport_height) :
Camera(nearplane, farplane, viewport_x, viewport_y, viewport_width, viewport_height),
fov_y(fov_y)
{
	if (viewport_width == 0 && Display::getWidth() > viewport_x) {
		viewport_width = Display::getWidth() - viewport_x;
	}
	if (viewport_height == 0 && Display::getHeight() > viewport_y) {
		viewport_height = Display::getHeight() - viewport_y;
	}
	update();
}

inline void Perspectivecamera::setFovY(Angle const& fov_y)
{
	this->fov_y = fov_y;
}

inline void Perspectivecamera::shootRay(Vector3& result_begin, Vector3& result_dir, Vector2 const& pos_rel) const
{
	Hpp::Angle fov_x = fovYToFovX(fov_y, aspectratio);

	result_begin = transf.getPosition();

	result_dir = Hpp::Vector3::ZERO;
	result_dir += getRight() * (fov_x / 2).tan() * pos_rel.x;
	result_dir += getUp() * (fov_y / 2).tan() * pos_rel.y;
	result_dir += getDir();
}

inline void Perspectivecamera::update(void)
{
	aspectratio = viewport_width / Real(viewport_height);

	viewmat = transf.getMatrix().inverse();
	projmat = Matrix4::perspectiveProjectionMatrix(fov_y, aspectratio, nearplane, farplane);
	projviewmat = projmat * viewmat;

	Transform transf_rotscale = transf.getRotScale();
	right = transf_rotscale.applyToPosition(Hpp::Vector3(1, 0, 0));
	up = transf_rotscale.applyToPosition(Hpp::Vector3(0, 1, 0));
	dir = transf_rotscale.applyToPosition(Hpp::Vector3(0, 0, -1));
}

inline Viewfrustum Perspectivecamera::getViewfrustum(void) const
{
	return Viewfrustum::fromCamera(transf.getPosition(),
	                                dir, up, right,
	                                fov_y,
	                                fovYToFovX(fov_y, aspectratio),
	                                nearplane);
}

}

#endif
