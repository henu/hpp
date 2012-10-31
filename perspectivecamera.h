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

	// Updates precalculated stuff after transform,
	// fov, nearplane, farplane or viewport is changed.
	inline virtual void update(void);

	inline Viewfrustum getViewfrustum(void) const;

private:

	Angle fov_y;

};

inline Perspectivecamera::Perspectivecamera(void) :
fov_y(Angle::fromDegrees(0))
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

inline void Perspectivecamera::update(void)
{
	aspectratio = viewport_width / Real(viewport_height);

	viewmat = transf.getMatrix().inverse();
	projmat = Matrix4::projectionMatrix(fov_y, aspectratio, nearplane, farplane);
	projviewmat = projmat * viewmat;
}

inline Viewfrustum Perspectivecamera::getViewfrustum(void) const
{
	Vector3 dir(0, 0, -1);
	Vector3 up(0, 1, 0);
	Vector3 right(1, 0, 0);

	dir = transf.applyToPosition(dir);
	up = transf.applyToPosition(up);
	right = transf.applyToPosition(right);

	return Viewfrustum::fromCamera(transf.getPosition(),
	                                dir, up, right,
	                                fov_y,
	                                calculateFovXFromAspectRatio(fov_y, aspectratio),
	                                nearplane);
}

}

#endif
