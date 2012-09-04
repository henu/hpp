#ifndef HPP_OLDCAMERA_H
#define HPP_OLDCAMERA_H

#include "movable.h"
#include "angle.h"
#include "3dutils.h"
#include "assert.h"
#include "gldebug.h"
#include "inc_gl.h"

namespace Hpp
{

class OldCamera : public Movable
{

public:

	inline OldCamera(void);

	// Setters
	inline void setFovX(Angle const& fov_x) { this->fov_x = fov_x; }
	inline void setFovY(Angle const& fov_y) { this->fov_y = fov_y; }
	inline void setNear(Real clip_near) { this->clip_near = clip_near; }
	inline void setFar(Real clip_far) { this->clip_far = clip_far; }
	inline void setNearAndFar(Real clip_near, Real clip_far) { this->clip_near = clip_near; this->clip_far = clip_far; }
	inline void setRotationFromUpRight(Vector3 const& up, Vector3 const& right);

	// Getters
	inline Angle getFovX(void) const { return fov_x; }
	inline Angle getFovY(void) const { return fov_y; }
	inline Real getNear(void) const { return clip_near; }
	inline Real getFar(void) const { return clip_far; }

	inline void prepareProjection(Real viewport_width, Real viewport_height);

	// Get all Renderables from given Movable and and its children that
	// visible from this OldCamera. Movable must not have parent.
	inline void getVisibles(Visibles& result, Movable const* movable) const;

private:

	static size_t const CLIP_NEAR_DEFAULT = 1;
	static size_t const CLIP_FAR_DEFAULT = 1000;

	Angle fov_x, fov_y;
	Real clip_near, clip_far;

};

inline OldCamera::OldCamera(void) :
fov_x(Angle::fromDegrees(90)),
fov_y(Angle::fromDegrees(90)),
clip_near(CLIP_NEAR_DEFAULT),
clip_far(CLIP_FAR_DEFAULT)
{
}

inline void OldCamera::setRotationFromUpRight(Vector3 const& up, Vector3 const& right)
{
	transf.setRotationFromUpRight(up, right);
	transformChanged();
}

inline void OldCamera::prepareProjection(Real viewport_width, Real viewport_height)
{
	// Tune projection
	HppCheckGlErrors();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = viewport_width / viewport_height;
	GLdouble ymax = clip_near * (fov_y / 2.0).tan();
	GLdouble ymin = -ymax;
	GLdouble xmin = ymin * aspect;
	GLdouble xmax = ymax * aspect;
	glFrustum(xmin, xmax, ymin, ymax, clip_near, clip_far);

	// Apply location and projection of OldCamera
	HppCheckGlErrors();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Transform transf = getAbsoluteTransform();
	glLoadMatrixf(transpose(transf.getMatrix()).inverse().getCells());
	HppCheckGlErrors();
}

inline void OldCamera::getVisibles(Visibles& result, Movable const* movable) const
{
	HppAssert(!movable->getParent(), "This function can be only called to root Movables!");
	Transform cam_transf = getAbsoluteTransform();
	Viewfrustum vfrust = Viewfrustum::fromCamera(cam_transf.getPosition(),
	                                             -cam_transf.getZVector(),
	                                             cam_transf.getYVector(),
	                                             cam_transf.getXVector(),
	                                             getFovY(),
	                                             getFovX(),
	                                             clip_near);
	Viewfrustums vfrusts;
	vfrusts.push_back(vfrust);
	movable->getAllVisibles(result, vfrusts);
}

}

#endif
