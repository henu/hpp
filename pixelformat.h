#ifndef HPP_PIXELFORMAT_H
#define HPP_PIXELFORMAT_H

#ifndef HPP_NO_3D
#include "inc_gl.h"
#endif
#include "assert.h"

#include <stdint.h>

namespace Hpp
{

enum Pixelformat { RGB,
                   RGBA,
                   GRAYSCALE,
                   GRAYSCALE_ALPHA,
                   ALPHA,
                   DEPTH16,
                   DEPTH24,
                   DEPTH32,
                   DEFAULT };

// Returns BytesPerPixel that is used outside OpenGL. Because of this,
// even 16 and 24 bit depth components are considered 32 bit floats.
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
	case DEPTH16:
		return 4;
	case DEPTH24:
		return 4;
	case DEPTH32:
		return 4;
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

inline bool pixelformatHasDepth(Pixelformat format)
{
	return format == DEPTH16 || format == DEPTH24 || format == DEPTH32;
}

#ifndef HPP_NO_3D
inline GLenum pixelformatToGlFormat(Pixelformat format)
{
	switch (format)
	{
	case RGB:
		return GL_RGB;
	case RGBA:
		return GL_RGBA;
	case GRAYSCALE:
		return GL_LUMINANCE;
	case GRAYSCALE_ALPHA:
		return GL_LUMINANCE_ALPHA;
	case ALPHA:
		return GL_ALPHA;
	case DEPTH16:
		return GL_DEPTH_COMPONENT;
	case DEPTH24:
		return GL_DEPTH_COMPONENT;
	case DEPTH32:
		return GL_DEPTH_COMPONENT;
	case DEFAULT:
	default:
		HppAssert(false, "Invalid format!");
	}
	return 0;
}
#endif

#ifndef HPP_NO_3D
inline GLint pixelformatToGlInternalFormat(Pixelformat format)
{
	switch (format)
	{
	case RGB:
		return GL_RGB;
	case RGBA:
		return GL_RGBA;
	case GRAYSCALE:
		return GL_LUMINANCE;
	case GRAYSCALE_ALPHA:
		return GL_LUMINANCE_ALPHA;
	case ALPHA:
		return GL_ALPHA;
	case DEPTH16:
		return GL_DEPTH_COMPONENT16;
	case DEPTH24:
		return GL_DEPTH_COMPONENT24;
	case DEPTH32:
		return GL_DEPTH_COMPONENT32F;
	case DEFAULT:
	default:
		HppAssert(false, "Invalid format!");
	}
	return 0;
}
#endif

}

#endif
