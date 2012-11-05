#ifndef HPP_COLLISIONS_H
#define HPP_COLLISIONS_H

#include "vector2.h"
#include "vector3.h"
#include "3dutils.h"
#include "transform2d.h"

#include <vector>

namespace Hpp
{

// Type and container for collisions. Length of normal is always 1.
struct Collision
{
	inline Collision(void) { }
	inline Collision(Vector3 const& normal, Real depth) :
	normal(normal),
	depth(depth)
	{
	}
	Vector3 normal;
	Real depth;
};
typedef std::vector< Collision > Collisions;
struct Collision2D
{
	Vector2 normal;
	Real depth;
};
typedef std::vector< Collision2D > Collisions2D;

typedef std::vector< Vector2 > Collisionpolygon;

// Collisions are measured so that cp1 is seen as static that cannot move and
// cp2 is the polygon that needs to move to remove collisions.
inline void addCollisions(Collisions2D& result, Collisionpolygon const& cp1, Collisionpolygon const& cp2, Real extra_radius);

// Removes extra radius from all collisions
inline void removeExtraRadius(Collisions& colls, Real extra_radius);
inline void removeExtraRadius(Collisions2D& colls, Real extra_radius);

// Calculates position delta to get object out of walls. Note, that this
// function invalidates depth values of collisions and removes those collisions
// that does not really hit walls.
inline Vector3 moveOut(Collisions& colls);
inline Vector2 moveOut(Collisions2D& colls);

inline void removeExtraRadius(Collisions& colls, Real extra_radius)
{
	for (Collisions::iterator colls_it = colls.begin();
	     colls_it != colls.end();
	     colls_it ++) {
		colls_it->depth -= extra_radius;
	}
}

inline void removeExtraRadius(Collisions2D& colls, Real extra_radius)
{
	for (Collisions2D::iterator colls_it = colls.begin();
	     colls_it != colls.end();
	     colls_it ++) {
		colls_it->depth -= extra_radius;
	}
}

inline Vector3 moveOut(Collisions& colls)
{
	if (colls.empty()) {
		return Vector3::ZERO;
	}

	Vector3 result;
	Collisions real_colls;

	// Find out what collisions is the deepest one
	size_t deepest;
	#ifndef NDEBUG
	deepest = 0;
	#endif
	Real deepest_depth = -999999;
	for (size_t colls_id = 0;
	     colls_id < colls.size();
	     colls_id ++) {
		Collision& coll = colls[colls_id];
		HppAssert(coll.normal.lengthTo2() > 0.999 && coll.normal.lengthTo2() < 1.001, "Normal is not normalized!");
		if (coll.depth > deepest_depth) {
			deepest_depth = coll.depth;
			deepest = colls_id;
		}
	}

	Collision& coll_d = colls[deepest];

	// First move the object out using the deepest collision
	if (deepest_depth >= 0.0) {
		result = coll_d.normal * deepest_depth;
		real_colls.push_back(coll_d);
	} else {
		colls.clear();
		return Vector3::ZERO;
	}

	// If this was the only collision, then do nothing else
	if (colls.size() == 1) {
		return result;
	}

	Real dp_cdn_cdn = dotProduct(coll_d.normal, coll_d.normal);
	HppAssert(dp_cdn_cdn != 0.0, "Division by zero!");
	// Go rest of collisions through and check how much they should be
	// moved so object would come out of wall. Note, that since one
	// collision is already fixed, all other movings must be done at the
	// plane of that collision!
	size_t deepest2;
	#ifndef NDEBUG
	deepest2 = 0;
	#endif
	Real deepest2_depth = -999999;
	Vector3 deepest2_nrm_p;
	for (size_t colls_id = 0;
	     colls_id < colls.size();
	     colls_id ++) {
		// Skip deepest collision
		if (colls_id == deepest) {
			continue;
		}
		Collision& coll = colls[colls_id];

		// Since object has already moved, depth of other collisions
		// have changed. Recalculate depth now.
		Real dp_nn_nn = dotProduct(coll.normal, coll.normal);
		HppAssert(dp_nn_nn != 0.0, "Division by zero!");
		Real dp_r_n = dotProduct(result, coll.normal);
		coll.depth -= dp_r_n / dp_nn_nn;

		// Skip collisions that are not touching
		if (coll.depth <= 0.0005) {
			continue;
		}

		// Project normal to the plane of deepest collision. This must
		// be done in two steps. First direction at plane is
		// calculated, and then normal is projected to plane using it.
		Real dp_cdnn_cdnn = dotProduct(coll_d.normal, coll_d.normal);
		Real dp_cdnn_nn = dotProduct(coll_d.normal, coll.normal);
		HppAssert(dp_cdnn_cdnn != 0, "Division by zero!");
		Vector3 dir_at_plane = coll.normal - coll_d.normal * (dp_cdnn_nn / dp_cdnn_cdnn);
		dir_at_plane.normalize();
		// Second step
		Real dp_n_n = dotProduct(coll.normal, coll.normal) * coll.depth * coll.depth;
		Real dp_n_d = dotProduct(coll.normal, dir_at_plane) * coll.depth;
		if (fabs(dp_n_d) < 0.0005) {
			continue;
		}
		Vector3 move_at_plane = dir_at_plane * (dp_n_n / dp_n_d);

		Real depth = move_at_plane.length();
		if (depth > deepest2_depth) {
			deepest2_depth = depth;
			deepest2 = colls_id;
			deepest2_nrm_p = move_at_plane;
		}
	}

	// If no touching collisions were found, then mark all except deepest
	// as not real and leave.
	if (deepest2_depth <= 0.0) {
		colls.swap(real_colls);
		return result;
	}

	Collision& coll_d2 = colls[deepest2];
	real_colls.push_back(coll_d2);

	// Again, modify position using the second deepest collision
	HppAssert(fabs(dotProduct(deepest2_nrm_p, result)) < 0.005, "Second out moving is not done at the plane of the first one!");
	result += deepest2_nrm_p;

	// Go rest of collisions through and check how much they should be
	// moved so object would come out of wall. Note, that since two
	// collisions are already fixed, all other movings must be done at the
	// planes of these collisions! This means one line in space.
	Vector3 move_v = crossProduct(coll_d.normal, coll_d2.normal);
	HppAssert(move_v.lengthTo2() != 0.0, "Too small cross product!");
	move_v.normalize();
	Real deepest3_depth = -99999;
	Vector3 deepest3_move;
	#ifndef NDEBUG
	deepest3_move = Hpp::Vector3::ZERO;
	#endif
	for (size_t colls_id = 0;
	     colls_id < colls.size();
	     colls_id ++) {
		// Skip deepest collisions
		if (colls_id == deepest ||
		    colls_id == deepest2) {
			continue;
		}
		Collision& coll = colls[colls_id];

		// Since object has moved again, depth of other collisions
		// have changed. Recalculate depth now.
		Real dp_nn_nn = dotProduct(coll.normal, coll.normal);
		HppAssert(dp_nn_nn != 0.0, "Division by zero!");
		Real dp_cd2n_n = dotProduct(coll_d2.normal*coll_d2.depth, coll.normal);
		Real depthmod = dp_cd2n_n / dp_nn_nn;
		coll.depth -= depthmod;

		// Skip collisions that are not touching
		if (coll.depth <= 0.0005) {
			continue;
		}
		real_colls.push_back(coll);

		// Project normal to the move vector. If vectors are in 90°
		// against each others, then this collision must be abandon,
		// because we could never move along move_v to undo this
		// collision.
		Vector3 coll_real = coll.normal * coll.depth;
		Real dp_c_mv = dotProduct(coll_real, move_v);
		if (fabs(dp_c_mv) > 0.0005) {
			Real dp_c_c = dotProduct(coll_real, coll_real);
			Vector3 projected = move_v * (dp_c_c / dp_c_mv);
			if (projected.length() > deepest3_depth) {
				deepest3_depth = projected.length();
				deepest3_move = projected;
			}
		}
	}

	// Now move position for final time
	if (deepest3_depth > 0.0) {
		result += deepest3_move;
	}

	colls.swap(real_colls);
	return result;
}

inline Vector2 moveOut(Collisions2D& colls)
{
	if (colls.empty()) {
		return Vector2::ZERO;
	}

	Vector2 result;

	// Find out what collisions is the deepest one
	size_t deepest = 0;
	Real deepest_depth = colls[0].depth;
	for (size_t colls_id = 1;
	     colls_id < colls.size();
	     colls_id ++) {
		Collision2D& coll = colls[colls_id];
		HppAssert(coll.normal.lengthTo2() > 0.999 && coll.normal.lengthTo2() < 1.001, "Normal is not normalized!");
		if (coll.depth > deepest_depth) {
			deepest_depth = coll.depth;
			deepest = colls_id;
		}
	}

	Collision2D& coll_d = colls[deepest];

	// First move the object out using the deepest collision
	if (deepest_depth >= 0.0) {
		result = coll_d.normal * deepest_depth;
	} else {
		colls.clear();
		return Vector2::ZERO;
	}

	// If this was the only collision, then do nothing else
	if (colls.size() == 1) {
		return result;
	}

	// Current position delta also affects to other collisions, so now go
	// them through, and check how their depth changes. Because one
	// collision is already fixed, the rest of collisions must be fixed by
	// going along a vector that is perpendicular to the normal of fixed
	// collision. That is why the new depths are measured along that vector.
	Vector2 fix2v = coll_d.normal.perp();
	size_t deepest2;
	#ifndef NDEBUG
	deepest2 = 0;
	#endif
	Real deepest2_depth = -1;
	for (size_t colls_id = 0;
	     colls_id < colls.size();
	     colls_id ++) {
		// Skip deepest collision
		if (colls_id == deepest) {
			continue;
		}
		Collision2D& coll = colls[colls_id];
		// Calculate new depth
		Real dp_n_n = dotProduct(coll.normal, coll.normal);
		HppAssert(dp_n_n != 0.0, "Divizion by zero!");
		coll.depth -= dotProduct(result, coll.normal) / dp_n_n;

		// If this collision has negative depth, ie is not touching the
		// other object, then remove it from container.
// TODO: What is good value here?
		if (coll.depth <= 0.0005) {
			coll = colls.back();
			colls.pop_back();
			continue;
		}

		// Remember that it is mandatory to do the fixings of these
		// collisions along that vector(fix2v) that was perpendicular
		// to the normal of already fixed collision? Because of this,
		// the new depth is not the one we want. From now on, the depth
		// will be measured using vector fix2v. Also, the negativity of
		// depth no longer tells if collision is really touching or not.
		Real dp_n_f2v = dotProduct(coll.normal, fix2v);
		// If this is zero, then it means that this collision has almost
		// the same normal, as the already fixed collision. In this
		// case, this does not need fixing, so skip it!
		if (dp_n_f2v <= 0.0) continue;
		// Calculate how much fix2v should be added, so this collision
		// would be fixed.
		coll.depth = (dp_n_n * coll.depth) / dp_n_f2v;

		// Check if this is the "deepest" one
		Real coll_depth_abs = fabs(coll.depth);
		if (coll_depth_abs > deepest2_depth) {
			deepest2_depth = coll_depth_abs;
			deepest2 = colls_id;
		}
	}

	// If new "deepest" collision was found, then fix along fix2v
	if (deepest2_depth > 0.0) {
		result += fix2v * colls[deepest2].depth;
	}

	return result;
}

inline void addCollisions(Collisions2D& result, Collisionpolygon const& cp1, Collisionpolygon const& cp2, Real extra_radius)
{
	// Go all lines of cp2 through and check if they collide to lines of cp1
	for (size_t cp2_it = 0; cp2_it < cp2.size(); cp2_it ++) {
		Vector2 cp2_v0 = cp2[cp2_it];
		Vector2 cp2_v1 = cp2[(cp2_it + 1) % cp2.size()];

		for (size_t cp1_it = 0; cp1_it < cp1.size(); cp1_it ++) {
			Vector2 cp1_v0 = cp1[cp1_it];
			Vector2 cp1_v1 = cp1[(cp1_it + 1) % cp1.size()];
			Vector2 cp1_l = cp1_v1 - cp1_v0;
			Vector2 cp2_l = cp2_v1 - cp2_v0;

			// Skip parallel lines
			if (fabs(dotProduct(cp1_l.perp(), cp2_l)) < 0.001) continue;

			// Check if these two collide and skip if not.
			Vector2 c;
			if (!linesegmentsIntersect(cp2_v0, cp2_v1, cp1_v0, cp1_v1, c)) {
				continue;
			}

			// Check which one of vertices is inside cp1.
			Vector2 const* p_in;
			Vector2 const* p2_in;
			Real cp = crossProduct(cp1_l, cp2_v0 - cp1_v0);
			if (cp > 0.0001) {
				p_in = &cp2_v0;
				p2_in = &cp1_v1;
			} else if (cp < -0.0001) {
				p_in = &cp2_v1;
				p2_in = &cp1_v0;
			} else {
				continue;
			}

			// Possibilities to get line out of another one.
			// First candidate: Move is perpendicular to line of cp1
			Vector2 cp1_perp_n = cp1_l.perp().normalized();
			Vector2 movecand0 = cp1_perp_n * distanceToRay(*p_in, cp1_v0, cp1_l);
			// Second candidate: Move is perpendicular to line of cp2
			Vector2 cp2_perp_n = cp2_l.perp().normalized();
			Vector2 movecand1 = cp2_perp_n * distanceToRay(*p2_in, cp2_v0, -cp2_l);

			Real movecand0_len = movecand0.length();
			Real movecand1_len = movecand1.length();

			if (movecand0_len < movecand1_len) {
				if (movecand0_len > 0) {
					Collision2D new_coll;
					new_coll.depth = movecand0_len + extra_radius;
					new_coll.normal = movecand0 / movecand0_len;
					result.push_back(new_coll);
				}
			} else if (movecand1_len > 0) {
				Collision2D new_coll;
				new_coll.depth = movecand1_len + extra_radius;
				new_coll.normal = movecand1 / movecand1_len;
				result.push_back(new_coll);
			}

		}
	}

}

}

#endif
