#ifndef HPP_VIEWFRUSTUM_H
#define HPP_VIEWFRUSTUM_H

#include "boundingvolume.h"
#include "boundingconvex.h"
#include "3dutils.h"
#include "angle.h"
#include "vector2.h"
#include "vector3.h"

#include <vector>
#include <string>

namespace Hpp
{

class Viewfrustum
{

public:

	typedef std::vector< Viewfrustum > Vec;

	// How boundinvolume is related to viewfrustum
	enum VFResult { OUTSIDE, PARTIALLY_INSIDE, INSIDE };

	// Default constructor makes Viewfrustum that contains nothing
	inline Viewfrustum(void);

	// Constructor. Vrts means vertices of frustrum polygon that are on a
	// plane that is one unit from the position of viewfrustrum.
	inline Viewfrustum(Vector3 const& pos,
	                   Vector3 const& dir, Vector3 const& up, Vector3 const& right,
	                   Vector2::Vec const& vrts,
	                   Real nearplane = -1,
	                   Real farplane = -1);
	inline Viewfrustum(Vector3 const& pos,
	                   Boundingconvex const& bconvex);

	inline Vector3 getPosition(void) const { return pos; }

	inline bool isEmpty(void) const { return type == EMPTY; }

	inline static Viewfrustum fromCamera(Vector3 const& pos,
	                                     Angle const& yaw,
	                                     Angle const& pitch,
	                                     Angle const& head,
	                                     Angle const& fov_y,
	                                     Angle const& fov_x,
	                                     Real nearplane = -1,
	                                     Real farplane = -1);
	inline static Viewfrustum fromCamera(Vector3 const& pos,
	                                     Vector3 const& dir, Vector3 const& up, Vector3 const& right,
	                                     Angle const& fov_y,
	                                     Angle const& fov_x,
	                                     Real nearplane = -1,
	                                     Real farplane = -1);

	inline static Viewfrustum all(Vector3 const& pos) { return Viewfrustum(ALL, pos); }
	inline static Viewfrustum empty(Vector3 const& pos) { return Viewfrustum(EMPTY, pos); }

	inline VFResult testBoundingvolume(Boundingvolume const* bv) const;

	inline bool testPoint(Vector3 const& point) const;

	inline Viewfrustum doIntersection(Viewfrustum const& vfrust) const;

	inline std::string toString(void) const;

	// May return NULL if there is no faces to render.
	// Also infinite faces will be constructed poorly.
	inline Mesh* createMesh(void) const;

private:

	enum Type { NORMAL, ALL, EMPTY };

	Type type;
	Vector3 pos;

	Boundingconvex bconvex;

	inline Viewfrustum(Type const& type, Vector3 const& pos) : type(type), pos(pos) { }

};

inline Viewfrustum::Viewfrustum(void) :
type(EMPTY)
{
}

inline Viewfrustum::Viewfrustum(Vector3 const& pos,
                                Vector3 const& dir, Vector3 const& up, Vector3 const& right,
                                Vector2::Vec const& vrts,
                                Real nearplane,
                                Real farplane) :
type(NORMAL),
pos(pos)
{
	HppAssert(fabs(dotProduct(dir, up)) < 0.001, "Dir and up must be perpendicular to each others!");
	HppAssert(fabs(dotProduct(up, right)) < 0.001, "Up and right must be perpendicular to each others!");
	HppAssert(fabs(dotProduct(right, dir)) < 0.001, "Right and dir must be perpendicular to each others!");
	HppAssert(fabs(dir.length() - 1) < 0.001, "Dir must be normalized!");

	// Form Boundingconvex Planes from vertices
	Plane::Vec planes;
	Vector3 last_vrt_3d = pos + dir + right * vrts.back().x + up * vrts.back().y;
	for (Vector2::Vec::const_iterator vrts_it = vrts.begin();
	     vrts_it != vrts.end();
	     ++ vrts_it) {
		Vector2 const& vrt = *vrts_it;
		Vector3 vrt_3d = pos + dir + right * vrt.x + up * vrt.y;
		Vector3 plane_edge0 = vrt_3d - pos;
		Vector3 plane_edge1 = last_vrt_3d - pos;
		Vector3 plane_normal = crossProduct(plane_edge0, plane_edge1);
		// Swap normal if it does not point towards inside of Viewfrustum
		if (dotProduct(plane_normal, dir) < 0) {
			plane_normal = -plane_normal;
		}
		planes.push_back(Plane(plane_normal, pos));

		last_vrt_3d = vrt_3d;
	}

	// Form Boundingconvex Plane from possible nearplane and/or farplane
	if (nearplane > 0) {
		if (farplane > 0) {
			if (nearplane >= farplane) {
				throw Exception("Nearplane must be neared than farplane!");
			}
		}
		planes.push_back(Plane(dir, pos + dir * nearplane));
	}
	if (farplane > 0) {
		planes.push_back(Plane(-dir, pos + dir * farplane));
	}

	bconvex.setCutplanes(planes);
}

inline Viewfrustum::Viewfrustum(Vector3 const& pos,
                                Boundingconvex const& bconvex) :
pos(pos)
{
	if (bconvex.isNothing()) {
		type = EMPTY;
	}
	type = NORMAL;
	this->bconvex = bconvex;
}

inline Viewfrustum Viewfrustum::fromCamera(Vector3 const& pos,
                                           Angle const& yaw,
                                           Angle const& pitch,
                                           Angle const& roll,
                                           Angle const& fov_y,
                                           Angle const& fov_x,
                                           Real nearplane,
                                           Real farplane)
{
	// Get vectors
	Vector3 dir, up, right;
	getCameraDirections(yaw, pitch, roll, &dir, &up, &right);

	return fromCamera(pos, dir, up, right, fov_y, fov_x, nearplane, farplane);
}

inline Viewfrustum Viewfrustum::fromCamera(Vector3 const& pos,
                                           Vector3 const& dir, Vector3 const& up, Vector3 const& right,
                                           Angle const& fov_y,
                                           Angle const& fov_x,
                                           Real nearplane,
                                           Real farplane)
{
	// Calculate vertices
	Real fov_y_tan = (fov_y / 2).tan();
	Real fov_x_tan = (fov_x / 2).tan();
	Vector2::Vec vrts;
	vrts.push_back(Vector2(fov_x_tan, fov_y_tan));
	vrts.push_back(Vector2(-fov_x_tan, fov_y_tan));
	vrts.push_back(Vector2(-fov_x_tan, -fov_y_tan));
	vrts.push_back(Vector2(fov_x_tan, -fov_y_tan));

	return Viewfrustum(pos, dir, up, right, vrts, nearplane, farplane);
}

inline Viewfrustum::VFResult Viewfrustum::testBoundingvolume(Boundingvolume const* bv) const
{
	if (type == EMPTY) {
		return OUTSIDE;
	}
	if (type == ALL) {
		return INSIDE;
	}

	Boundingvolume::Testresult result = bconvex.testAnotherBoundingvolume(bv);

	if (result == Boundingvolume::OUTSIDE) return OUTSIDE;
	if (result == Boundingvolume::INSIDE) return INSIDE;
	return PARTIALLY_INSIDE;
}

inline bool Viewfrustum::testPoint(Vector3 const& point) const
{
	if (type == EMPTY) {
		return OUTSIDE;
	}
	if (type == ALL) {
		return INSIDE;
	}

	return bconvex.isPositionInside(point);
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

	Viewfrustum result(NORMAL, pos);
	result.bconvex = bconvex.intersection(vfrust.bconvex);

	if (result.bconvex.isNothing()) {
		result.type = EMPTY;
	}

	return result;
}

inline std::string Viewfrustum::toString(void) const
{
	std::string result = pos.toString();

	result += " ";

	if (type == ALL) {
		result += "ALL";
	} else if (type == EMPTY) {
		result += "EMPTY";
	} else {
		result += bconvex.toString();
	}

	return result;
}

inline Mesh* Viewfrustum::createMesh(void) const
{
	if (type == ALL || type == EMPTY) {
		return NULL;
	}
	return bconvex.createMesh();
}

}

#endif
