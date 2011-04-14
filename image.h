#ifndef HPP_IMAGE_H
#define HPP_IMAGE_H

#include "ivectors.h"
#include "color.h"
#include "path.h"
#include "assert.h"
#include "pixelformat.h"
#include "bytev.h"
#include "assert.h"
#include "misc.h"

#include <SDL.h>
#include <string>

namespace Hpp
{

class Image
{

public:

	typedef uint8_t Flags;
	static Flags const DONT_DROP_USELESS_ALPHA	= 0x01;
	static Flags const DONT_DETECT_GRAYSCALE	= 0x02;
	static Flags const FLIP_VERTICALLY		= 0x04;

	Image(void);
// TODO: Remove support for C style filename!
	Image(std::string const& filename, Pixelformat format = DEFAULT, Flags flags = 0);
	Image(Path const& path, Pixelformat format = DEFAULT, Flags flags = 0);
	Image(ByteV const& data, size_t width, size_t height, Pixelformat format, Flags flags = 0);
	Image(uint8_t const* data, size_t width, size_t height, Pixelformat format, Flags flags = 0);
	Image(ByteV const& filedata, Pixelformat format = DEFAULT, Flags flags = 0);
	Image(uint8_t const* filedata, Pixelformat format = DEFAULT, Flags flags = 0);
	~Image(void);

	// Converts format of this Image
	void convertTo(Pixelformat format);

	// Functions to save image. Only BMP format is supported for now!
	void save(Path const& filename) const;

	// Getters
	inline ByteV getData(void) const { return data; }
	inline ByteV getDataFlipped(void) const;
	inline Pixelformat getFormat(void) const { return format; }
	inline size_t getWidth(void) const { return width; }
	inline size_t getHeight(void) const { return height; }
	inline uint8_t getBytesPerPixel(void) const { return getBppOfPixelformat(format); }
	inline Color getPixel(size_t offset) const;
	inline Color getPixel(IVector2 const& pos) const;
	inline Color getPixel(size_t x, size_t y) const;

	inline bool hasAlpha(void) const { return pixelformatHasAlpha(format); }

	// Adds pixel. Like setPixel(), but effect of new color depends its
	// alpha. Also, the alpha of image is never decreased by this operation.
	inline void addPixel(size_t offset, Color const& color);
	inline void addPixel(IVector2 const& pos, Color const& color);
	inline void addPixel(size_t x, size_t y, Color const& color);

private:

	ByteV data;
	Pixelformat format;
	size_t width, height;

	SDL_Surface* convertToSDLSurface(void) const;

	void loadFromPixeldata(uint8_t const* data, size_t width, size_t height, Pixelformat format, Flags flags);
	void loadFromFiledata(uint8_t const* filedata, Pixelformat format, Flags flags);
};

inline ByteV Image::getDataFlipped(void) const
{
	size_t scanline = width * getBytesPerPixel();
	ByteV result;
	result.reserve(data.size());
	for (ssize_t y = height - 1; y >= 0; y --) {
		result.insert(result.end(), data.begin() + y * scanline, data.begin() + (y + 1) * scanline);
	}
	HppAssert(result.size() == data.size(), "Fail!");
	return result;
}

inline Color Image::getPixel(size_t offset) const
{
	switch (format) {
	case RGB:
		{
			ByteV::const_iterator data_it = data.begin() + offset * 3;
			return Color(*(data_it + 0) / 255.0, *(data_it + 1) / 255.0, *(data_it + 2) / 255.0);
		}
	case RGBA:
		{
			ByteV::const_iterator data_it = data.begin() + offset * 4;
			return Color(*(data_it + 0) / 255.0, *(data_it + 1) / 255.0, *(data_it + 2) / 255.0, *(data_it + 3) / 255.0);
		}
	case GRAYSCALE:
		return Color(data[offset] / 255.0, GRAYSCALE);
	case GRAYSCALE_ALPHA:
		{
			ByteV::const_iterator data_it = data.begin() + offset * 2;
			return Color(*(data_it + 0) / 255.0, *(data_it + 1) / 255.0);
		}
	case ALPHA:
		return Color(data[offset] / 255.0, ALPHA);
	default:
		HppAssert(false, "Invalid image format!");
	}
	return Color(0, 0, 0);
}

inline Color Image::getPixel(IVector2 const& pos) const
{
	return getPixel(pos.y * width + pos.x);
}

inline Color Image::getPixel(size_t x, size_t y) const
{
	return getPixel(y * width + x);
}

inline void Image::addPixel(size_t offset, Color const& color)
{
	switch (format) {
	case RGB:
		{
			size_t offset_fixed = offset*3;
			float alpha = color.getAlpha();
			data[offset_fixed+0] = clamp< int >(0, 255, data[offset_fixed+0] * (1-alpha) + 255 * color.getRed() * alpha);
			data[offset_fixed+1] = clamp< int >(0, 255, data[offset_fixed+1] * (1-alpha) + 255 * color.getGreen() * alpha);
			data[offset_fixed+2] = clamp< int >(0, 255, data[offset_fixed+2] * (1-alpha) + 255 * color.getBlue() * alpha);
		}
		break;
	case RGBA:
		{
			size_t offset_fixed = offset*4;
			float alpha = color.getAlpha();
			data[offset_fixed+0] = clamp< int >(0, 255, data[offset_fixed+0] * (1-alpha) + 255 * color.getRed() * alpha);
			data[offset_fixed+1] = clamp< int >(0, 255, data[offset_fixed+1] * (1-alpha) + 255 * color.getGreen() * alpha);
			data[offset_fixed+2] = clamp< int >(0, 255, data[offset_fixed+2] * (1-alpha) + 255 * color.getBlue() * alpha);
			data[offset_fixed+3] = clamp< int >(0, 255, data[offset_fixed+3] * (1-alpha) + 255 * alpha);
		}
		break;
	case GRAYSCALE:
		{
			float alpha = color.getAlpha();
			data[offset] = clamp< int >(0, 255, data[offset] * (1-alpha) + 255 * color.getValue() * alpha);
		}
		break;
	case GRAYSCALE_ALPHA:
		{
			size_t offset_fixed = offset*2;
			float alpha = color.getAlpha();
			data[offset_fixed+0] = clamp< int >(0, 255, data[offset_fixed+0] * (1-alpha) + 255 * color.getValue() * alpha);
			data[offset_fixed+1] = clamp< int >(0, 255, data[offset_fixed+1] * (1-alpha) + 255 * alpha);
		}
		break;
	case ALPHA:
		{
			float alpha = color.getAlpha();
			data[offset] = clamp< int >(0, 255, data[offset] * (1-alpha) + 255 * alpha);
		}
		break;
	default:
		HppAssert(false, "Invalid image format!");
	}
}

inline void Image::addPixel(IVector2 const& pos, Color const& color)
{
	addPixel(pos.y * width + pos.x, color);
}

inline void Image::addPixel(size_t x, size_t y, Color const& color)
{
	addPixel(y * width + x, color);
}

}

#endif
