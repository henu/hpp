#ifndef HPP_COLOR_H
#define HPP_COLOR_H

#include "assert.h"
#include "misc.h"
#include "pixelformat.h"

namespace Hpp
{

class Color
{

public:

	// Constructors
	inline Color(void);
	inline Color(float red, float green, float blue, float alpha);
	inline Color(float red, float green, float blue);
	inline Color(float value, float alpha);
	inline Color(float value, Pixelformat format);

	// Comparison operators
	inline bool operator==(Color const& c) const;

	// Operators
	inline Color operator*(float f) const;

	// Values. On grayscale colors, values is stored to red.
	float red, green, blue, alpha;

	// Getters. These will always return proper values based on format
	inline float getRed(void) const;
	inline float getGreen(void) const;
	inline float getBlue(void) const;
	inline float getValue(void) const;
	inline float getAlpha(void) const;

	Pixelformat format;

private:

};



// ----------------------------------------
// ----------------------------------------
//
// Implemented inline functions
//
// ----------------------------------------
// ----------------------------------------

inline Color::Color(void) :
format(DEFAULT)
{
}

inline Color::Color(float red, float green, float blue, float alpha) :
red(red),
green(green),
blue(blue),
alpha(alpha),
format(RGBA)
{
}

inline Color::Color(float red, float green, float blue) :
red(red),
green(green),
blue(blue),
format(RGB)
{
}

inline Color::Color(float value, float alpha) :
red(value),
alpha(alpha),
format(GRAYSCALE_ALPHA)
{
}

inline Color::Color(float value, Pixelformat format) :
format(format)
{
	if (format == GRAYSCALE) {
		red = value;
	} else {
		HppAssert(format == ALPHA, "Invalid color format!");
		alpha = value;
	}
}

inline bool Color::operator==(Color const& c) const
{
	HppAssert(format == c.format, "Unable to compare colors, because they have different pixel format!");
	switch (format)
	{
	case RGB:
		if (red != c.red ||
		    green != c.green ||
		    blue != c.blue) {
			return false;
		}
		break;
	case RGBA:
		if (red != c.red ||
		    green != c.green ||
		    blue != c.blue ||
		    alpha != c.alpha) {
			return false;
		}
		break;
	case GRAYSCALE:
		if (red != c.red) {
			return false;
		}
		break;
	case GRAYSCALE_ALPHA:
		if (red != c.red ||
		    alpha != c.alpha) {
			return false;
		}
		break;
	case ALPHA:
		if (alpha != c.alpha) {
			return false;
		}
		break;
	case DEFAULT:
	default:
		HppAssert(false, "Invalid format!");
	}
	return true;
}

inline Color Color::operator*(float f) const
{
	Color result;
	result.format = format;
	switch (format)
	{
	case RGB:
		result.red = red * f;
		result.green = green * f;
		result.blue = blue * f;
		break;
	case RGBA:
		result.red = red * f;
		result.green = green * f;
		result.blue = blue * f;
		result.alpha = alpha * f;
		break;
	case GRAYSCALE:
		result.red = red * f;
		break;
	case GRAYSCALE_ALPHA:
		result.red = red * f;
		result.alpha = alpha * f;
		break;
	case ALPHA:
		result.alpha = alpha * f;
		break;
	case DEFAULT:
	default:
		HppAssert(false, "Invalid format!");
	}
	return result;
}

inline float Color::getRed(void) const
{
	if (format != ALPHA) return red;
	return 1.0;
}

inline float Color::getGreen(void) const
{
	if (format == RGB || format == RGBA) return green;
	if (format == GRAYSCALE || format == GRAYSCALE_ALPHA) return red;
	return 1.0;
}

inline float Color::getBlue(void) const
{
	if (format == RGB || format == RGBA) return blue;
	if (format == GRAYSCALE || format == GRAYSCALE_ALPHA) return red;
	return 1.0;
}

inline float Color::getValue(void) const
{
	if (format == GRAYSCALE || format == GRAYSCALE_ALPHA) return red;
	if (format == ALPHA) return 1.0;
	return clamp(0.0, 1.0, (red + green + blue) / 3.0);
}

inline float Color::getAlpha(void) const
{
	if (format == RGBA || format == GRAYSCALE_ALPHA || format == ALPHA) return alpha;
	return 1.0;
}

}

#endif
