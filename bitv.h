#ifndef HPP_BITV_H
#define HPP_BITV_H

#include <vector>
#include <stdint.h>

namespace Hpp
{

// Bit vector
typedef std::vector< bool > BitV;

}

// Add operators
inline Hpp::BitV operator+(Hpp::BitV const& v0, Hpp::BitV const& v1);
inline Hpp::BitV operator+=(Hpp::BitV& v0, Hpp::BitV const& v1);

inline Hpp::BitV operator+(Hpp::BitV const& v0, Hpp::BitV const& v1)
{
	Hpp::BitV new_v(v0.begin(), v0.end());
	new_v.insert(new_v.end(), v1.begin(), v1.end());
	return new_v;
}

inline Hpp::BitV operator+=(Hpp::BitV& v0, Hpp::BitV const& v1)
{
	v0.insert(v0.end(), v1.begin(), v1.end());
	return v0;
}

#endif
