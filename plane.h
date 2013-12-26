#ifndef HPP_PLANE_H
#define HPP_PLANE_H

#include "vector3.h"
#include "ray.h"

#include <vector>
#include <string>

namespace Hpp
{

class Plane
{

public:

	typedef std::vector< Plane > Vec;

	inline Plane(void);
	inline Plane(Vector3 const& normal, Real distance_from_origin);
	inline Plane(Vector3 const& normal, Vector3 const& pos_at_plane);
	inline Plane(Vector3 const& v0, Vector3 const& v1, Vector3 const& v2);

	inline Vector3 getNormal(void) const { return normal; }
	inline Vector3 getPosition(void) const { return normal * distance_from_origin; }

	inline void swapNormal(void);

	// "result_dir_mult" tells how many times dir
	// must be added to begin so hitpos is reached.
	inline bool hitsLine(Vector3 const& begin, Vector3 const& dir, Vector3* result_hitpos = NULL, Real* result_dir_mult = NULL) const;

	// "result_dir_mult" tells how many times dir
	// must be added to begin so hitpos is reached.
	inline bool hitsRay(Ray const& ray, Vector3* result_hitpos = NULL, Real* result_dir_mult = NULL) const;

	// In case of hit, result_hitline_dir will be normalized
	inline bool hitsPlane(Plane const& plane2, Vector3* result_hitline_pos = NULL, Vector3* result_hitline_dir = NULL) const;

	inline bool isPositionAtFront(Vector3 const& pos) const;

	inline Vector3 posToPlane(Vector3 const& pos) const;
	inline Vector3 posToPlane(Vector3 const& pos, Vector3 const& moving_dir) const;

	// Gives negative result if position is at back of the Plane
	inline Real distanceToPosition(Vector3 const& pos) const;

	inline std::string toString(void) const;

private:

	Vector3 normal; // Normalized always, except when not initialized
	Real distance_from_origin;

	// Normal must be normalized when this is called
	inline void calculateDistanceToOrigin(Vector3 const& pos_at_plane);

};

inline Plane::Plane(void)
{
}

inline Plane::Plane(Vector3 const& normal, Real distance_from_origin) :
normal(normal),
distance_from_origin(distance_from_origin)
{
	HppAssert(normal.length() > 0, "Unable to form Plane from given normal!");
	this->normal.normalize();
}

inline Plane::Plane(Vector3 const& v0, Vector3 const& v1, Vector3 const& v2)
{
	// Calculate normal
	Vector3 edge0 = v1 - v0;
	Vector3 edge1 = v2 - v0;
	normal = crossProduct(edge0, edge1);
	HppAssert(normal.length() > 0, "Unable to form Plane from given triangle!");
	normal.normalize();

	calculateDistanceToOrigin(v0);
}

inline Plane::Plane(Vector3 const& normal, Vector3 const& pos_at_plane) :
normal(normal)
{
	HppAssert(normal.length() > 0, "Unable to form Plane from given normal!");
	this->normal.normalize();
	calculateDistanceToOrigin(pos_at_plane);
}

inline void Plane::swapNormal(void)
{
	normal = -normal;
	distance_from_origin = -distance_from_origin;
}

inline bool Plane::hitsLine(Vector3 const& begin, Vector3 const& dir, Vector3* result_hitpos, Real* result_dir_mult) const
{
	// If direction and plane normal are perpendicular
	// to each others, then hit cannot occure.
	Real dir_len = dir.length();
	Real dp_n_d = dotProduct(normal, dir);
	if (dir_len < 0.00001) {
		return false;
	}

	Real radians = static_cast< Real >(::asin(dp_n_d / dir_len));
	if (::finite(radians) && fabs(radians) < 0.0002) {
		return false;
	}

	Vector3 pos_at_plane = getPosition();

	Real dir_m = (dotProduct(normal, pos_at_plane) - dotProduct(normal, begin)) / dp_n_d;

	if (result_hitpos) {
		*result_hitpos = begin + dir * dir_m;
	}
	if (result_dir_mult) {
		*result_dir_mult = dir_m;
	}

	return true;
}

bool Plane::hitsRay(Ray const& ray, Vector3* result_hitpos, Real* result_dir_mult) const
{
	Hpp::Real result_dir_mult2;
	bool hits_line = hitsLine(ray.begin, ray.dir, result_hitpos, &result_dir_mult2);
	// If it does not hit plane
	if (!hits_line) return false;
	if (result_dir_mult2 < 0) return false;
	// If it hits
	if (result_dir_mult) *result_dir_mult = result_dir_mult2;
	return true;
}

inline bool Plane::hitsPlane(Plane const& plane2, Vector3* result_hitline_pos, Vector3* result_hitline_dir) const
{
	Vector3 hitline_dir = crossProduct(normal, plane2.normal);
	Real hitline_dir_len = hitline_dir.length();
	// If cutplanes would hit only at far
	// away, then do not consider as hit.
	if (hitline_dir_len < 0.00001) {
		return false;
	}

	hitline_dir /= hitline_dir_len;

	if (result_hitline_pos) {
		Vector3 finder = crossProduct(plane2.normal, hitline_dir);
		*result_hitline_pos = posToPlane(plane2.getPosition(), finder);
	}

	if (result_hitline_dir) {
		*result_hitline_dir = hitline_dir;
	}

	return true;
}

inline bool Plane::isPositionAtFront(Vector3 const& pos) const
{
	Vector3 pos_relative_to_plane = pos - getPosition();
	return dotProduct(pos_relative_to_plane, normal) > 0;
}

inline Vector3 Plane::posToPlane(Vector3 const& pos) const
{
	return posToPlane(pos, normal);
}

inline Vector3 Plane::posToPlane(Vector3 const& pos, Vector3 const& moving_dir) const
{
	Vector3 result;
	if (!hitsLine(pos, moving_dir, &result)) {
		throw Exception("Unable to project position to plane!");
	}
	return result;
}

inline Real Plane::distanceToPosition(Vector3 const& pos) const
{
	Real dp_n_n = dotProduct(normal, normal);
	return dotProduct(normal, pos) / dp_n_n - distance_from_origin;
}

inline std::string Plane::toString(void) const
{
	return normal.toString() + ":" + floatToStr(distance_from_origin);
}

inline void Plane::calculateDistanceToOrigin(Vector3 const& pos_at_plane)
{
	Real dp_nn = dotProduct(normal, normal);
	HppAssert(dp_nn != 0, "Division by zero! Normal is not normalized!");
	distance_from_origin = dotProduct(normal, pos_at_plane) / dp_nn;
}

}

#endif
