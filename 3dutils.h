#ifndef HPP_3DUTILS_H
#define HPP_3DUTILS_H

#include "angle.h"
#include "vector2.h"
#include "vector3.h"
#include "trigon.h"
#include "assert.h"

namespace Hpp
{

inline Real dotProduct(Vector2 const& v1, Vector2 const& v2);
inline Real dotProduct(Vector3 const& v1, Vector3 const& v2);
inline Vector3 crossProduct(Vector3 const& v1, Vector3 const& v2);
inline Real crossProduct(Vector2 const& v1, Vector2 const& v2);

inline Angle angleBetweenVectors(Vector3 const& v1, Vector3 const& v2);
inline Angle angleBetweenVectors(Vector2 const& v1, Vector2 const& v2);

// Forces angle between vectors to be 90°.
inline void forceVectorsPerpendicular(Vector3& v1, Vector3& v2);

inline Vector3 posToPlane(Vector3 const& pos, Vector3 const& plane_pos, Vector3 const& plane_nrm);

// Get directions of camera. By default, the camera looks towards positive
// Y-axis with it's up vector being positive Z-axis. Rotations are done by
// right hand method.
inline void getCameraDirections(Angle const& yaw, Angle const& pitch, Angle const& roll,
                                Vector3* dir,
                                Vector3* up = NULL,
                                Vector3* right = NULL,
                                Vector3* back = NULL,
                                Vector3* down = NULL,
                                Vector3* left = NULL);

// Calculates FOV from aspect ratio and from another
// FOV. Aspecratio is calculated width / height.
inline Angle calculateFovXFromAspectRatio(Angle const& fov_y, Real aspectratio);
inline Angle calculateFovYFromAspectRatio(Angle const& fov_x, Real aspectratio);

// Returns distance to plane. If point is at the back side of plane, then
// distance is negative. Note, that distance is measured in length of
// plane_normal, so if you want it to be measured in basic units, then
// normalize plane_normal!
// TODO: Put point parameter first!
inline Real distanceToPlane(Vector3 const& plane_pos, Vector3 const& plane_normal, Vector3 const& point);

// Distance is negative if point is at the "left side" of ray. Sides of ray are
// measured in space where X is right and Y is up and front of ray points to
// ray_dir.
inline Real distanceToRay(Vector2 const& point, Vector2 const& ray_begin, Vector2 const& ray_dir);

// Calculates nearest point between line/ray and a point. It is possible to get
// the nearest point in two ways and it is possible to get the distance between
// nearest and given points. The nearest point is defined in these ways:
// nearest_point = line_pos1 + (line_pos2 - line_pos1) * m = ray_begin + ray_dir * m
// Note, that these functions assume that line/ray is infinite long from both ends!
inline void nearestPointToLine(Vector3 const& point,
                               Vector3 const& line_pos1, Vector3 const& line_pos2,
                               Vector3* nearest_point, Real* m, Real* dst_to_point);
inline void nearestPointToRay(Vector3 const& point,
                              Vector3 const& ray_begin, Vector3 const& ray_dir,
                              Vector3* nearest_point, Real* m, Real* dst_to_point);

// Transforms a 3D or 2D point at the plane of triangle to the space of it so
// that the point can be defined using axes of triangle. This can be used to
// check if point is inside triangle or not. Note, that position is measured so
// that triangle vertex where axes start from, is origin.
inline Vector2 transformPointToTrianglespace(Vector3 const& pos, Vector3 const& x_axis, Vector3 const& y_axis);

// Calculates the position where ray intersects with plane. The result may also
// be behind ray begin.
inline Vector3 rayPlaneIntersection(Vector3 const& ray_begin,
                                    Vector3 const& ray_dir,
                                    Vector3 const& plane_pos,
                                    Vector3 const& plane_nrm);

// Checks if linesegments intersects and store collision point if required
inline bool linesegmentsIntersect(Vector2 const& ls0_begin,
                                  Vector2 const& ls0_end,
                                  Vector2 const& ls1_begin,
                                  Vector2 const& ls1_end);
inline bool linesegmentsIntersect(Vector2 const& ls0_begin,
                                  Vector2 const& ls0_end,
                                  Vector2 const& ls1_begin,
                                  Vector2 const& ls1_end,
                                  Vector2& collisionpoint);

inline bool triangleHitsRay(Vector3 const& begin,
                            Vector3 const& dir,
                            Vector3 const& v0,
                            Vector3 const& v1,
                            Vector3 const& v2,
                            Vector3* hit_pos = NULL,
                            Vector3 const* normal = NULL);



// ----------------------------------------
// ----------------------------------------
//
// Implementation of inline functions
//
// ----------------------------------------
// ----------------------------------------

inline Real dotProduct(Vector2 const& v1, Vector2 const& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

inline Real dotProduct(Vector3 const& v1, Vector3 const& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector3 crossProduct(Vector3 const& v1, Vector3 const& v2)
{
	return Vector3(v1.y * v2.z - v1.z * v2.y,
	               v1.z * v2.x - v1.x * v2.z,
	               v1.x * v2.y - v1.y * v2.x);
}

inline Real crossProduct(Vector2 const& v1, Vector2 const& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

inline Angle angleBetweenVectors(Vector3 const& v1, Vector3 const& v2)
{
	Real v1_len = v1.length();
	if (v1_len == 0) return Angle::fromRadians(0);
	Real v2_len = v2.length();
	if (v2_len == 0) return Angle::fromRadians(0);

	Vector3 v1_n = v1 / v1_len;
	Vector3 v2_n = v2 / v2_len;

	Real radians = static_cast< Real >(::acos(dotProduct(v1_n, v2_n)));
	if (!::finite(radians)) {
		if ((v1_n + v2_n).length() > 1.0) {
			return Angle::fromRadians(0);
		} else {
			return Angle::fromDegrees(180);
		}
	}
	return Angle::fromRadians(radians);

}

inline Angle angleBetweenVectors(Vector2 const& v1, Vector2 const& v2)
{
	Real v1_len = v1.length();
	if (v1_len == 0) return Angle::fromRadians(0);
	Real v2_len = v2.length();
	if (v2_len == 0) return Angle::fromRadians(0);

	Vector2 v1_n = v1 / v1_len;
	Vector2 v2_n = v2 / v2_len;

	Real radians = static_cast< Real >(::acos(dotProduct(v1_n, v2_n)));
	if (!::finite(radians)) {
		if ((v1_n + v2_n).length() > 1.0) {
			return Angle::fromRadians(0);
		} else {
			return Angle::fromDegrees(180);
		}
	}
	return Angle::fromRadians(radians);

}

inline void forceVectorsPerpendicular(Vector3& v1, Vector3& v2)
{
	Vector3 center = (v1 + v2) / 2.0;
	Vector3 to_v1 = v1 - center;
	Vector3 to_v2 = v2 - center;
	Real dp_tov1_tov2 = dotProduct(to_v1, to_v2);
	Real dp_tov1_c = dotProduct(to_v1, center);
	Real dp_tov2_c = dotProduct(to_v2, center);
	Real dp_c_c = dotProduct(center, center);
	Real a = dp_tov1_tov2;
	Real b = dp_tov1_c + dp_tov2_c;
	Real c = dp_c_c;
	HppAssert(a != 0.0, "Divizion by zero!");
	HppAssert(b*b - 4*a*c >= 0.0, "Sqrt cannot take negative numbers!");
	Real sqrt_result = sqrt(b*b - 4*a*c);
	Real m1 = (-b + sqrt_result) / (2.0 * a);
	Real m2 = (-b - sqrt_result)	 / (2.0 * a);
	if (m1 > 0.0) {
		v1 = center + m1 * to_v1;
		v2 = center + m1 * to_v2;
	} else {
		HppAssert(m2 > 0.0, "Impossible situation!");
		v1 = center + m2 * to_v1;
		v2 = center + m2 * to_v2;
	}
}

inline Vector3 posToPlane(Vector3 const& pos, Vector3 const& plane_pos, Vector3 const& plane_nrm)
{
	HppAssert(dotProduct(plane_nrm, plane_nrm) != 0, "Plane normal is too small!");
	return pos + plane_nrm * ((dotProduct(plane_pos, plane_nrm) - dotProduct(pos, plane_nrm)) / dotProduct(plane_nrm, plane_nrm));
}

inline void getCameraDirections(Angle const& yaw, Angle const& pitch, Angle const& roll,
                                Vector3* dir,
                                Vector3* up,
                                Vector3* right,
                                Vector3* back,
                                Vector3* down,
                                Vector3* left)
{
	Real yaw_s = yaw.sin();
	Real yaw_c = yaw.cos();
	Real pitch_s = pitch.sin();
	Real pitch_c = pitch.cos();
	Real roll_s = roll.sin();
	Real roll_c = roll.cos();

	if (dir) {
		dir->x = pitch_c * -yaw_s;
		dir->y = pitch_c * yaw_c;
		dir->z = pitch_s;
		if (back) {
			*back = -*dir;
		}
	} else if (back) {
		back->x = -pitch_c * -yaw_s;
		back->y = -pitch_c * yaw_c;
		back->z = -pitch_s;
	}

	if (right) {
		right->x = yaw_c * roll_c - yaw_s * roll_s * pitch_s;
		right->y = yaw_s * roll_c + yaw_c * roll_s * pitch_s;
		right->z = -roll_s * pitch_c;
		if (left) {
			*left = -*right;
		}
	} else if (left) {
		left->x = -(yaw_c * roll_c - yaw_s * roll_s * pitch_s);
		left->y = -(yaw_s * roll_c + yaw_c * roll_s * pitch_s);
		left->z = -(-roll_s * pitch_c);
	}

	if (up) {
		up->x = yaw_c * roll_s + yaw_s * -pitch_s * -roll_c;
		up->y = yaw_s * roll_s + yaw_c * pitch_s * -roll_c;
		up->z = pitch_c * roll_c;
		if (down) {
			*down = -*up;
		}
	} else if (down) {
		down->x = -(yaw_c * roll_s + yaw_s * -pitch_s * -roll_c);
		down->y = -(yaw_s * roll_s + yaw_c * pitch_s * -roll_c);
		down->z = -(pitch_c * roll_c);
	}
}

inline Angle calculateFovXFromAspectRatio(Angle const& fov_y, Real aspectratio)
{
	return atan(((fov_y / 2.0).tan() * aspectratio)) * 2.0;
}

inline Angle calculateFovYFromAspectRatio(Angle const& fov_x, Real aspectratio)
{
	return atan(((fov_x / 2.0).tan() / aspectratio)) * 2.0;
}

inline Real distanceToPlane(Vector3 const& plane_pos, Vector3 const& plane_normal, Vector3 const& point)
{
	Real dp_nn = dotProduct(plane_normal, plane_normal);
	return (dotProduct(plane_normal, point) - dotProduct(plane_normal, plane_pos)) / dp_nn;
}

inline Real distanceToRay(Vector2 const& point, Vector2 const& ray_begin, Vector2 const& ray_dir)
{
	HppAssert(dotProduct(ray_dir, ray_dir) != 0, "Ray dir cannot be zero!");
	Vector2 ray_dir_n = ray_dir.normalized();
	return dotProduct(ray_dir_n.perp(), ray_begin - point);
}

inline void nearestPointToLine(Vector3 const& point,
                               Vector3 const& line_pos1, Vector3 const& line_pos2,
                               Vector3* nearest_point, Real* m, Real* dst_to_point)
{
	nearestPointToRay(point, line_pos1, line_pos2 - line_pos1, nearest_point, m, dst_to_point);
}

inline void nearestPointToRay(Vector3 const& point,
                              Vector3 const& ray_begin, Vector3 const& ray_dir,
                              Vector3* nearest_point, Real* m, Real* dst_to_point)
{
	Real dp_rd_rd = dotProduct(ray_dir, ray_dir);
	HppAssert(dp_rd_rd != 0.0, "Division by zero!");
	Real m2 = dotProduct(ray_dir, point - ray_begin) / dp_rd_rd;
	// Store results
	if (m) {
		*m = m2;
	}
	if (nearest_point) {
		*nearest_point = ray_begin + ray_dir * m2;
		if (dst_to_point) {
			*dst_to_point = (*nearest_point - point).length();
		}
	} else if (dst_to_point) {
		*dst_to_point = ((ray_begin + ray_dir * m2) - point).length();
	}
}

inline Vector2 transformPointToTrianglespace(Vector3 const& pos, Vector3 const& x_axis, Vector3 const& y_axis)
{
// TODO: This solution is buggy! The det goes to zero if axis are (0, 1, 1) and (-1, 1, 1)!
// TODO: Merge these two methods? The first one gives more error and the second one takes 23% more time.
/*
	Real det = x_axis.x * y_axis.y
	         + y_axis.x * x_axis.z
	         + x_axis.y * y_axis.z
	         - x_axis.x * y_axis.z
	         - y_axis.x * x_axis.y
	         - y_axis.y * x_axis.z;
	if (det == 0) {
		return Vector2::INF;
	}

	return Vector2(( y_axis.y-y_axis.z)*pos.x + (-y_axis.x+y_axis.z)*pos.y + ( y_axis.x-y_axis.y)*pos.z,
	               (-x_axis.y+x_axis.z)*pos.x + ( x_axis.x-x_axis.z)*pos.y + (-x_axis.x+x_axis.y)*pos.z) / det;
*/

	// Calculate helper vector that is in 90 degree against y_axis.
	Vector3 helper = crossProduct(crossProduct(x_axis, y_axis), y_axis);
	Vector2 result;

	Real dp_xh = dotProduct(x_axis, helper);
	HppAssert(dp_xh != 0, "Division by zero!");
	result.x = dotProduct(pos, helper) / dp_xh;

	Vector3 y_axis_abs(std::fabs(y_axis.x), std::fabs(y_axis.y), std::fabs(y_axis.z));
	if (y_axis_abs.x > y_axis_abs.y &&
	    y_axis_abs.x > y_axis_abs.z) {
		HppAssert(y_axis.x != 0, "Division by zero!");
		result.y = (pos.x + -result.x * x_axis.x) / y_axis.x;
	} else if (y_axis_abs.y > y_axis_abs.z) {
		HppAssert(y_axis.y != 0, "Division by zero!");
		result.y = (pos.y + -result.x * x_axis.y) / y_axis.y;
	} else {
		HppAssert(y_axis.z != 0, "Division by zero!");
		result.y = (pos.z + -result.x * x_axis.z) / y_axis.z;
	}

	return result;
}

inline Vector3 rayPlaneIntersection(Vector3 const& ray_begin,
                                    Vector3 const& ray_dir,
                                    Vector3 const& plane_pos,
                                    Vector3 const& plane_nrm)
{
	HppAssert(ray_dir.length() > 0.0, "Segment cannot be zero sized!");
	HppAssert(dotProduct(ray_dir, plane_nrm) != 0.0, "This ray will never hit plane!");
	HppAssert(plane_nrm.length() > 0.0, "Plane normal cannot have zero length!");
	Real m = (dotProduct(plane_pos, plane_nrm) - dotProduct(ray_begin, plane_nrm)) / dotProduct(ray_dir, plane_nrm);
	return ray_begin + ray_dir * m;
}

inline bool linesegmentsIntersect(Vector2 const& ls0_begin,
                                  Vector2 const& ls0_end,
                                  Vector2 const& ls1_begin,
                                  Vector2 const& ls1_end)
{
	// Check if bounding boxes touch
	Vector2 ls0_bb_min(std::min(ls0_begin.x, ls0_end.x), std::min(ls0_begin.y, ls0_end.y));
	Vector2 ls0_bb_max(std::max(ls0_begin.x, ls0_end.x), std::max(ls0_begin.y, ls0_end.y));
	Vector2 ls1_bb_min(std::min(ls1_begin.x, ls1_end.x), std::min(ls1_begin.y, ls1_end.y));
	Vector2 ls1_bb_max(std::max(ls1_begin.x, ls1_end.x), std::max(ls1_begin.y, ls1_end.y));
	if ((ls1_bb_min.x >= ls0_bb_min.x && ls1_bb_min.x <= ls0_bb_max.x) ||
	    (ls1_bb_max.x >= ls0_bb_min.x && ls1_bb_max.x <= ls0_bb_max.x) ||
	    (ls1_bb_min.x <= ls0_bb_min.x && ls1_bb_max.x >= ls0_bb_max.x)) {
		if ((ls1_bb_min.y >= ls0_bb_min.y && ls1_bb_min.y <= ls0_bb_max.y) ||
		    (ls1_bb_max.y >= ls0_bb_min.y && ls1_bb_max.y <= ls0_bb_max.y) ||
		    (ls1_bb_min.y <= ls0_bb_min.y && ls1_bb_max.y >= ls0_bb_max.y)) {
		} else {
			return false;
		}
	} else {
		return false;
	}
	Vector2 ls0 = ls0_end - ls0_begin;
	bool cp0a_neg = (crossProduct(ls0, ls1_begin - ls0_begin) < 0);
	bool cp0b_neg = (crossProduct(ls0, ls1_end - ls0_begin) < 0);
	if (cp0a_neg == cp0b_neg) {
		return false;
	}
	Vector2 ls1 = ls1_end - ls1_begin;
	bool cp1a_neg = (crossProduct(ls1, ls0_begin - ls1_begin) < 0);
	bool cp1b_neg = (crossProduct(ls1, ls0_end - ls1_begin) < 0);
	if (cp1a_neg == cp1b_neg) {
		return false;
	}
	return true;
}

inline bool linesegmentsIntersect(Vector2 const& ls0_begin,
                                  Vector2 const& ls0_end,
                                  Vector2 const& ls1_begin,
                                  Vector2 const& ls1_end,
                                  Vector2& collisionpoint)
{
	// Check if bounding boxes touch
	Vector2 ls0_bb_min(std::min(ls0_begin.x, ls0_end.x), std::min(ls0_begin.y, ls0_end.y));
	Vector2 ls0_bb_max(std::max(ls0_begin.x, ls0_end.x), std::max(ls0_begin.y, ls0_end.y));
	Vector2 ls1_bb_min(std::min(ls1_begin.x, ls1_end.x), std::min(ls1_begin.y, ls1_end.y));
	Vector2 ls1_bb_max(std::max(ls1_begin.x, ls1_end.x), std::max(ls1_begin.y, ls1_end.y));
	if ((ls1_bb_min.x >= ls0_bb_min.x && ls1_bb_min.x <= ls0_bb_max.x) ||
	    (ls1_bb_max.x >= ls0_bb_min.x && ls1_bb_max.x <= ls0_bb_max.x) ||
	    (ls1_bb_min.x <= ls0_bb_min.x && ls1_bb_max.x >= ls0_bb_max.x)) {
		if ((ls1_bb_min.y >= ls0_bb_min.y && ls1_bb_min.y <= ls0_bb_max.y) ||
		    (ls1_bb_max.y >= ls0_bb_min.y && ls1_bb_max.y <= ls0_bb_max.y) ||
		    (ls1_bb_min.y <= ls0_bb_min.y && ls1_bb_max.y >= ls0_bb_max.y)) {
		} else {
			return false;
		}
	} else {
		return false;
	}
	// Find collisionposition at ray
	Vector2 ls0 = ls0_end - ls0_begin;
	Vector2 ls1 = ls1_end - ls1_begin;

	Vector2 ls0_perp = ls0.perp();
	Real dp_ls0p_ls1 = dotProduct(ls0_perp, ls1);
	if (dp_ls0p_ls1 == 0) {
		return false;
	}

	Real n = (dotProduct(ls0_perp, ls0_begin) - dotProduct(ls0_perp, ls1_begin)) / dp_ls0p_ls1;
	if (n < 0 || n > 1) {
		return false;
	}
	collisionpoint = ls1_begin + ls1 * n;

	Vector2 ls0_abs(fabs(ls0.x), fabs(ls0.y));
	Real m;
	if (ls0_abs.x > ls0_abs.y) {
		m = (collisionpoint.x - ls0_begin.x) / ls0.x;
	} else {
		if (ls0_begin.y == 0) {
			return false;
		}
		m = (collisionpoint.y - ls0_begin.y) / ls0.y;
	}
	if (m < 0 || m > 1) {
		return false;
	}
	return true;
}

inline bool triangleHitsRay(Vector3 const& begin,
                            Vector3 const& dir,
                            Vector3 const& v0,
                            Vector3 const& v1,
                            Vector3 const& v2,
                            Vector3* hit_pos,
                            Vector3 const* normal)
{
	HppAssert(dir.length() > 0.0, "Dir can not be zero!");

	Vector3 tedge0 = v1 - v0;
	Vector3 tedge1 = v2 - v0;

	// Calculate normal of plane of triangle.
	Vector3 plane_nrm;
	if (normal) {
		plane_nrm = *normal;
	} else {
		plane_nrm = crossProduct(tedge0, tedge1);
	}

	// Calculate the position at where linesegment hits the plane of
	// triangle.
	Real dp_e_n = dotProduct(dir, plane_nrm);
	if (dp_e_n == 0) {
		return false;
	}
	Real m = (dotProduct(v0, plane_nrm) - dotProduct(begin, plane_nrm)) / dp_e_n;

	// If collision point is behind ray, then collision is not possible.
	if (m < 0.0) {
		return false;
	}

	Vector3 cpos = begin + dir*m;

	// Find out the position in the coordinates of the plane of triangle.
	Vector2 pos_at_plane = transformPointToTrianglespace(cpos - v0, tedge0, tedge1);

	if (pos_at_plane.x < 0 ||
	    pos_at_plane.y < 0 ||
	    pos_at_plane.x + pos_at_plane.y > 1) {
		return false;
	}

	if (hit_pos) {
		*hit_pos = cpos;
	}

	return true;
}

}

#endif
