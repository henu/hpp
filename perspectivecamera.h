#ifndef HPP_PERSPECTIVECAMERA_H
#define HPP_PERSPECTIVECAMERA_H

#include "camera.h"
#include "angle.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
namespace Hpp
{

class Perspectivecamera : public Camera
{

public:

	// Constructor. FOV means angle between opposite viewfrustum planes.
	inline Perspectivecamera(Hpp::Angle const& fov_y,
	                          Hpp::Real near, Hpp::Real far,
	                          size_t viewport_x, size_t viewport_y,
	                          size_t viewport_width, size_t viewport_height);

	// Updates precalculated stuff after transform,
	// fov, near, far or viewport is changed.
	inline virtual void update(void);

private:

	Hpp::Angle fov_y;

};

inline Perspectivecamera::Perspectivecamera(Hpp::Angle const& fov_y,
                                             Hpp::Real near, Hpp::Real far,
                                             size_t viewport_x, size_t viewport_y,
                                             size_t viewport_width, size_t viewport_height) :
Camera(near, far, viewport_x, viewport_y, viewport_width, viewport_height),
fov_y(fov_y)
{
	update();
}

inline void Perspectivecamera::update(void)
{
	aspectratio = viewport_width / Real(viewport_height);

	Matrix4 view = transf.getMatrix().inverse();
	Matrix4 proj = Matrix4::projectionMatrix(fov_y, aspectratio, near, far);
	projviewmat = proj * view;
}

}

#endif
