#ifndef HPP_BOUNDINGBOX_H
#define HPP_BOUNDINGBOX_H

#include "boundingvolume.h"
#include "boundingsphere.h"
#include "vector3.h"
#include "assert.h"

#include <algorithm>

namespace Hpp
{

class Boundingbox : public Boundingvolume
{

public:

	// Constructors and creators
	inline Boundingbox(void);
	inline Boundingbox(Vector3 const& cmin, Vector3 const& cmax);
	inline static Boundingbox fromTriangle(Vector3 const& v0, Vector3 const& v1, Vector3 const& v2);
	inline static Boundingbox fromCapsule(Vector3 const& pos0, Vector3 const& pos1, Real radius);

	inline void setSize(Vector3 const& cmin, Vector3 const& cmax);

	inline Real getMinX(void) const { return cmin.x; }
	inline Real getMinY(void) const { return cmin.y; }
	inline Real getMinZ(void) const { return cmin.z; }
	inline Real getMaxX(void) const { return cmax.x; }
	inline Real getMaxY(void) const { return cmax.y; }
	inline Real getMaxZ(void) const { return cmax.z; }

	inline Vector3 getCenter(void) const;

	inline virtual Testresult testAnotherBoundingvolume(Boundingvolume const* bv) const;
	inline virtual bool isPositionInside(Vector3 const& pos) const;

private:

	Vector3 cmin, cmax;

};

inline Boundingbox::Boundingbox(void) :
cmin(Vector3::ZERO),
cmax(Vector3::ZERO)
{
}

inline Boundingbox::Boundingbox(Vector3 const& cmin, Vector3 const& cmax) :
cmin(cmin),
cmax(cmax)
{
	HppAssert(cmin.x <= cmax.x, "Minimum X component is bigger than maximum!");
	HppAssert(cmin.y <= cmax.y, "Minimum Y component is bigger than maximum!");
	HppAssert(cmin.z <= cmax.z, "Minimum Z component is bigger than maximum!");
}

inline Boundingbox Boundingbox::fromTriangle(Vector3 const& v0, Vector3 const& v1, Vector3 const& v2)
{
	Vector3 cmin, cmax;
	cmin.x = std::min(std::min(v0.x, v1.x), v2.x);
	cmin.y = std::min(std::min(v0.y, v1.y), v2.y);
	cmin.z = std::min(std::min(v0.z, v1.z), v2.z);
	cmax.x = std::max(std::max(v0.x, v1.x), v2.x);
	cmax.y = std::max(std::max(v0.y, v1.y), v2.y);
	cmax.z = std::max(std::max(v0.z, v1.z), v2.z);
	return Boundingbox(cmin, cmax);
}

inline Boundingbox Boundingbox::fromCapsule(Vector3 const& pos0, Vector3 const& pos1, Real radius)
{
	Vector3 cmin, cmax;
	cmin.x = std::min(pos0.x, pos1.x);
	cmin.y = std::min(pos0.y, pos1.y);
	cmin.z = std::min(pos0.z, pos1.z);
	cmax.x = std::max(pos0.x, pos1.x);
	cmax.y = std::max(pos0.y, pos1.y);
	cmax.z = std::max(pos0.z, pos1.z);
	cmin -= Vector3::ONE * radius;
	cmax += Vector3::ONE * radius;
	return Boundingbox(cmin, cmax);
}

inline void Boundingbox::setSize(Vector3 const& cmin, Vector3 const& cmax)
{
	this->cmin = cmin;
	this->cmax = cmax;
	HppAssert(cmin.x <= cmax.x, "Minimum X component is bigger than maximum!");
	HppAssert(cmin.y <= cmax.y, "Minimum Y component is bigger than maximum!");
	HppAssert(cmin.z <= cmax.z, "Minimum Z component is bigger than maximum!");
}

inline Vector3 Boundingbox::getCenter(void) const
{
	return (cmin + cmax) / 2;
}

inline Boundingvolume::Testresult Boundingbox::testAnotherBoundingvolume(Boundingvolume const* bv) const
{
	// If another Boundingvolume is Boundingsphere
	Boundingsphere const* bs = dynamic_cast< Boundingsphere const* >(bv);
	if (bs) {
		if (bs->isInfinite()) {
			return PARTIALLY_INSIDE;
		}
		bool fully_inside = true;

		// +X side
		Real distance_to_side = cmax.x - bs->getPosition().x;
		if (distance_to_side < -bs->getRadius()) return OUTSIDE;
		if (distance_to_side < bs->getRadius()) fully_inside = false;

		// -X side
		distance_to_side = bs->getPosition().x - cmin.x;
		if (distance_to_side < -bs->getRadius()) return OUTSIDE;
		if (distance_to_side < bs->getRadius()) fully_inside = false;

		// +Y side
		distance_to_side = cmax.y - bs->getPosition().y;
		if (distance_to_side < -bs->getRadius()) return OUTSIDE;
		if (distance_to_side < bs->getRadius()) fully_inside = false;

		// -Y side
		distance_to_side = bs->getPosition().y - cmin.y;
		if (distance_to_side < -bs->getRadius()) return OUTSIDE;
		if (distance_to_side < bs->getRadius()) fully_inside = false;

		// +Z side
		distance_to_side = cmax.z - bs->getPosition().z;
		if (distance_to_side < -bs->getRadius()) return OUTSIDE;
		if (distance_to_side < bs->getRadius()) fully_inside = false;

		// -Z side
		distance_to_side = bs->getPosition().z - cmin.z;
		if (distance_to_side < -bs->getRadius()) return OUTSIDE;
		if (distance_to_side < bs->getRadius()) fully_inside = false;

		if (fully_inside) return INSIDE;
		return PARTIALLY_INSIDE;
	}

	// If another Boundingvolume is Boundingbox
	Boundingbox const* bb = dynamic_cast< Boundingbox const* >(bv);
	if (bb) {

		if (cmax.x > bb->cmax.x &&
		    cmax.y > bb->cmax.y &&
		    cmax.z > bb->cmax.z &&
		    cmin.x < bb->cmin.x &&
		    cmin.y < bb->cmin.y &&
		    cmin.z < bb->cmin.z) {
			return INSIDE;
		}
		if (cmax.x < bb->cmin.x || cmin.x > bb->cmax.x) return OUTSIDE;
		if (cmax.y < bb->cmin.y || cmin.y > bb->cmax.y) return OUTSIDE;
		if (cmax.z < bb->cmin.z || cmin.z > bb->cmax.z) return OUTSIDE;
		return PARTIALLY_INSIDE;
	}

	// This Boundingvolume cannot be handled, so try the other way
	return bv->testAnotherBoundingvolume(this);
}

inline bool Boundingbox::isPositionInside(Vector3 const& pos) const
{
	if (pos.x < cmin.x) return false;
	if (pos.y < cmin.y) return false;
	if (pos.z < cmin.z) return false;
	if (pos.x > cmax.x) return false;
	if (pos.y > cmax.y) return false;
	if (pos.z > cmax.z) return false;
	return true;
}

}

#endif
