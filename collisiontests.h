#ifndef HPP_COLLISIONTESTS_H
#define HPP_COLLISIONTESTS_H

#include "3dutils.h"
#include "collisions.h"
#include "vector3.h"
#include "real.h"

namespace Hpp
{

namespace Collisiontests
{

inline bool sphereToTriangle(Collision& result,
                             Vector3 const& pos, Real radius,
                             Vector3 const& corner0, Vector3 const& corner1, Vector3 const& corner2,
                             Real extra_radius = -1);

inline bool capsuleToTriangle(Collision& result,
                              Vector3 const& pos0, Vector3 const& pos1, Real radius,
                              Vector3 const& corner0, Vector3 const& corner1, Vector3 const& corner2,
                              Real extra_radius = -1);

inline bool sphereToTriangle(Collision& result,
                             Vector3 const& pos, Real radius,
                             Vector3 const& corner0, Vector3 const& corner1, Vector3 const& corner2,
                             Real extra_radius)
{
	if (extra_radius < 0) {
		extra_radius = radius;
	}

	Vector3 coll_pos;
	if (!triangleHitsSphere(pos, radius + extra_radius,
	                        corner0, corner1, corner2,
	                        coll_pos, result.normal, result.depth)) {
		return false;
	}
	HppAssert(result.normal.length() > 0.99 && result.normal.length() < 1.01, "Normal is not normalized!");

	result.depth -= extra_radius;

	return true;
}

inline bool capsuleToTriangle(Collision& result,
                              Vector3 const& pos0, Vector3 const& pos1, Real radius,
                              Vector3 const& corner0, Vector3 const& corner1, Vector3 const& corner2,
                              Real extra_radius)
{
	if (extra_radius < 0) {
		extra_radius = radius;
	}

// TODO: Test middle cylinder too!
	if (sphereToTriangle(result, pos0, radius,
                             corner0, corner1, corner2,
                             extra_radius)) {
		return true;
	}
	return sphereToTriangle(result, pos1, radius,
                             corner0, corner1, corner2,
                             extra_radius);
}

}

}

#endif

