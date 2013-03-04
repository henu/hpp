#ifndef HPP_3DUTILS_H
#define HPP_3DUTILS_H

#include "angle.h"
#include "vector2.h"
#include "vector3.h"
#include "trigon.h"
#include "assert.h"
#include "constants.h"

#include <iostream>
namespace Hpp
{

inline Real dotProduct(Vector2 const& v1, Vector2 const& v2);
inline Real dotProduct(Vector3 const& v1, Vector3 const& v2);
inline Vector3 crossProduct(Vector3 const& v1, Vector3 const& v2);
inline Real crossProduct(Vector2 const& v1, Vector2 const& v2);

inline Angle angleBetweenVectors(Vector3 const& v1, Vector3 const& v2);
inline Angle angleBetweenVectors(Vector2 const& v1, Vector2 const& v2);

// Returns the amount of right hand rotation that needs to be added to vector
// #1 so it would become vector #2 when the rotation is done at given plane.
// The result is between [-180 - 180]. Note! Positions do not need to be at the
// given plane, the result is always their real angle in 3D space. The plane is
// used only to determine if the angle should be negative or positive.
inline Angle angleAtPlane(Vector3 const& v1, Vector3 const& v2, Vector3 const& normal);

// Returns angle from compontents of Vector2. Positive Y
// axis is 0°, negative X axis is 90° and so on. This means
// right hand rotation. Result is between [-180° and 180°).
inline Angle angleOfVector(Vector2 const& v);
inline Vector2 vectorOfAngle(Angle const& a);

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

// Calculates FOV from aspect ratio/screen size and from
// another FOV. FOV means angle between opposite viewfrustum
// planes. Aspectratio is calculated width / height.
inline Angle fovYToFovX(Angle const& fov_y, Real aspectratio);
inline Angle fovXToFovY(Angle const& fov_x, Real aspectratio);
inline Angle fovYToFovX(Angle const& fov_y, Real width, Real height);
inline Angle fovXToFovY(Angle const& fov_x, Real width, Real height);

// Returns distance to plane. If point is at the back side of plane, then
// distance is negative. Note, that distance is measured in length of
// plane_normal, so if you want it to be measured in basic units, then
// normalize plane_normal!
// TODO: Put point parameter first!
inline Real distanceToPlane(Vector3 const& plane_pos, Vector3 const& plane_normal, Vector3 const& point);
inline Real distanceToPlane(Vector2 const& plane_pos, Vector2 const& plane_normal, Vector2 const& point);

// Distance is negative if point is at the "left side" of ray. Sides of ray are
// measured in space where X is right and Y is up and front of ray points to
// ray_dir.
inline Real distanceToRay(Vector2 const& point, Vector2 const& ray_begin, Vector2 const& ray_dir);

// Calculates nearest point between line/ray and a point. It is possible to get
// the nearest point in two ways and it is possible to get the distance between
// nearest and given points. The nearest point is defined in these ways:
// nearest_point = line_pos1 + (line_pos2 - line_pos1)
// nearest_point = ray_begin + ray_dir * m
// Note, that these functions assume that line is infinite long from both ends
// but ray is infinite only from head!
inline void nearestPointToLine(Vector3 const& point,
                               Vector3 const& line_pos1, Vector3 const& line_pos2,
                               Vector3* nearest_point, Real* m, Real* dst_to_point);
inline void nearestPointToRay(Vector3 const& point,
                              Vector3 const& ray_begin, Vector3 const& ray_dir,
                              Vector3* nearest_point, Real* m, Real* dst_to_point);

inline Real distanceBetweenLines(Vector3 const& begin1, Vector3 const& dir1,
                                 Vector3 const& begin2, Vector3 const& dir2,
                                 Vector3* nearest_point1 = NULL, Vector3* nearest_point2 = NULL);

inline Real distanceBetweenRays(Vector3 const& begin1, Vector3 const& dir1,
                                Vector3 const& begin2, Vector3 const& dir2);

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

// Length of collision normal will be 1.
// TODO: Make it possible to not want collision information!
inline bool triangleHitsSphere(Vector3 const& pos, Real radius,
                               Vector3 const& tp0, Vector3 const& tp1, Vector3 const& tp2,
                               Vector3& coll_pos, Vector3& coll_nrm, Real& coll_depth,
                               Vector3 const& tri_bs_pos = Vector3::ZERO, Real tri_bs_r = -1.0);



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
			return Angle(180);
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
			return Angle(180);
		}
	}
	return Angle::fromRadians(radians);

}

inline Angle angleAtPlane(Vector3 const& v1, Vector3 const& v2, Vector3 const& normal)
{
	// Get rotation between vectors in case where there is no plane
	Angle angle = angleBetweenVectors(v1, v2);
	// Check which side the rotation is
	Real dp = dotProduct(normal, crossProduct(v1, v2));
	if (dp < 0) {
		angle = -angle;
	}
	return angle;
}

inline Angle angleOfVector(Vector2 const& v)
{
	Real rad;
	if (v.y > 0) {
		rad = ::atan(-v.x / v.y);
	} else if (v.y < 0) {
		if (v.x < 0) {
			rad = HPP_PI - ::atan(v.x / v.y);
		} else {
			rad = -HPP_PI - ::atan(v.x / v.y);
		}
	} else {
		if (v.x < 0) {
			rad = HPP_PI / 2.0;
		} else {
			rad = -HPP_PI / 2.0;
		}
	}
	return Angle::fromRadians(rad);
}

inline Vector2 vectorOfAngle(Angle const& a)
{
	Vector2 result;
	result.x = -a.sin();
	result.y = a.cos();
	return result;
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

inline Angle fovYToFovX(Angle const& fov_y, Real aspectratio)
{
	return atan((fov_y / 2.0).tan() * aspectratio) * 2.0;
}

inline Angle fovXToFovY(Angle const& fov_x, Real aspectratio)
{
	return atan((fov_x / 2.0).tan() / aspectratio) * 2.0;
}

inline Angle fovYToFovX(Angle const& fov_y, Real width, Real height)
{
	HppAssert(height != 0, "Division by zero!");
	return fovYToFovX(fov_y, width / height);
}

inline Angle fovXToFovY(Angle const& fov_x, Real width, Real height)
{
	HppAssert(height != 0, "Division by zero!");
	return fovXToFovY(fov_x, width / height);
}

inline Real distanceToPlane(Vector3 const& plane_pos, Vector3 const& plane_normal, Vector3 const& point)
{
	Real dp_nn = dotProduct(plane_normal, plane_normal);
	return (dotProduct(plane_normal, point) - dotProduct(plane_normal, plane_pos)) / dp_nn;
}

inline Real distanceToPlane(Vector2 const& plane_pos, Vector2 const& plane_normal, Vector2 const& point)
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
	Vector3 dir = line_pos2 - line_pos1;
	Real dp_rd_rd = dotProduct(dir, dir);
	HppAssert(dp_rd_rd != 0.0, "Line positions are too near each others!");
	Real m2 = dotProduct(dir, point - line_pos1) / dp_rd_rd;
	// Store results
	if (m) {
		*m = m2;
	}
	if (nearest_point) {
		*nearest_point = line_pos1 + dir * m2;
		if (dst_to_point) {
			*dst_to_point = (*nearest_point - point).length();
		}
	} else if (dst_to_point) {
		*dst_to_point = ((line_pos1 + dir * m2) - point).length();
	}
}

inline void nearestPointToRay(Vector3 const& point,
                              Vector3 const& ray_begin, Vector3 const& ray_dir,
                              Vector3* nearest_point, Real* m, Real* dst_to_point)
{
	// First check if point is behind the ray begin
	Real dp_rd_pd = dotProduct(ray_dir, point - ray_begin);
	if (dp_rd_pd < 0) {
		if (nearest_point) {
			*nearest_point = ray_begin;
		}
		if (m) {
			*m = 0;
		}
		if (dst_to_point) {
			*dst_to_point = (ray_begin - point).length();
		}
		return;
	}

	Real dp_rd_rd = dotProduct(ray_dir, ray_dir);
	HppAssert(dp_rd_rd != 0.0, "Ray direction is too short!");
	Real m2 = dp_rd_pd / dp_rd_rd;
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

inline Real distanceBetweenLines(Vector3 const& begin1, Vector3 const& dir1,
                                 Vector3 const& begin2, Vector3 const& dir2,
                                 Vector3* nearest_point1, Vector3* nearest_point2)
{
	Vector3 cp_d1_d2 = crossProduct(dir1, dir2);
	Real cp_d1_d2_len_to_2 = cp_d1_d2.lengthTo2();
	if (cp_d1_d2_len_to_2 < 0.000001) {
		Vector3 helper = posToPlane(begin2, begin1, dir1);
		if (nearest_point1) {
			*nearest_point1 = begin1;
		}
		if (nearest_point2) {
			*nearest_point2 = helper;
		}
		return (begin1 - helper).length();
	}
	Vector3 begin_diff = begin1 - begin2;
	Vector3 cp = crossProduct(begin_diff, cp_d1_d2 / cp_d1_d2_len_to_2);

	// Calculate nearest points, if needed
	if (nearest_point1) {
		*nearest_point1 = begin1 + dotProduct(cp, dir2) * dir1;
	}
	if (nearest_point2) {
		*nearest_point2 = begin2 + dotProduct(cp, dir1) * dir2;
	}

	// Calculate distance
	Real cp_d1_d2_len = ::sqrt(cp_d1_d2_len_to_2);
	Vector3 n = cp_d1_d2 / cp_d1_d2_len;
	return ::fabs(dotProduct(n, begin_diff));
}

inline Real distanceBetweenRays(Vector3 const& begin1, Vector3 const& dir1,
                                Vector3 const& begin2, Vector3 const& dir2)
{
	Real dp_d1_d2 = dotProduct(dir1, dir2);

	// Check on which side beginning of rays are compared to the plane
	// that other ray forms (begin as position and dir as normal).
	bool ray1_begins_front = dotProduct(begin1 - begin2, dir2) > 0;
	bool ray2_begins_front = dotProduct(begin2 - begin1, dir1) > 0;

	// Angle between rays is less than 90 °
	if (dp_d1_d2 > 0) {

		// If both rays begin from the backside of each others,
		// then this means that they will never get any closer.
		if (!ray1_begins_front && !ray2_begins_front) {
			return (begin1 - begin2).length();
		}

		// If both rays begin from the frontside of each
		// others, then it means they will get closer
		// and distance between lines can be used.
		if (ray1_begins_front && ray2_begins_front) {
			return distanceBetweenLines(begin1, dir1, begin2, dir2);
		}

		// If another begin is at frontside and another at
		// backside, then find the position where ray becomes
		// to frontside. If this ray is assumed to begin from
		// this new position and another ray is back of it, then
		// their possible nearest point is between old and new
		// begins of that ray that was actually backside.
		if (ray1_begins_front && !ray2_begins_front) {
			Real move_amount = (dotProduct(dir1, begin1) - dotProduct(dir1, begin2)) / dp_d1_d2;
			Vector3 new_begin = begin2 + dir2 * move_amount;
			// If ray #1 is at backside of new_begin,
			// then rays cannot get any closer.
			if (dotProduct(begin1 - new_begin, dir2) <= 0) {
				Real result;
				nearestPointToRay(begin1, new_begin, dir2, NULL, NULL, &result);
				return result;
			} else {
				return distanceBetweenLines(begin1, dir1, begin2, dir2);
			}
		} else {
			Real move_amount = (dotProduct(dir2, begin2) - dotProduct(dir2, begin1)) / dp_d1_d2;
			Vector3 new_begin = begin1 + dir1 * move_amount;
			// If ray #2 is at backside of new_begin,
			// then rays cannot get any closer.
			if (dotProduct(begin2 - new_begin, dir1) <= 0) {
				Real result;
				nearestPointToRay(begin2, new_begin, dir1, NULL, NULL, &result);
				return result;
			} else {
				return distanceBetweenLines(begin1, dir1, begin2, dir2);
			}
		}

	// Angle between rays is more than 90 °
	} else if (dp_d1_d2 < 0) {

		// If both of rays begin from the backside of eachothers,
		// then it means that rays can never get any closer
		if (!ray1_begins_front && !ray2_begins_front) {
			return (begin1 - begin2).length();
		}

		// If only one ray begins from the backside of the
		// another, then calculate distance from point to ray.
		if (ray1_begins_front && !ray2_begins_front) {
			Real result;
			nearestPointToRay(begin1, begin2, dir2, NULL, NULL, &result);
			return result;
		}
		if (!ray1_begins_front && ray2_begins_front) {
			Real result;
			nearestPointToRay(begin2, begin1, dir1, NULL, NULL, &result);
			return result;
		}

// TODO: Code this!
std::cerr << __FILE__ << ":" << __LINE__ << " WARNING: Not implemented yet, so giving rough estimation!" << std::endl;
return distanceBetweenLines(begin1, dir1, begin2, dir2);

	}
	// Angle between rays is exactly 90 °
	else {
// TODO: Code this!
std::cerr << __FILE__ << ":" << __LINE__ << " WARNING: Not implemented yet, so giving rough estimation!" << std::endl;
return distanceBetweenLines(begin1, dir1, begin2, dir2);
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
	HppAssert(ray_dir.lengthTo2() > 0.0, "Segment cannot be zero sized!");
	HppAssert(dotProduct(ray_dir, plane_nrm) != 0.0, "This ray will never hit plane!");
	HppAssert(plane_nrm.lengthTo2() > 0.0, "Plane normal cannot have zero length!");
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
	HppAssert(dir.lengthTo2() > 0.0, "Dir can not be zero!");

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

inline bool triangleHitsSphere(Vector3 const& pos, Real radius,
                               Vector3 const& tp0, Vector3 const& tp1, Vector3 const& tp2,
                               Vector3& coll_pos, Vector3& coll_nrm, Real& coll_depth,
                               Vector3 const& tri_bs_pos, Real tri_bs_r)
{
	Vector3 edge0(tp1 - tp0);
	Vector3 edge1(tp2 - tp1);
	// Before collision check, do bounding sphere check.
	if (tri_bs_r >= 0.0) {
		if ((pos - tri_bs_pos).length() > radius + tri_bs_r) {
			return false;
		}
	} else {
		Vector3 const& tri_bs_pos2 = tp1;
		Real dst0 = edge0.length();
		Real dst1 = edge1.length();
		Real tri_bs_r2 = (dst0 > dst1) ? dst0 : dst1;
		if ((pos - tri_bs_pos2).length() > radius + tri_bs_r2) {
			return false;
		}
	}
	// Do the collision check.
	Vector3 edge2(tp0 - tp2);
	// Form normal of plane. If result is zero, then do not test against
	// plane. Only do edge and corner tests then.
	Vector3 plane_nrm = crossProduct(tp1 - tp0, tp2 - tp0);
	Real plane_nrm_length = plane_nrm.length();
	if (plane_nrm_length > 0.0) {
		// Check if center of sphere is above/below triangle
		HppAssert(plane_nrm.lengthTo2() > 0.0, "Plane normal must not be zero!");
		Vector3 pos_at_plane = posToPlane(pos, tp0, plane_nrm);

		// We can calculate collision normal and depth here.
		coll_nrm = pos_at_plane - pos;
		coll_depth = radius - coll_nrm.length();

		// Do not check any other stuff if sphere is too far away from
		// triangle plane.
		if (coll_depth > 0) {

			// Check if the position is inside the area of triangle
			Vector2 pos_at_tri = transformPointToTrianglespace(pos_at_plane - tp0, edge0, -edge2);
			if (pos_at_tri.x >= 0.0 && pos_at_tri.y >= 0.0 && pos_at_tri.x + pos_at_tri.y <= 1.0) {
				coll_pos = pos_at_plane;
				Real coll_nrm_length = coll_nrm.length();
				if (coll_nrm_length != 0) {
					coll_nrm = -coll_nrm / coll_nrm_length;
				} else {
					coll_nrm = plane_nrm / plane_nrm_length;
				}
				HppAssert(coll_nrm.lengthTo2() != 0.0, "Fail!");
				return true;
			}
		} else {
			return false;
		}
	}

	Real deepest_neg_depth;
	bool coll_found;
	#ifndef NDEBUG
	deepest_neg_depth = 0;
	#endif

	// Check if edges collide
	Real dp0 = dotProduct(tp0, tp0);
	Real dp1 = dotProduct(tp1, tp1);
	Real dp2 = dotProduct(tp2, tp2);
	Real dp01 = dotProduct(tp0, tp1);
	Real dp12 = dotProduct(tp1, tp2);
	Real dp20 = dotProduct(tp2, tp0);
	Real val0 = dotProduct(tp0, pos);
	Real val1 = dotProduct(tp1, pos);
	Real val2 = dotProduct(tp2, pos);
	Real divider = 2*dp01-dp0-dp1;
	if (divider != 0) {
		Vector3 edge0_np = tp0 + edge0 * ((dp01-val1+val0-dp0) / divider);
		Real edge0_len = edge0.length();
		Real edge0_dst = (edge0_np - pos).length();
		if (edge0_dst <= radius &&
		    (tp0 - edge0_np).length() <= edge0_len &&
		    (tp1 - edge0_np).length() <= edge0_len) {
			coll_pos = edge0_np;
			coll_nrm = edge0_np - pos;
			deepest_neg_depth = coll_nrm.length();
			coll_found = true;
		} else coll_found = false;
	} else coll_found = false;
	divider = 2*dp12-dp1-dp2;
	if (divider != 0) {
		Vector3 edge1_np = tp1 + edge1 * ((dp12-val2+val1-dp1) / divider);
		Real edge1_len = edge1.length();
		Real edge1_dst = (edge1_np - pos).length();
		if (edge1_dst <= radius &&
		    (tp1 - edge1_np).length() <= edge1_len &&
		    (tp2 - edge1_np).length() <= edge1_len) {
			if (!coll_found) {
				coll_pos = edge1_np;
				coll_nrm = edge1_np - pos;
				deepest_neg_depth = coll_nrm.length();
				coll_found = true;
			} else {
				Vector3 test_nrm = edge1_np - pos;
				Real test_neg_depth = test_nrm.length();
				if (test_neg_depth < deepest_neg_depth) {
					coll_pos = edge1_np;
					coll_nrm = test_nrm;
					deepest_neg_depth = test_neg_depth;
				}
			}
		}
	}
	divider = 2*dp20-dp2-dp0;
	if (divider != 0) {
		Vector3 edge2_np = tp2 + edge2 * ((dp20-val0+val2-dp2) / divider);
		Real edge2_len = edge2.length();
		Real edge2_dst = (edge2_np - pos).length();
		if (edge2_dst <= radius &&
		    (tp2 - edge2_np).length() <= edge2_len &&
		    (tp0 - edge2_np).length() <= edge2_len) {
			if (!coll_found) {
				coll_pos = edge2_np;
				coll_nrm = edge2_np - pos;
				deepest_neg_depth = coll_nrm.length();
				coll_found = true;
			} else {
				Vector3 test_nrm = edge2_np - pos;
				Real test_neg_depth = test_nrm.length();
				if (test_neg_depth < deepest_neg_depth) {
					coll_pos = edge2_np;
					coll_nrm = test_nrm;
					deepest_neg_depth = test_neg_depth;
				}
			}
		}
	}

	// Check if corners hit sphere
	if ((tp0 - pos).length() <= radius) {
		if (!coll_found) {
			coll_pos = tp0;
			coll_nrm = tp0 - pos;
			deepest_neg_depth = coll_nrm.length();
			coll_found = true;
		} else {
			Vector3 test_nrm = tp0 - pos;
			Real test_neg_depth = test_nrm.length();
			if (test_neg_depth < deepest_neg_depth) {
				coll_pos = tp0;
				coll_nrm = test_nrm;
				deepest_neg_depth = test_neg_depth;
			}
		}
	}
	if ((tp1 - pos).length() <= radius) {
		if (!coll_found) {
			coll_pos = tp1;
			coll_nrm = tp1 - pos;
			deepest_neg_depth = coll_nrm.length();
			coll_found = true;
		} else {
			Vector3 test_nrm = tp1 - pos;
			Real test_neg_depth = test_nrm.length();
			if (test_neg_depth < deepest_neg_depth) {
				coll_pos = tp1;
				coll_nrm = test_nrm;
				deepest_neg_depth = test_neg_depth;
			}
		}
	}
	if ((tp2 - pos).length() <= radius) {
		if (!coll_found) {
			coll_pos = tp2;
			coll_nrm = tp2 - pos;
			deepest_neg_depth = coll_nrm.length();
			coll_found = true;
		} else {
			Vector3 test_nrm = tp2 - pos;
			Real test_neg_depth = test_nrm.length();
			if (test_neg_depth < deepest_neg_depth) {
				coll_pos = tp2;
				coll_nrm = test_nrm;
				deepest_neg_depth = test_neg_depth;
			}
		}
	}
	if (coll_found) {
		coll_depth = radius - deepest_neg_depth;
		HppAssert(deepest_neg_depth != 0.0, "Division by zero!");
		coll_nrm /= -deepest_neg_depth;
		return true;
	}
	return false;
}

}

#endif
