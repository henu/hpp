#ifndef HPP_BYTEV_H
#define HPP_BYTEV_H

#include <vector>
#include <string>
#include <stdint.h>

namespace Hpp
{

// Byte vector
typedef std::vector< uint8_t > ByteV;

}

// Add operators
inline Hpp::ByteV operator+(Hpp::ByteV const& v0, Hpp::ByteV const& v1);
inline Hpp::ByteV operator+(Hpp::ByteV const& v, std::string const& s);
inline Hpp::ByteV operator+(std::string const& s, Hpp::ByteV const& v);
inline Hpp::ByteV operator+=(Hpp::ByteV& v0, Hpp::ByteV const& v1);
inline Hpp::ByteV operator+=(Hpp::ByteV& v, std::string const& s);

inline Hpp::ByteV operator+(Hpp::ByteV const& v0, Hpp::ByteV const& v1)
{
	Hpp::ByteV new_v;
	new_v.reserve(v0.size() + v1.size());
	new_v.insert(new_v.end(), v0.begin(), v0.end());
	new_v.insert(new_v.end(), v1.begin(), v1.end());
	return new_v;
}

inline Hpp::ByteV operator+(Hpp::ByteV const& v, std::string const& s)
{
	Hpp::ByteV new_v;
	new_v.reserve(v.size() + s.size());
	new_v.insert(new_v.end(), v.begin(), v.end());
	new_v.insert(new_v.end(), s.begin(), s.end());
	return new_v;
}

inline Hpp::ByteV operator+(std::string const& s, Hpp::ByteV const& v)
{
	Hpp::ByteV new_v;
	new_v.reserve(s.size() + v.size());
	new_v.insert(new_v.end(), s.begin(), s.end());
	new_v.insert(new_v.end(), v.begin(), v.end());
	return new_v;
}

inline Hpp::ByteV operator+=(Hpp::ByteV& v0, Hpp::ByteV const& v1)
{
	v0.insert(v0.end(), v1.begin(), v1.end());
	return v0;
}

inline Hpp::ByteV operator+=(Hpp::ByteV& v, std::string const& s)
{
	v.insert(v.end(), s.begin(), s.end());
	return v;
}

#endif
