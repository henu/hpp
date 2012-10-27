#ifndef HPP_BOUNDINGSPHERE_H
#define HPP_BOUNDINGSPHERE_H

#include "transform.h"
#include "vector3.h"
#include "real.h"

#include <vector>
#include <ostream>

namespace Hpp
{

class Boundingsphere
{

public:

	inline Boundingsphere(void) {}
	inline Boundingsphere(Vector3 const& pos, Real radius) : pos(pos), radius(radius) {}
	inline static Boundingsphere infinite(void) { Boundingsphere bs; bs.radius = -1; return bs; }
	inline static Boundingsphere nothing(void) { Boundingsphere bs; bs.radius = 0.0; bs.pos = Vector3::ZERO; return bs; }

	inline void set(Vector3 const& pos, Real radius) { this->pos = pos; this->radius = radius; }
	inline void setPosition(Vector3 const& pos) { this->pos = pos; }
	inline void setRadius(Real radius) { this->radius = radius; }
	inline void setInfinite(void) { this->radius = -1; }
	inline void setNothing(void) { this->radius = 0.0; }

	inline Vector3 getPosition(void) const { return pos; }
	inline Real getRadius(void) const { return radius; }

	inline bool isInfinite(void) const { return radius < 0; }
	inline bool isNothing(void) const { return radius == 0.0; }

	// Applies transform to this boundingsphere
	inline void applyTransform(Transform const& transf);

	// Hit tests
	inline bool rayHits(Vector3 const& begin, Vector3 const& dir) const;

private:

	Vector3 pos;
	Real radius;

};
typedef std::vector< Boundingsphere > Boundingspheres;

inline std::ostream& operator<<(std::ostream& strm, Boundingsphere const& bs);

inline void Boundingsphere::applyTransform(Transform const& transf)
{
	pos = transf.applyToPosition(pos);
	radius = transf.getMaximumScaling() * radius;
}

inline bool Boundingsphere::rayHits(Vector3 const& begin, Vector3 const& dir) const
{
	// Ray begins from front of sphere center
	if (distanceToPlane(pos, dir, begin) > 0.0) {
		return (pos - begin).length() < radius;
	}
	// Ray begins from back of sphere center
	else {
		Real ray_dst_to_pos;
		nearestPointToRay(pos, begin, dir, NULL, NULL, &ray_dst_to_pos);
		return ray_dst_to_pos < radius;
	}
}

inline std::ostream& operator<<(std::ostream& strm, Boundingsphere const& bs)
{
	if (bs.isInfinite()) {
		strm << "INF";
	} else {
		strm << bs.getPosition() << " / " << bs.getRadius();
	}
	return strm;
}

}

#endif
