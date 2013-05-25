#ifndef HPP_BOUNDINGVOLUME_H
#define HPP_BOUNDINGVOLUME_H

#include "vector3.h"

namespace Hpp
{

class Boundingvolume
{

public:

	enum Testresult {
		OUTSIDE = 0,
		PARTIALLY_INSIDE = 1,
		INSIDE = 2
	};

	// Tests how another Boundingvolume is compared to this one. Note,
	// that these maye give false positives, i.e. say that another
	// volume is completely or partially inside this one, even when
	// its not. However false negatives are never given.
	virtual Testresult testAnotherBoundingvolume(Boundingvolume const* bv) const = 0;

	virtual bool isPositionInside(Vector3 const& pos) const = 0;

private:

};

}

#endif
