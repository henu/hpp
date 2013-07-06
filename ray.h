#ifndef HPP_RAY_H
#define HPP_RAY_H

#include "vector3.h"

namespace Hpp
{

class Ray
{

public:

	inline Ray(void);
	inline Ray(Vector3 const& begin, Vector3 const& dir);

	Vector3 begin;
	Vector3 dir;

};

inline Ray::Ray(void)
{
}

inline Ray::Ray(Vector3 const& begin, Vector3 const& dir) :
begin(begin),
dir(dir)
{
}

}

#endif
