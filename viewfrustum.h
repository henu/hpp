#ifndef HPP_VIEWFRUSTUM_H
#define HPP_VIEWFRUSTUM_H

#include "boundingsphere.h"
#include "3dutils.h"
#include "angle.h"
#include "vector2.h"
#include "vector3.h"

#include <vector>

namespace Hpp
{

// TODO: Make this class to precalculate all planes!
class Viewfrustum
{

public:

	// How boundinvolume is related to viewfrustum
	enum VFResult { OUTSIDE, PARTIALLY_INSIDE, INSIDE };

	// Default constructor makes Viewfrustum that contains nothing
	inline Viewfrustum(void);

	// Constructor. Vrts means vertices of frustrum polygon that are on a
	// plane that is one unit from the position of viewfrustrum.
	inline Viewfrustum(Vector3 const& pos,
	                   Vector3 const& dir, Vector3 const& up, Vector3 const& right,
	                   std::vector< Vector2 > const& vrts,
	                   Real nearplane = 0.0);

	inline Vector3 getPosition(void) const { return pos; }

	inline bool isEmpty(void) const { return type == EMPTY; }

	inline static Viewfrustum fromCamera(Vector3 const& pos,
	                                     Angle const& yaw,
	                                     Angle const& pitch,
	                                     Angle const& head,
	                                     Angle const& fov_y,
	                                     Angle const& fov_x,
	                                     Real nearplane);
	inline static Viewfrustum fromCamera(Vector3 const& pos,
	                                     Vector3 const& dir, Vector3 const& up, Vector3 const& right,
	                                     Angle const& fov_y,
	                                     Angle const& fov_x,
	                                     Real nearplane);

	inline static Viewfrustum all(Vector3 const& pos) { return Viewfrustum(ALL, pos); }

	inline VFResult testBoundingsphere(Boundingsphere const& bs) const;

	inline bool testPoint(Vector3 const& point) const;

	inline Viewfrustum doIntersection(Viewfrustum const& vfrust) const;

private:

	enum Type { NORMAL, ALL, EMPTY };

	Type type;
	Vector3 pos;
	Vector3 dir, up, right;
	// Vertices must always do loop counterclockwise, when looking through
	// view frustrum.
	std::vector< Vector2 > vrts;
	// Depth of near plane
	Real nearplane;
// TODO: Code support for farplane!

	inline Viewfrustum(Type const& type, Vector3 const& pos) : type(type), pos(pos) { }

	// Checks if at least some part of boundingsphere is at the front of plane.
	inline static VFResult bsInFront(Boundingsphere const& bs, Vector3 const& pos, Vector3 const& nrm);

	// Checks if point is at the front of plane.
	inline static bool pointInFront(Vector3 const& point, Vector3 const& pos, Vector3 const& nrm);

};
typedef std::vector< Viewfrustum > Viewfrustums;

inline Viewfrustum::Viewfrustum(void) :
type(EMPTY)
{
}

inline Viewfrustum::Viewfrustum(Vector3 const& pos,
                                Vector3 const& dir, Vector3 const& up, Vector3 const& right,
                                std::vector< Vector2 > const& vrts,
                                Real nearplane) :
type(NORMAL),
pos(pos),
dir(dir),
up(up),
right(right),
vrts(vrts),
nearplane(nearplane)
{
	HppAssert(fabs(dotProduct(dir, up)) < 0.001, "Dir and up must be perpendicular to each others!");
	HppAssert(fabs(dotProduct(up, right)) < 0.001, "Up and right must be perpendicular to each others!");
	HppAssert(fabs(dotProduct(right, dir)) < 0.001, "Right and dir must be perpendicular to each others!");
	if (vrts.empty()) {
		type = EMPTY;
	}
}

inline Viewfrustum Viewfrustum::fromCamera(Vector3 const& pos,
                                           Angle const& yaw,
                                           Angle const& pitch,
                                           Angle const& roll,
                                           Angle const& fov_y,
                                           Angle const& fov_x,
                                           Real nearplane)
{
	// Get vectors
	Vector3 dir, up, right;
	getCameraDirections(yaw, pitch, roll, &dir, &up, &right);

	return fromCamera(pos, dir, up, right, fov_y, fov_x, nearplane);
}

inline Viewfrustum Viewfrustum::fromCamera(Vector3 const& pos,
                                           Vector3 const& dir, Vector3 const& up, Vector3 const& right,
                                           Angle const& fov_y,
                                           Angle const& fov_x,
                                           Real nearplane)
{
	// Calculate vertices
	Real fov_y_tan = (fov_y / 2).tan();
	Real fov_x_tan = (fov_x / 2).tan();
	std::vector< Vector2 > vrts;
	vrts.push_back(Vector2(fov_x_tan, fov_y_tan));
	vrts.push_back(Vector2(-fov_x_tan, fov_y_tan));
	vrts.push_back(Vector2(-fov_x_tan, -fov_y_tan));
	vrts.push_back(Vector2(fov_x_tan, -fov_y_tan));

	return Viewfrustum(pos, dir, up, right, vrts, nearplane);
}

inline Viewfrustum::VFResult Viewfrustum::testBoundingsphere(Boundingsphere const& bs) const
{
	if (type == EMPTY) {
		return OUTSIDE;
	}
	if (type == ALL) {
		return INSIDE;
	}
	if (bs.isInfinite()) {
		return PARTIALLY_INSIDE;
	}

	HppAssert(dir.lengthTo2() > 0.999 && dir.lengthTo2() < 1.001, "Not normalized!");
	HppAssert(up.lengthTo2() > 0.999 && up.lengthTo2() < 1.001, "Not normalized!");
	HppAssert(right.lengthTo2() > 0.999 && right.lengthTo2() < 1.001, "Not normalized!");

	bool fully_inside = true;

	// Check if boundingsphere is behind near plane.
	VFResult subresult = bsInFront(bs, pos + dir * nearplane, dir);
	if (subresult == OUTSIDE) {
		return OUTSIDE;
	}
	if (subresult == PARTIALLY_INSIDE) {
		fully_inside = false;
	}

	// Check all sides of viewfrustum
// TODO/Improve: This is not accurate enough! Some boundingspheres get drawn even when they are not shown!
	HppAssert(vrts.size() >= 3, "Not enough vertices!");
	Vector3 v0_v = vrts[0].x * right + vrts[0].y * up + dir;
	for (size_t side_id = 0; side_id < vrts.size(); side_id ++) {
		Vector2 vrt = vrts[(side_id + 1) % vrts.size()];
		Vector3 v1_v = vrt.x * right + vrt.y * up + dir;

		// Calculate normal of this plane. Normal
		// is facing inside Viewfrustum
// TODO/Optimize: Should these be recalculated?
		Vector3 normal = crossProduct(v1_v, v0_v);
		// Check if boundngsphere is fully behind this plane.
		VFResult subresult = bsInFront(bs, pos, normal.normalized());
		if (subresult == OUTSIDE) {
			return OUTSIDE;
		}
		if (subresult == PARTIALLY_INSIDE) {
			fully_inside = false;
		}

		v0_v = v1_v;
	}

	if (fully_inside) {
		return INSIDE;
	} else {
		return PARTIALLY_INSIDE;
	}
}

inline bool Viewfrustum::testPoint(Vector3 const& point) const
{
	if (type == EMPTY) {
		return OUTSIDE;
	}
	if (type == ALL) {
		return INSIDE;
	}

	HppAssert(dir.lengthTo2() > 0.999 && dir.lengthTo2() < 1.001, "Not normalized!");
	HppAssert(up.lengthTo2() > 0.999 && up.lengthTo2() < 1.001, "Not normalized!");
	HppAssert(right.lengthTo2() > 0.999 && right.lengthTo2() < 1.001, "Not normalized!");

	// Check if boundingsphere is behind near plane.
	bool subresult = pointInFront(point, pos + dir * nearplane, dir);
	if (!subresult) {
		return false;
	}

	// Check all sides of viewfrustum
// TODO/Improve: This is not accurate enough! Some boundingspheres get drawn even when they are not shown!
	HppAssert(vrts.size() >= 3, "Not enough vertices!");
	Vector3 v0_v = vrts[0].x * right + vrts[0].y * up + dir;
	for (size_t side_id = 0; side_id < vrts.size(); side_id ++) {
		Vector2 vrt = vrts[(side_id + 1) % vrts.size()];
		Vector3 v1_v = vrt.x * right + vrt.y * up + dir;

		// Calculate normal of this plane. Normal
		// is facing inside Viewfrustum
// TODO/Optimize: Should these be recalculated?
		Vector3 normal = crossProduct(v1_v, v0_v);
		// Check if boundngsphere is fully behind this plane.
		bool subresult = pointInFront(point, pos, normal.normalized());
		if (!subresult) {
			return false;
		}

		v0_v = v1_v;
	}

	return true;
}

inline Viewfrustum Viewfrustum::doIntersection(Viewfrustum const& vfrust) const
{
	HppAssert((pos - vfrust.pos).lengthTo2() < 0.000001, "Unable to merge two viewfrustrums because they don\'t have the same position!");
	if (type == EMPTY || vfrust.type == EMPTY) {
		return Viewfrustum(EMPTY, pos);
	}
	if (type == ALL) {
		return vfrust;
	}
	if (vfrust.type == ALL) {
		return *this;
	}

	// Position of plane of this (and resulting) Viewfrustum.
	Vector3 ppos = pos + dir;

	// This frustum will be used as a base for new one. This means that it
	// will have same vectors for dir, up and right. First we need to move
	// vertices from another Viewfrustrum to the space of this one. Some
	// vertices may go to infinity when transformed to new space. Those
	// vertices are ignored, however it would be great if there would not
	// be any "holes of ignored vertices". That is why we need two
	// container. If vertices at infinite location are found, then we
	// switch to another container and finally move another container at
	// the front of main one.
	std::vector< Vector2 > vfrust_vrts;
	std::vector< Vector2 > vfrust_vrts_extra;
	bool infinites_found = false;
	for (std::vector< Vector2 >::const_iterator vrts_it = vfrust.vrts.begin();
	     vrts_it != vfrust.vrts.end();
	     ++ vrts_it) {
		Vector2 const& vrt = *vrts_it;
		Vector3 diff = vfrust.dir + vfrust.right * vrt.x + vfrust.up * vrt.y;
		// Check if this vertex would go to infinity
		if (dotProduct(diff, dir) <= 0) {
			infinites_found = true;
			continue;
		}

		// First move vertex to the same plane with other vertices.
		Vector3 vrt_newpos_abs = rayPlaneIntersection(pos, diff, ppos, dir);

		// Then calculate it's position in the space of plane
		Vector2 vrt_newpos = transformPointToTrianglespace(vrt_newpos_abs - ppos, right, up);

		if (!infinites_found) {
			vfrust_vrts.push_back(vrt_newpos);
		} else {
			vfrust_vrts_extra.push_back(vrt_newpos);
		}
	}
	vfrust_vrts.insert(vfrust_vrts.end(), vfrust_vrts_extra.begin(), vfrust_vrts_extra.end());

HppAssert(false, "Not implemented yet!");
// TODO: Code this!
return Viewfrustum(EMPTY, pos);
}

inline Viewfrustum::VFResult Viewfrustum::bsInFront(Boundingsphere const& bs, Vector3 const& pos, Vector3 const& nrm)
{
	HppAssert(!bs.isInfinite(), "Cannot be infinite!");
	HppAssert(nrm.lengthTo2() > 0.999 && nrm.lengthTo2() < 1.001, "Not normalized!");
	Real dst = distanceToPlane(pos, nrm, bs.getPosition()) + bs.getRadius();
	if (dst < 0) {
		return OUTSIDE;
	}
	dst -= bs.getRadius()*2;
	if (dst < 0) {
		return PARTIALLY_INSIDE;
	}
	return INSIDE;
}

inline bool Viewfrustum::pointInFront(Vector3 const& point, Vector3 const& pos, Vector3 const& nrm)
{
	HppAssert(nrm.lengthTo2() > 0.999 && nrm.lengthTo2() < 1.001, "Not normalized!");
	Real dst = distanceToPlane(pos, nrm, point);
	return dst >= 0;
}

}

#endif
