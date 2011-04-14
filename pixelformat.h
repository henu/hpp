#ifndef HPP_PIXELFORMAT_H
#define HPP_PIXELFORMAT_H

#include "assert.h"

#include <stdint.h>

namespace Hpp
{

enum Pixelformat { RGB,
                   RGBA,
                   GRAYSCALE,
                   GRAYSCALE_ALPHA,
                   ALPHA,
                   DEFAULT };

inline uint8_t getBppOfPixelformat(Pixelformat format)
{
	switch (format)
	{
	case RGB:
		return 3;
	case RGBA:
		return 4;
	case GRAYSCALE:
		return 1;
	case GRAYSCALE_ALPHA:
		return 2;
	case ALPHA:
		return 1;
	case DEFAULT:
	default:
		HppAssert(false, "Invalid format!");
	}
	return 0;
}

inline bool pixelformatHasAlpha(Pixelformat format)
{
	return format == RGBA || format == GRAYSCALE_ALPHA || format == ALPHA;
}

}

#endif
