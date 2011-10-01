#include "angle.h"

#include "cast.h"

namespace Hpp
{

std::string Angle::toRadiansStr(void) const
{
	return floatToStr(rad);
}

std::string Angle::toDegreesStr(void) const
{
	return floatToStr(rad / HPP_PI * 180) + "°";
}

}
