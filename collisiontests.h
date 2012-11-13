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

// Collisions are returned so that normal points towards first object.
// If extra_radius is not given, then it is set to same as the biggest
// radius. Returns true if collision occurs. Note, that every function
// may modify result, even when collision does not happen!
inline bool sphereToSphere(Collision& result,
                           Vector3 pos0, Real radius0,
                           Vector3 pos1, Real radius1,
                           Real extra_radius = -1);
inline bool sphereToTriangle(Collision& result,
                             Vector3 const& pos, Real radius,
                             Vector3 const& corner0, Vector3 const& corner1, Vector3 const& corner2,
                             Real extra_radius = -1);
inline bool capsuleToTriangle(Collision& result,
                              Vector3 const& pos0, Vector3 const& pos1, Real radius,
                              Vector3 const& corner0, Vector3 const& corner1, Vector3 const& corner2,
                              Real extra_radius = -1);

inline bool sphereToSphere(Collision& result,
                           Vector3 pos0, Real radius0,
                           Vector3 pos1, Real radius1,
                           Real extra_radius)
{
	if (extra_radius < 0) {
		extra_radius = std::max(radius0, radius1);
	}
	result.normal = pos0 - pos1;
	Real diff_len_to_2 = result.normal.lengthTo2();
	Real coll_distance = radius0 + radius1 + extra_radius;
	if (diff_len_to_2 < coll_distance*coll_distance) {
		Real diff_len = ::sqrt(diff_len_to_2);
		HppAssert(diff_len != 0, "Division by zero!");
		result.normal /= diff_len;
		result.depth = radius0 + radius1 - diff_len;
		return true;
	}
	return false;
}

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
// TODO: Use boundingsphere!
	if (extra_radius < 0) {
		extra_radius = radius;
	}

	Vector3 const diff = pos1 - pos0;

	// This is kind of hard shape, so go different kind of
	// collision types through and pick all collisions to
	// container. Finally deepest one of these is selected.
	Collisions ccolls;

	// Test first capsule cap
	if (sphereToTriangle(result, pos0, radius,
                             corner0, corner1, corner2,
                             extra_radius)) {
		ccolls.push_back(result);
	}

	// Test second capsule cap
	if (sphereToTriangle(result, pos1, radius,
                             corner0, corner1, corner2,
                             extra_radius)) {
		ccolls.push_back(result);
	}

	// Now check middle cylinder. Start from corners
	for (uint8_t corner_id = 0; corner_id < 3; ++ corner_id) {
		Hpp::Vector3 corner = (corner_id == 0) ? corner0 : ((corner_id == 1) ? corner1 : corner2);

		// Discard this corner, if its above or below cylinder
		if (dotProduct(diff, corner - pos0) < 0 ||
		    dotProduct(-diff, corner - pos1) < 0) {
			continue;
		}

		// Calculate depth
		Real distance_to_centerline;
		Vector3 point_at_centerline;
		nearestPointToLine(corner, pos0, pos1, &point_at_centerline, NULL, &distance_to_centerline);
		Real depth = radius - distance_to_centerline;

		if (depth + extra_radius > 0) {
			Collision new_ccoll;
			new_ccoll.depth = depth;
			new_ccoll.normal = (point_at_centerline - corner).normalized();
			ccolls.push_back(new_ccoll);
		}
		
	}

	// Then check if edges are inside cylinder
	for (uint8_t edge_id = 0; edge_id < 3; ++ edge_id) {
		Hpp::Vector3 begin = (edge_id == 0) ? corner0 : ((edge_id == 1) ? corner1 : corner2);
		Hpp::Vector3 end = (edge_id == 0) ? corner1 : ((edge_id == 1) ? corner2 : corner0);
		Vector3 edge = end - begin;

		// Is begin and end of edge inside or outside?
		// 0 = inside, -1 = below, 1 = outside.
		int8_t begin_outside, end_outside;
		if (dotProduct(diff, begin - pos0) < 0) begin_outside = -1;
		else if (dotProduct(-diff, begin - pos1) < 0) begin_outside = 1;
		else begin_outside = 0;
		if (dotProduct(diff, end - pos0) < 0) end_outside = -1;
		else if (dotProduct(-diff, end - pos1) < 0) end_outside = 1;
		else end_outside = 0;

		// Discard this edge, if its fully outside cylinder
		if ((begin_outside == -1 && end_outside == -1) ||
		    (begin_outside == 1 && end_outside == 1)) {
			continue;
		}

		// Get collision where edge enters cylinder from below
		if (begin_outside == -1) {
			Real dp_d_e = dotProduct(diff, edge);
			if (fabs(dp_d_e) > 0.0005) {
				Vector3 x = begin + edge * (dotProduct(diff, pos0) - dotProduct(diff, begin)) / dp_d_e;
				Real depth = radius - x.length();
				if (depth + extra_radius > 0) {
					Collision new_ccoll;
					new_ccoll.depth = depth;
					new_ccoll.normal = -x.normalized();
					ccolls.push_back(new_ccoll);
				}
			}
		}
		
		// Get collision where edge leaves cylinder from above
		if (end_outside == 1) {
			Real dp_d_e = dotProduct(diff, edge);
			if (fabs(dp_d_e) > 0.0005) {
				Vector3 x = end - edge * (dotProduct(-diff, pos1) - dotProduct(-diff, end)) / dp_d_e;
				Real depth = radius - x.length();
				if (depth + extra_radius > 0) {
					Collision new_ccoll;
					new_ccoll.depth = depth;
					new_ccoll.normal = -x.normalized();
					ccolls.push_back(new_ccoll);
				}
			}
		}

		// Get collision where edge is nearest
		// to the center line of the cylinder
		Vector3 centerline = pos1 - pos0;
		Vector3 point_at_centerline;
		Vector3 point_at_edge;
		Real dst = distanceBetweenLines(pos0, centerline,
		                                begin, edge,
		                                &point_at_centerline, &point_at_edge);
		// Add new collision if:
		// 1) Depth is not too small and
		// 2) nearest point is inside cylinder and at the edge
		Real depth = radius - dst;
		if (depth + extra_radius > 0 &&
		    dotProduct(centerline, point_at_centerline - pos0) > 0 &&
		    dotProduct(-centerline, point_at_centerline - pos1) > 0 &&
		    dotProduct(edge, point_at_edge - begin) > 0 &&
		    dotProduct(-edge, point_at_edge - end) > 0) {
			Collision new_ccoll;
			new_ccoll.depth = depth;
			new_ccoll.normal = (point_at_centerline - point_at_edge).normalized();
			ccolls.push_back(new_ccoll);
		}

		// Get collisions where cylinder hits triangle plane
// TODO: Code this!

	}

	if (!ccolls.empty()) {
		// Search the deepest collision
		Real deepest_depth = ccolls[0].depth;
		size_t deepest = 0;
		for (size_t ccoll_id = 1;
		     ccoll_id < ccolls.size();
		     ++ ccoll_id) {
			Collision const& ccoll = ccolls[ccoll_id];
			if (ccoll.depth > deepest_depth) {
				deepest_depth = ccoll.depth;
				deepest = ccoll_id;
			}
		}
		// Return result
		result = ccolls[deepest];

		return true;
	}

	return false;
}

}

}

#endif

