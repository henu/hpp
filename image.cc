#include "image.h"

#include "cast.h"
#include "exception.h"
#include "assert.h"

#include <SDL/SDL_image.h>
#include <png.h>

namespace Hpp
{

static SDL_Surface* loadSDLSurface(std::string const& filename, uint8_t const* data = NULL, size_t data_size = 0);
static void loadFromSDLSurface(SDL_Surface* surf,
                               Pixelformat given_format,
                               Image::Flags flags,
                               ByteV& data,
                               Pixelformat& format,
                               size_t& width,
                               size_t& height);

Image::Image(void)
{
}

Image::Image(std::string const& filename, Pixelformat format, Flags flags)
{
	if (filename.empty()) {
		throw Exception("No filename given!");
	}
	SDL_Surface* surf = loadSDLSurface(filename);
	loadFromSDLSurface(surf, format, flags, data, this->format, width, height);
	SDL_FreeSurface(surf);
}

Image::Image(Path const& path, Pixelformat format, Flags flags)
{
	if (path.isUnknown()) {
		throw Exception("Given filename is unknown!");
	}
	SDL_Surface* surf = loadSDLSurface(path.toString());
	loadFromSDLSurface(surf, format, flags, data, this->format, width, height);
	SDL_FreeSurface(surf);
}

Image::Image(ByteV const& data, size_t width, size_t height, Pixelformat format, Flags flags)
{
	loadFromPixeldata(&data[0], width, height, format, flags);
}

Image::Image(uint8_t const* data, size_t width, size_t height, Pixelformat format, Flags flags)
{
	loadFromPixeldata(data, width, height, format, flags);
}

Image::Image(ByteV const& filedata, Pixelformat format, Flags flags)
{
	loadFromFiledata(&filedata[0], format, flags);
}

Image::Image(uint8_t const* filedata, Pixelformat format, Flags flags)
{
	loadFromFiledata(filedata, format, flags);
}

Image::Image(size_t width, size_t height, Pixelformat format)
{
	loadFromPixeldata(NULL, width, height, format, 0);
}

Image::~Image(void)
{
}

Image::Image(Image const& image) :
data(image.data),
format(image.format),
width(image.width),
height(image.height)
{
}

Image& Image::operator=(Image const& image)
{
	this->data = image.data;
	this->format = image.format;
	this->width = image.width;
	this->height = image.height;
	return *this;
}

void Image::convertTo(Pixelformat format)
{
	HppAssert(data.size() == getBppOfPixelformat(this->format) * width * height, "Fail!");

	uint8_t bpp = getBppOfPixelformat(format);
	ByteV data_new;
	data_new.reserve(width*height*bpp);

	ByteV::const_iterator data_it = data.begin();
	for (size_t offset = 0; offset < width*height; offset ++) {
		uint8_t red, green, blue, alpha;
		// Read pixel
		switch (this->format) {
		case RGB:
			red = *data_it; data_it ++;
			green = *data_it; data_it ++;
			blue = *data_it; data_it ++;
			alpha = 0xff;
			break;
		case RGBA:
			red = *data_it; data_it ++;
			green = *data_it; data_it ++;
			blue = *data_it; data_it ++;
			alpha = *data_it; data_it ++;
			break;
		case GRAYSCALE:
			red = *data_it;
			green = *data_it;
			blue = *data_it; data_it ++;
			alpha = 0xff;
			break;
		case GRAYSCALE_ALPHA:
			red = *data_it;
			green = *data_it;
			blue = *data_it; data_it ++;
			alpha = *data_it; data_it ++;
			break;
		case ALPHA:
			red = 255;
			green = 255;
			blue = 255;
			alpha = *data_it; data_it ++;
			break;
		case DEFAULT:
		default:
			HppAssert(false, "Invalid format!");
		}
		// Write pixel
		switch (format) {
		case RGB:
			data_new.push_back(red);
			data_new.push_back(green);
			data_new.push_back(blue);
			break;
		case RGBA:
			data_new.push_back(red);
			data_new.push_back(green);
			data_new.push_back(blue);
			data_new.push_back(alpha);
			break;
		case GRAYSCALE:
			data_new.push_back((red + green + blue) / 3);
			break;
		case GRAYSCALE_ALPHA:
			data_new.push_back((red + green + blue) / 3);
			data_new.push_back(alpha);
			break;
		case ALPHA:
			data_new.push_back(alpha);
			break;
		case DEFAULT:
		default:
			HppAssert(false, "Invalid format!");
		}
	}
	HppAssert(data_it == data.end(), "Fail!");
	HppAssert(data_new.size() == width*height*bpp, "Fail!");

	data.swap(data_new);
	this->format = format;

}

void Image::save(Path const& filename, Filetype ft) const
{
	if (ft == FT_BMP) {
		SDL_Surface* surf = convertToSDLSurface();
		if (SDL_SaveBMP(surf, filename.toString().c_str()) != 0) {
			throw Exception("Unable to save image \"" + filename.toString() + "\"! Reason: " + SDL_GetError());
		}
		SDL_FreeSurface(surf);
	} else if (ft == FT_PNG) {

		FILE* fp = fopen(filename.toString().c_str(), "wb");
		if (!fp) {
			throw Exception("Unable to open PNG file \"" + filename.toString() + "\"!");
		}

		// Initialize PNG saving
		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr) {
			fclose(fp);
			throw Exception("Unable to save PNG file!");
		}
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_write_struct(&png_ptr, NULL);
			fclose(fp);
			throw Exception("Unable to save PNG file!");
		}
		if (setjmp(png_jmpbuf(png_ptr))) {
			png_destroy_write_struct(&png_ptr, NULL);
			fclose(fp);
			throw Exception("Unable to save PNG file!");
		}

		// Set file as target for PNG saving.
		png_init_io(png_ptr, fp);

		// Write PNG header. Always use 32 bit pixels
		png_set_IHDR(png_ptr, info_ptr,
		             width, height, 8, PNG_COLOR_TYPE_RGBA,
		             PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);

		// Write image one row at a time.
		png_bytep row = new png_byte[4 * width];
		ByteV::const_iterator pdata_it = data.begin();
		for (size_t y = 0; y < height; ++ y) {
			// Fill row
			for (size_t x = 0; x < width; ++ x) {
				uint32_t red;
				uint32_t green;
				uint32_t blue;
				uint32_t alpha;
				#ifndef NDEBUG
				red = 0;
				green = 0;
				blue = 0;
				alpha = 0;
				#endif
				switch (format) {
				case RGB:
					red = *pdata_it; pdata_it ++;
					green = *pdata_it; pdata_it ++;
					blue = *pdata_it; pdata_it ++;
					alpha = 255;
					break;
				case RGBA:
					red = *pdata_it; pdata_it ++;
					green = *pdata_it; pdata_it ++;
					blue = *pdata_it; pdata_it ++;
					alpha = *pdata_it; pdata_it ++;
					break;
				case GRAYSCALE:
					red = *pdata_it;
					green = *pdata_it;
					blue = *pdata_it;
					pdata_it ++;
					alpha = 255;
					break;
				case GRAYSCALE_ALPHA:
					red = *pdata_it;
					green = *pdata_it;
					blue = *pdata_it;
					pdata_it ++;
					alpha = *pdata_it; pdata_it ++;
					break;
				case ALPHA:
					red = 255;
					green = 255;
					blue = 255;
					alpha = *pdata_it; pdata_it ++;
					break;
				case DEPTH16:
				case DEPTH24:
				case DEPTH32:
				case DEFAULT:
					HppAssertAlways(false, "Fail!");
				}
				row[x*4 + 0] = red;
				row[x*4 + 1] = green;
				row[x*4 + 2] = blue;
				row[x*4 + 3] = alpha;
			}
			png_write_row(png_ptr, row);
		}
		delete[] row;

		// Finalize PNG saving
		png_write_end(png_ptr, NULL);
		fclose(fp);
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_write_struct(&png_ptr, NULL);
	}
}

Image Image::getSubimage(IVector2 begin, IVector2 size) const
{
	// Ensure begin and size do not overflow
	if (begin.x < 0) {
		size.x += begin.x;
		begin.x = 0;
	}
	if (begin.y < 0) {
		size.y += begin.y;
		begin.y = 0;
	}
	if (begin.x >= ssize_t(width)) begin.x = 0;
	if (begin.y >= ssize_t(height)) begin.y = 0;
	if (begin.x + size.x > ssize_t(width)) size.x = width - begin.x;
	if (begin.y + size.y > ssize_t(height)) size.y = height - begin.y;
	if (size.x < 0) size.x = 0;
	if (size.y < 0) size.y = 0;

	Image result(NULL, size.x, size.y, format);

	IVector2 it;
	for (it.y = 0; it.y < size.y; ++ it.y) {
		for (it.x = 0; it.x < size.x; ++ it.x) {
			Color pixel = getPixel(begin + it);
			result.setPixel(it, pixel);
		}
	}

	return result;
}

static SDL_Surface* loadSDLSurface(std::string const& filename, uint8_t const* data, size_t data_size)
{
	if (!filename.empty()) {
		SDL_Surface* result = IMG_Load(filename.c_str());
		if (!result) {
			throw Exception(std::string("Unable to load image \"" + filename + "\"! Reason: ") + IMG_GetError());
		}
		return result;
	}

	HppAssert(data && data_size > 0, "No data or filename given!");

	SDL_RWops* rw = SDL_RWFromMem((void*)data, data_size);
	if (!rw) {
		throw Exception(std::string("Unable to allocate SDL_RWops! Reason: ") + IMG_GetError());
	}
	SDL_Surface* surf = IMG_Load_RW(rw, true);
	if (!surf) {
		throw Exception(std::string("Unable to load image from raw file data! Reason: ") + IMG_GetError());
	}

	return surf;
}


static void loadFromSDLSurface(SDL_Surface* surf,
                               Pixelformat given_format,
                               Image::Flags flags,
                               ByteV& data,
                               Pixelformat& format,
                               size_t& width,
                               size_t& height)
{
	HppAssert(surf, "No surface given!");

	width = surf->w;
	height = surf->h;

	SDL_PixelFormat* pf = surf->format;
	if (given_format != DEFAULT) {
		format = given_format;
	} else {
		if (pf->BitsPerPixel == 8) {
			format = GRAYSCALE;
		} else if (pf->BitsPerPixel == 16) {
			format = GRAYSCALE_ALPHA;
		} else if (pf->BitsPerPixel == 24) {
			format = RGB;
		} else if (pf->BitsPerPixel == 32) {
			format = RGBA;
		} else {
			throw Exception("Unknown image format of " + sizeToStr(pf->BitsPerPixel) + " bits per pixel!");
		}
	}

	data.clear();

	data.reserve(width*height*getBppOfPixelformat(format));

	// Copy pixeldata
	if (SDL_LockSurface(surf)) {
		throw Exception("Unable to lock SDL surface!");
	}
	uint8_t* pixels_offset = (uint8_t*)(surf->pixels);
	uint8_t pixel_raw[4] = { 0, 0, 0, 0 };
	bool alpha_found = false;
	bool all_gray = true;
	size_t pitch_extra = surf->pitch - width*pf->BytesPerPixel;
	for (size_t y = 0; y < height; y ++) {
		for (size_t x = 0; x < width; x ++) {
			for (uint8_t pixel_read = 0; pixel_read < pf->BytesPerPixel; pixel_read ++) {
				pixel_raw[pixel_read] = *pixels_offset;
				pixels_offset ++;
			}
			void* pixel_raw_raw = pixel_raw;
			uint32_t pixel = *(uint32_t*)(pixel_raw_raw);
			uint32_t red = (pixel & pf->Rmask) >> pf->Rshift; red = (red << pf->Rloss) & 0xff;
			uint32_t green = (pixel & pf->Gmask) >> pf->Gshift; green = (green << pf->Gloss) & 0xff;
			uint32_t blue = (pixel & pf->Bmask) >> pf->Bshift; blue = (blue << pf->Bloss) & 0xff;
			uint32_t alpha = (pixel & pf->Amask) >> pf->Ashift; alpha = (alpha << pf->Aloss) & 0xff;
			// Do some statistic checks
			if (red != green || red != blue) all_gray = false;
			if (alpha < 255) alpha_found = true;
			switch (format) {
			case RGB:
				data.push_back(red);
				data.push_back(green);
				data.push_back(blue);
				break;
			case RGBA:
				data.push_back(red);
				data.push_back(green);
				data.push_back(blue);
				data.push_back(alpha);
				break;
			case GRAYSCALE:
				data.push_back(red + green + blue / 3);
				break;
			case GRAYSCALE_ALPHA:
				data.push_back(red + green + blue / 3);
				data.push_back(alpha);
				break;
			case ALPHA:
				data.push_back(alpha);
				break;
			case DEPTH16:
			case DEPTH24:
			case DEPTH32:
			case DEFAULT:
				HppAssert(false, "Fail!");
			}
		}
		pixels_offset += pitch_extra;
	}
	SDL_UnlockSurface(surf);

	if ((flags & Image::DONT_DROP_USELESS_ALPHA) == 0 && !alpha_found) {

		if (format == RGBA) {
			ByteV new_data;
			new_data.reserve(data.size() * 3 / 4);
			for (ByteV::const_iterator data_it = data.begin();
			     data_it != data.end();
			     data_it += 4) {
				new_data.push_back(*(data_it + 0));
				new_data.push_back(*(data_it + 1));
				new_data.push_back(*(data_it + 2));
				HppAssert(*(data_it + 3) == 255, "Fail!");
			}
			data.swap(new_data);
			format = RGB;
		} else if (format == GRAYSCALE_ALPHA) {
			ByteV new_data;
			new_data.reserve(data.size() / 2);
			for (ByteV::const_iterator data_it = data.begin();
			     data_it != data.end();
			     data_it += 2) {
				new_data.push_back(*(data_it + 0));
				HppAssert(*(data_it + 1) == 255, "Fail!");
			}
			data.swap(new_data);
			format = GRAYSCALE;
		}
	}
	if ((flags & Image::DONT_DETECT_GRAYSCALE) == 0 && all_gray) {

		if (format == RGB) {
			ByteV new_data;
			new_data.reserve(data.size() / 3);
			for (ByteV::const_iterator data_it = data.begin();
			     data_it != data.end();
			     data_it += 3) {
				new_data.push_back(*(data_it + 0));
				HppAssert(*(data_it + 0) == *(data_it + 1), "Fail!");
				HppAssert(*(data_it + 0) == *(data_it + 2), "Fail!");
			}
			data.swap(new_data);
			format = GRAYSCALE;
		} else if (format == RGBA) {
			ByteV new_data;
			new_data.reserve(data.size() / 2);
			for (ByteV::const_iterator data_it = data.begin();
			     data_it != data.end();
			     data_it += 4) {
				new_data.push_back(*(data_it + 0));
				new_data.push_back(*(data_it + 3));
				HppAssert(*(data_it + 0) == *(data_it + 1), "Fail!");
				HppAssert(*(data_it + 0) == *(data_it + 2), "Fail!");
			}
			data.swap(new_data);
			format = GRAYSCALE_ALPHA;
		}
	}


}

SDL_Surface* Image::convertToSDLSurface(void) const
{
	HppAssert(!data.empty(), "Image is not loaded!");

	// SDL surfaces are always 32 bits. At least for now for now.
	uint8_t bitspp = 32;
	uint32_t flags = SDL_SWSURFACE;
	// Masks
	uint32_t rmask, gmask, bmask, amask;
//	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
/*
	#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
	#endif
*/

// TODO: There is a problem. This can be called only when display is opened!
	SDL_Surface* surf = SDL_CreateRGBSurface(flags, width, height, bitspp,
	                                         rmask, gmask, bmask, amask);
	if (!surf) {
		throw Exception("Unable to convert image to SDL_Surface!");
	}

	// Store pixels
	SDL_PixelFormat* pf = surf->format;
	uint8_t surf_bpp = pf->BytesPerPixel;
	if (SDL_LockSurface(surf)) {
		throw Exception("Unable to lock SDL surface when converting image to SDL_Surface!");
	}
	ByteV::const_iterator pdata_it = data.begin();
	for (size_t offset = 0; offset < width * height; ++ offset) {
		uint32_t red;
		uint32_t green;
		uint32_t blue;
		uint32_t alpha;
		#ifndef NDEBUG
		red = 0;
		green = 0;
		blue = 0;
		alpha = 0;
		#endif
		switch (format) {
		case RGB:
			red = *pdata_it; pdata_it ++;
			green = *pdata_it; pdata_it ++;
			blue = *pdata_it; pdata_it ++;
			alpha = 255;
			break;
		case RGBA:
			red = *pdata_it; pdata_it ++;
			green = *pdata_it; pdata_it ++;
			blue = *pdata_it; pdata_it ++;
			alpha = *pdata_it; pdata_it ++;
			break;
		case GRAYSCALE:
			red = *pdata_it;
			green = *pdata_it;
			blue = *pdata_it;
			pdata_it ++;
			alpha = 255;
			break;
		case GRAYSCALE_ALPHA:
			red = *pdata_it;
			green = *pdata_it;
			blue = *pdata_it;
			pdata_it ++;
			alpha = *pdata_it; pdata_it ++;
			break;
		case ALPHA:
			red = 255;
			green = 255;
			blue = 255;
			alpha = *pdata_it; pdata_it ++;
			break;
		case DEPTH16:
		case DEPTH24:
		case DEPTH32:
		case DEFAULT:
			HppAssertAlways(false, "Fail!");
		}
		uint32_t pixel = (((red >> pf->Rloss) << pf->Rshift) & pf->Rmask) |
				 (((green >> pf->Gloss) << pf->Gshift) & pf->Gmask) |
				 (((blue >> pf->Bloss) << pf->Bshift) & pf->Bmask) |
				 (((alpha >> pf->Aloss) << pf->Ashift) & pf->Amask);
		uint8_t* pixel_raw = reinterpret_cast< uint8_t* >(&pixel);
		for (uint8_t byte = 0; byte < surf_bpp; ++ byte) {
			static_cast< uint8_t* >(surf->pixels)[offset * surf_bpp + byte] = pixel_raw[byte];
		}
	}
	SDL_UnlockSurface(surf);

	return surf;

}

void Image::loadFromPixeldata(uint8_t const* data, size_t width, size_t height, Pixelformat format, Flags flags)
{
	this->data.clear();
	uint8_t bpp = getBppOfPixelformat(format);
	// Copy values
	if (data) {
		if (flags & FLIP_VERTICALLY) {
			this->data.reserve(width * height * bpp);
			for (size_t y = height; y > 0; y --) {
				uint8_t const* data_it = data + (y - 1) * width * bpp;
				this->data.insert(this->data.end(), data_it, data_it + width * bpp);
			}
		} else {
			this->data = ByteV(data, data + width * height * bpp);
		}
	} else {
		this->data.assign(width * height * bpp, 0);
	}
	this->width = width;
	this->height = height;
	this->format = format;
// TODO: Check for useless alpha or grayscale!
(void)flags;
	HppAssert(this->data.size() == getBppOfPixelformat(this->format) * this->width * this->height, "Fail!");
}

void Image::loadFromFiledata(uint8_t const* filedata, Pixelformat format, Flags flags)
{
	HppAssert(filedata, "Must be not NULL!");
	SDL_Surface* surf = loadSDLSurface("", filedata);
	loadFromSDLSurface(surf, format, flags, data, this->format, width, height);
	SDL_FreeSurface(surf);
}

}
