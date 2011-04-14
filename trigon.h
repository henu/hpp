#ifndef HPP_TRIGON_H
#define HPP_TRIGON_H

#include "angle.h"

#include <cmath>

namespace Hpp
{

inline double tan(Angle const& a);
inline float tanf(Angle const& a);
inline double sin(Angle const& a);
inline float sinf(Angle const& a);
inline double cos(Angle const& a);
inline float cosf(Angle const& a);

inline Angle atan(float f);
inline Angle atan(double d);
inline Angle asin(float f);
inline Angle asin(double d);
inline Angle acos(float f);
inline Angle acos(double d);


// ----------------------------------------
// Implementations of inline functions
// ----------------------------------------

inline double tan(Angle const& a) { return ::tan(static_cast< double >(a.getRadians())); }
inline float tanf(Angle const& a) { return ::tanf(a.getRadians()); }
inline double sin(Angle const& a) { return ::sin(static_cast< double >(a.getRadians())); }
inline float sinf(Angle const& a) { return ::sinf(a.getRadians()); }
inline double cos(Angle const& a) { return ::cos(static_cast< double >(a.getRadians())); }
inline float cosf(Angle const& a) { return ::cosf(a.getRadians()); }

inline Angle atan(float f) { return Angle::fromRadians(::atanf(f)); }
inline Angle atan(double d) { return Angle::fromRadians(::atan(d)); }
inline Angle asin(float f) { return Angle::fromRadians(::asinf(f)); }
inline Angle asin(double d) { return Angle::fromRadians(::asin(d)); }
inline Angle acos(float f) { return Angle::fromRadians(::acosf(f)); }
inline Angle acos(double d) { return Angle::fromRadians(::acos(d)); }

}

#endif
