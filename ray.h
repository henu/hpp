#ifndef HPP_RAY_H
#define HPP_RAY_H

#include "vector3.h"

namespace Hpp
{

class Ray
{

public:

	inline Ray(void);
	inline Ray(Hpp::Vector3 const& begin, Hpp::Vector3 const& dir);

	Hpp::Vector3 begin;
	Hpp::Vector3 dir;

};

inline Ray::Ray(void)
{
}

inline Ray::Ray(Hpp::Vector3 const& begin, Hpp::Vector3 const& dir) :
begin(begin),
dir(dir)
{
}

}

#endif
