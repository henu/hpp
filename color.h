#ifndef HPP_COLOR_H
#define HPP_COLOR_H

#include "cast.h"
#include "json.h"
#include "assert.h"
#include "misc.h"
#include "pixelformat.h"
#include <ostream>

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
	inline Color(Json const& json);

	// Comparison operators
	inline bool operator==(Color const& c) const;

	// Operators
	inline Color operator*(float f) const;

	// Getters. These will always return proper values based on format
	inline float getRed(void) const;
	inline float getGreen(void) const;
	inline float getBlue(void) const;
	inline float getValue(void) const;
	inline float getAlpha(void) const;

	inline Pixelformat getFormat(void) const { return format; }

	// Conversion functions
	inline std::string toString(void) const;
	inline Json toJson(void) const;

	// Returns grey version of color
	inline Color getGrey(void) const;

private:

	// Values. On grayscale colors, values is stored to red.
	float red, green, blue, alpha;

	Pixelformat format;

};

inline std::ostream& operator<<(std::ostream& strm, Color const& c);



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

inline Color::Color(Json const& json)
{
	if (json.getType() == Json::NUMBER) {
		red = json.getNumber();
		format = GRAYSCALE;
	} else if (json.getType() != Json::ARRAY) {
		throw Exception("Color must be number or array!");
	} else if (json.getArraySize() == 1) {
		red = json.getItem(0).getNumber();
		format = GRAYSCALE;
	} else if (json.getArraySize() == 2) {
		red = json.getItem(0).getNumber();
		alpha = json.getItem(1).getNumber();
		format = GRAYSCALE_ALPHA;
	} else if (json.getArraySize() == 3) {
		red = json.getItem(0).getNumber();
		green = json.getItem(1).getNumber();
		blue = json.getItem(2).getNumber();
		format = RGB;
	} else if (json.getArraySize() == 4) {
		red = json.getItem(0).getNumber();
		green = json.getItem(1).getNumber();
		blue = json.getItem(2).getNumber();
		alpha = json.getItem(3).getNumber();
		format = RGBA;
	} else {
		throw Exception("Invalid number of components in color!");
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

inline std::string Color::toString(void) const
{
	std::string result = "[" + floatToStr(getRed()) + ", " + floatToStr(getGreen()) + ", " + floatToStr(getBlue());
	if (getAlpha() < 1) {
		result += ", " + floatToStr(getAlpha());
	}
	result += "]";
	return result;
}

inline Json Color::toJson(void) const
{
	Json result = Json::newArray();
	result.addItem(Json::newNumber(getRed()));
	result.addItem(Json::newNumber(getGreen()));
	result.addItem(Json::newNumber(getBlue()));
	if (getAlpha() < 1) {
		result.addItem(Json::newNumber(getAlpha()));
	}
	return result;
}

inline Color Color::getGrey(void) const
{
	float g = (getRed() + getGreen() + getBlue()) / 3;
	return Color(g, getAlpha());
}

inline std::ostream& operator<<(std::ostream& strm, Color const& c)
{
	return strm << c.toString();
}

}

#endif
