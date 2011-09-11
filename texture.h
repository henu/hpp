#ifndef HPP_TEXTURE_H
#define HPP_TEXTURE_H

#include "texturemanager.h"
#include "inc_gl.h"
#include "gldebug.h"
#include "bytev.h"
#include "pixelformat.h"
#include "glsystem.h"
#include "display.h"
#include "image.h"
#include "exception.h"
#include "assert.h"

#include <string>

namespace Hpp
{

class Texture
{

public:

	// Flags
	static uint32_t const REMOVE_ALPHA           = 0x00000001;
	static uint32_t const FORCE_ALPHA            = 0x00000002;
	static uint32_t const NEAREST                = 0x00000004;
	static uint32_t const LINEAR                 = 0x00000008;
	static uint32_t const NEAREST_MIPMAP_NEAREST = 0x00000010;
	static uint32_t const LINEAR_MIPMAP_NEAREST  = 0x00000020;
	static uint32_t const NEAREST_MIPMAP_LINEAR  = 0x00000040;
	static uint32_t const LINEAR_MIPMAP_LINEAR   = 0x00000080;
	static uint32_t const FORCE_GRAYSCALE        = 0x00000100;
	static uint32_t const MINIMIZE_TO_64         = 0x00000200;


	// ----------------------------------------
	// Public functions
	// ----------------------------------------

	// Constructors and destructor
	inline Texture(void);
	inline Texture(Path const& path, Pixelformat format = DEFAULT, uint32_t flags = 0);
	inline Texture(ByteV const& data, Pixelformat format = DEFAULT, uint32_t flags = 0);
	inline ~Texture(void);

	// Checks if Texture is loaded
	inline bool loaded(void) const { return is_loaded || tempdata; }

	// Functions to (re)load Texture
	inline void loadFromFile(Path const& path, Pixelformat format = DEFAULT, uint32_t flags = 0);
	inline void loadFromData(ByteV const& data, Pixelformat format = DEFAULT, uint32_t flags = 0);

	// Function to create a new empty Texture
	inline void createNew(uint16_t width, uint16_t height, Pixelformat format, uint32_t flags = 0);

	// Gets width/height of texture
	inline uint16_t getWidth(void) const { HppAssert(loaded(), "Texture is not loaded!"); return width; }
	inline uint16_t getHeight(void) const { HppAssert(loaded(), "Texture is not loaded!"); return height; }

	// Function to get GL texture
	inline GLuint getGlTexture(void) const;

	// Checks if texture has alpha
	inline bool hasAlpha(void) const { HppAssert(loaded(), "Texture is not loaded!"); return pixelformatHasAlpha(format); }

	// Some getters. When getting pixel data, rendering thread is required.
	inline Pixelformat getFormat(void) const { return format; }
	inline ByteV getData(void) const;

	// Function to add a rectangular area of raw pixel data to texture.
	// Pixel data must not flow over texture borders. If flipped is set to
	// true, then data is flipped in vertical axis.
	inline void addPixeldata(uint16_t width, uint16_t height,
	                         uint16_t x_offset, uint16_t y_offset,
	                         unsigned char* data, bool flipped = false);

private:

	// ----------------------------------------
	// Private types
	// ----------------------------------------

	// Type for raw pixeldata. This is a temporary storage for pixels and
	// can be used also when texture is loaded from other thread that main
	// OpenGL-thread.
	struct Pixeldata
	{
		ByteV buf;
		size_t width, height;
		uint8_t bytes_per_pixel;
		bool alpha;
		GLint min_filter;
	};


	// ----------------------------------------
	// Data
	// ----------------------------------------

	mutable GLuint texture;
	mutable bool is_loaded;
	mutable uint16_t width, height;
	mutable Pixelformat format;
	mutable Pixeldata* tempdata;


	// ----------------------------------------
	// Private functions
	// ----------------------------------------

	// Generates new imagedata with different size from original data/size.
	inline static uint8_t* resizeImagedata(uint8_t const* orig_data,
	                                       size_t orig_width, size_t orig_height,
	                                       size_t new_width, size_t new_height,
	                                       size_t bytesperpixel);

	// Resizes the given Pixeldata struct
	inline static void resizePixeldata(Pixeldata& pdata, size_t new_width, size_t new_height);

	inline static Pixeldata load(Path const& path,
	                             ByteV const& data,
	                             bool load_from_data,
	                             Pixelformat format,
	                             uint32_t flags);

	// Tries to apply pixel data. If this is not correct OpenGL thread,
	// then pixel data is stored in temporary location. Note, that this
	// will anyway set all variables that is possible to set, like pixel
	// format, etc.
	void tryToUsePixeldata(Pixeldata const& pdata);

	// Applies pixel data. Must be called from correct OpenGL thread.
	void usePixeldata(Pixeldata pdata) const;

	// Releases gl texture thread safely
	void releaseGlTexture(void);

};

inline Texture::Texture(void) :
is_loaded(false),
tempdata(NULL)
{
}

inline Texture::Texture(Path const& path, Pixelformat format, uint32_t flags) :
is_loaded(false),
tempdata(NULL)
{
	HppCheckGlErrorsIfCorrectThread();
	loadFromFile(path, format, flags);
	HppCheckGlErrorsIfCorrectThread();
}

inline Texture::Texture(ByteV const& data, Pixelformat format, uint32_t flags) :
is_loaded(false),
tempdata(NULL)
{
	HppCheckGlErrorsIfCorrectThread();
	loadFromData(data, format, flags);
	HppCheckGlErrorsIfCorrectThread();
}

inline Texture::~Texture(void)
{
	if (is_loaded) {
		releaseGlTexture();
	}
	delete tempdata;
}

inline void Texture::loadFromFile(Path const& path, Pixelformat format, uint32_t flags)
{
	Pixeldata pdata = load(path, ByteV(), false, format, flags);
	tryToUsePixeldata(pdata);
}

inline void Texture::loadFromData(ByteV const& data, Pixelformat format, uint32_t flags)
{
	Pixeldata pdata = load(Path::getUnknown(), data, true, format, flags);
	tryToUsePixeldata(pdata);
}

inline void Texture::createNew(uint16_t width, uint16_t height, Pixelformat format, uint32_t flags)
{
	uint8_t bytes_per_pixel = 0;
	bool alpha = false;
	switch (format) {
	case RGB:
		bytes_per_pixel = 3;
		alpha = false;
		break;
	case RGBA:
		bytes_per_pixel = 4;
		alpha = true;
		break;
	case GRAYSCALE:
		bytes_per_pixel = 1;
		alpha = false;
		break;
	case GRAYSCALE_ALPHA:
		bytes_per_pixel = 2;
		alpha = true;
		break;
	case ALPHA:
		bytes_per_pixel = 1;
		alpha = true;
		break;
	case DEFAULT:
		throw Exception("Cannot set format \"DEFAULT\" when creating new empty image!");
	}

	// Read flags
	if (flags & REMOVE_ALPHA) {
		throw Exception("Flag \"REMOVE_ALPHA\" cannot be used when creating new empty texture!");
	}
	if (flags & FORCE_ALPHA) {
		throw Exception("Flag \"FORCE_ALPHA\" cannot be used when creating new empty texture!");
	}
	if (flags & FORCE_GRAYSCALE) {
		throw Exception("Flag \"FORCE_GRAYSCALE\" cannot be used when creating new empty texture!");
	}

	GLint min_filter = GL_LINEAR_MIPMAP_LINEAR;
	if (flags & NEAREST) {
		min_filter = GL_NEAREST;
	}
	if (flags & LINEAR) {
		min_filter = GL_LINEAR;
	}
	if (flags & NEAREST_MIPMAP_NEAREST) {
		min_filter = GL_NEAREST_MIPMAP_NEAREST;
	}
	if (flags & LINEAR_MIPMAP_NEAREST) {
		min_filter = GL_LINEAR_MIPMAP_NEAREST;
	}
	if (flags & NEAREST_MIPMAP_LINEAR) {
		min_filter = GL_NEAREST_MIPMAP_LINEAR;
	}
	if (flags & LINEAR_MIPMAP_LINEAR) {
		min_filter = GL_LINEAR_MIPMAP_LINEAR;
	}

	Pixeldata pdata;
	pdata.buf.resize(width * height * bytes_per_pixel, 0);
	pdata.width = width;
	pdata.height = height;
	pdata.bytes_per_pixel = bytes_per_pixel;
	pdata.alpha = alpha;
	pdata.min_filter = min_filter;

	tryToUsePixeldata(pdata);

}

inline GLuint Texture::getGlTexture(void) const
{
	if (tempdata) {
		Pixeldata* pdata = tempdata;
		tempdata = NULL;
		usePixeldata(*pdata);
		delete pdata;
	} else if (!is_loaded) {
		throw Exception("Texture is not yet loaded");
	}
	return texture;
}

ByteV Texture::getData(void) const
{
	HppAssert(loaded(), "Texture is not loaded!");
	HppCheckForCorrectThread();

	// Get specs from current texture
	glBindTexture(GL_TEXTURE_2D, texture);
	HppCheckGlErrors();
	GLsizei tex_width;
	GLsizei tex_height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_height);
	HppCheckGlErrors();

	// Get BPP and format
	GLenum gl_format;
	uint8_t bytes_per_pixel;
	#ifndef NDEBUG
	gl_format = rand();
	bytes_per_pixel = rand();
	#endif
	switch (format) {
	case RGB:
		bytes_per_pixel = 3;
		gl_format = GL_RGB;
		break;
	case RGBA:
		bytes_per_pixel = 4;
		gl_format = GL_RGBA;
		break;
	case GRAYSCALE:
		bytes_per_pixel = 1;
		gl_format = GL_LUMINANCE;
		break;
	case GRAYSCALE_ALPHA:
		bytes_per_pixel = 2;
		gl_format = GL_LUMINANCE_ALPHA;
		break;
	case ALPHA:
		bytes_per_pixel = 1;
		gl_format = GL_ALPHA;
		break;
	default:
		HppAssert(false, "Invalid format");
	}

	// Get image data
	uint8_t* tex_data = new uint8_t[tex_width * tex_height * bytes_per_pixel];
	glGetTexImage(GL_TEXTURE_2D, 0, gl_format, GL_UNSIGNED_BYTE, tex_data);
	HppCheckGlErrors();

	ByteV result(tex_data, tex_data + tex_width * tex_height * bytes_per_pixel);
	delete[] tex_data;

	return result;
}

inline void Texture::addPixeldata(uint16_t width, uint16_t height,
                                  uint16_t x_offset, uint16_t y_offset,
                                  unsigned char* data, bool flipped)
{

	HppAssert(loaded(), "Texture is not loaded!");
	HppCheckForCorrectThread();
// TODO: Code this to use the new pixelstuff also!
HppAssert(!tempdata, "Adding pixeldata with tempdata not implemented yet!");

	// Get specs from  current texture
	glBindTexture(GL_TEXTURE_2D, texture);
	HppCheckGlErrors();
	GLsizei tex_width;
	GLsizei tex_height;
	GLint min_filter;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_height);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);
	HppCheckGlErrors();

	// Ensure pixel data won't overflow
// TODO: Make this algorithm to allow overflow in future!
	HppAssert(x_offset + width <= tex_width, "Overflow over image borders is not allowed (for now)!");
	HppAssert(y_offset + height <= tex_height, "Overflow over image borders is not allowed (for now)!");

	// Get BPP and format
	GLint internalformat;
	GLenum gl_format;
	uint8_t bytes_per_pixel;
	#ifndef NDEBUG
	internalformat = 0;
	gl_format = 0;
	bytes_per_pixel = 0;
	#endif
	switch (format) {
	case RGB:
		bytes_per_pixel = 3;
		internalformat = GL_RGB;
		gl_format = GL_RGB;
		break;
	case RGBA:
		bytes_per_pixel = 4;
		internalformat = GL_RGBA;
		gl_format = GL_RGBA;
		break;
	case GRAYSCALE:
		bytes_per_pixel = 1;
		internalformat = GL_LUMINANCE;
		gl_format = GL_LUMINANCE;
		break;
	case GRAYSCALE_ALPHA:
		bytes_per_pixel = 2;
		internalformat = GL_LUMINANCE_ALPHA;
		gl_format = GL_LUMINANCE_ALPHA;
		break;
	case ALPHA:
		bytes_per_pixel = 1;
		internalformat = GL_ALPHA;
		gl_format = GL_ALPHA;
		break;
	default:
		HppAssert(false, "Invalid format");
	}

	// Get image data
	uint8_t* tex_data = new uint8_t[tex_width * tex_height * bytes_per_pixel];
	glGetTexImage(GL_TEXTURE_2D, 0, gl_format, GL_UNSIGNED_BYTE, tex_data);
	HppCheckGlErrors();

	// Add pixel data
	uint8_t* tex_data_it = tex_data + (y_offset * tex_width + x_offset) * bytes_per_pixel;
	unsigned char* data_it;
	if (flipped) {
		data_it = data + (width * (height - 1)) * bytes_per_pixel;
	} else {
		data_it = data;
	}
	for (unsigned int copy_y = 0; copy_y < height; copy_y ++) {
		memcpy(tex_data_it, data_it, width * bytes_per_pixel);
		tex_data_it += tex_width * bytes_per_pixel;
		if (flipped) {
			data_it -= width * bytes_per_pixel;
		} else {
			data_it += width * bytes_per_pixel;
		}
	}

	// Delete old texture
	glDeleteTextures(1, &texture);
	is_loaded = false;
	HppCheckGlErrors();

	// Create new texture
	Pixeldata pdata;
	pdata.buf = ByteV(tex_data, tex_data + (tex_width * tex_height * bytes_per_pixel));
	pdata.width = tex_width;
	pdata.height = tex_height;
	pdata.bytes_per_pixel = bytes_per_pixel;
	pdata.alpha = (gl_format == GL_ALPHA) || (gl_format == GL_RGBA) || (gl_format == GL_LUMINANCE_ALPHA);
	pdata.min_filter = min_filter;
	tryToUsePixeldata(pdata);

	delete[] tex_data;
}

inline uint8_t* Texture::resizeImagedata(uint8_t const* orig_data,
                                         size_t orig_width, size_t orig_height,
                                         size_t new_width, size_t new_height,
                                         size_t bytesperpixel)
{
	uint8_t* result = new uint8_t[new_width * new_height * bytesperpixel];

	uint8_t* result_it = result;
	for (size_t y = 0; y < new_height; y ++) {
		for (size_t x = 0; x < new_height; x ++) {
			for (size_t byte = 0; byte < bytesperpixel; byte ++) {
				float value;

				// Read value for byte
				float x_f = (float)x / new_width;
				float y_f = (float)y / new_height;
// TODO: Improve this crappy rescaling method!
ssize_t x2 = (ssize_t)::round(orig_width * x_f);
ssize_t y2 = (ssize_t)::round(orig_height * y_f);
if (x2 < 0) x2 = 0;
else if (x2 >= (ssize_t)orig_width) x2 = orig_width-1;
if (y2 < 0) y2 = 0;
else if (y2 >= (ssize_t)orig_height) y2 = orig_height-1;
value = orig_data[(y2 * orig_width + x2) * bytesperpixel + byte];

				// Round and store pixel
				ssize_t value_i = (ssize_t)::round(value);
				if (value_i < 0) {
					value_i = 0;
				} else if (value_i > 255) {
					value_i = 255;
				}
				*result_it = value_i;
				result_it ++;
			}
		}
	}

	return result;
}

inline void Texture::resizePixeldata(Pixeldata& pdata, size_t new_width, size_t new_height)
{
	uint8_t* new_buf = resizeImagedata(&(pdata.buf[0]), pdata.width, pdata.height, new_width, new_height, pdata.bytes_per_pixel);
	size_t new_buf_size = new_width * new_height * pdata.bytes_per_pixel;
	pdata.buf = ByteV(new_buf, new_buf + new_buf_size);
	pdata.width = new_width;
	pdata.height = new_height;
}

inline Texture::Pixeldata Texture::load(Path const& path, ByteV const& data, bool load_from_data, Pixelformat format, uint32_t flags)
{
	// Read flags
	bool force_no_alpha = flags & REMOVE_ALPHA;
	bool force_alpha = flags & FORCE_ALPHA;
	bool force_grayscale = flags & FORCE_GRAYSCALE;
	switch (format) {
	case RGB:
		force_no_alpha = true;
		break;
	case RGBA:
		force_alpha = true;
		break;
	case GRAYSCALE:
		force_grayscale = true;
		break;
	case GRAYSCALE_ALPHA:
		force_grayscale = true;
		force_alpha = true;
		break;
	case ALPHA:
		force_grayscale = false;
		force_alpha = false;
		force_no_alpha = false;
		break;
	case DEFAULT:
		break;
	}
	GLint min_filter = GL_LINEAR_MIPMAP_LINEAR;
	if (flags & NEAREST) {
		min_filter = GL_NEAREST;
	}
	if (flags & LINEAR) {
		min_filter = GL_LINEAR;
	}
	if (flags & NEAREST_MIPMAP_NEAREST) {
		min_filter = GL_NEAREST_MIPMAP_NEAREST;
	}
	if (flags & LINEAR_MIPMAP_NEAREST) {
		min_filter = GL_LINEAR_MIPMAP_NEAREST;
	}
	if (flags & NEAREST_MIPMAP_LINEAR) {
		min_filter = GL_NEAREST_MIPMAP_LINEAR;
	}
	if (flags & LINEAR_MIPMAP_LINEAR) {
		min_filter = GL_LINEAR_MIPMAP_LINEAR;
	}

	Image image;

	if (!load_from_data) {
		image = Image(path, format);
	} else {
		image = Image(data, format);
	}

	Pixeldata result;
	result.buf = image.getDataFlipped();
	result.width = image.getWidth();
	result.height = image.getHeight();
	result.bytes_per_pixel = image.getBytesPerPixel();
	result.alpha = image.hasAlpha() || (format == ALPHA);
	result.min_filter = min_filter;

	// Do possible resize
	if (flags & MINIMIZE_TO_64) {
		resizePixeldata(result, std::min(64, (int)result.width), std::min(64, (int)result.height));
	}

	return result;

}

inline void Texture::tryToUsePixeldata(Pixeldata const& pdata)
{
	if (Display::isThisRenderingThread()) {
		usePixeldata(pdata);
	} else {
		if (tempdata) {
			delete tempdata;
		}
		tempdata = new Pixeldata(pdata);

		is_loaded = false;
		width = pdata.width;
		height = pdata.height;
		Pixelformat final_format;
		#ifndef NDEBUG
		final_format = ALPHA;
		#endif
		if (pdata.bytes_per_pixel == 4) {
			HppAssert(pdata.alpha, "Fail!");
			final_format = RGBA;
		} else if (pdata.bytes_per_pixel == 3) {
			HppAssert(!pdata.alpha, "Fail!");
			final_format = RGB;
		} else if (pdata.bytes_per_pixel == 2) {
			HppAssert(pdata.alpha, "Fail!");
			final_format = GRAYSCALE_ALPHA;
		} else if (pdata.bytes_per_pixel == 1 && !pdata.alpha) {
			final_format = GRAYSCALE;
		} else if (pdata.bytes_per_pixel == 1 && pdata.alpha) {
			final_format = ALPHA;
		} else {
			throw Exception("Invalid format!");
		}
		format = final_format;
	}
}

inline void Texture::usePixeldata(Pixeldata pdata) const
{
	HppCheckForCorrectThread()

	// Clean possible old stuff
	if (is_loaded) {
		glDeleteTextures(1, &texture);
	}
	if (tempdata) {
		delete tempdata;
		tempdata = NULL;
	}

	// Generate a new GL texture
	GLuint new_texture;
	glGenTextures(1, &new_texture);
	glBindTexture(GL_TEXTURE_2D, new_texture);

	GLenum gl_format;
	Pixelformat final_format;
	#ifndef NDEBUG
	gl_format = 0;
	final_format = ALPHA;
	#endif
	if (pdata.bytes_per_pixel == 4) {
		HppAssert(pdata.alpha, "Fail!");
		gl_format = GL_RGBA;
		final_format = RGBA;
	} else if (pdata.bytes_per_pixel == 3) {
		HppAssert(!pdata.alpha, "Fail!");
		gl_format = GL_RGB;
		final_format = RGB;
	} else if (pdata.bytes_per_pixel == 2) {
		HppAssert(pdata.alpha, "Fail!");
		gl_format = GL_LUMINANCE_ALPHA;
		final_format = GRAYSCALE_ALPHA;
	} else if (pdata.bytes_per_pixel == 1 && !pdata.alpha) {
		gl_format = GL_LUMINANCE;
		final_format = GRAYSCALE;
	} else if (pdata.bytes_per_pixel == 1 && pdata.alpha) {
		gl_format = GL_ALPHA;
		final_format = ALPHA;
	} else {
		throw Exception("Invalid format!");
	}

	int valid_tex = 1;
	do {

		// If texture is not valid, then halve pixeldata
		if (!valid_tex) {
			if (pdata.width == pdata.height) {
				resizePixeldata(pdata, pdata.width / 2, pdata.height / 2);
			} else if (pdata.width > pdata.height) {
				resizePixeldata(pdata, pdata.width / 2, pdata.height);
			} else {
				resizePixeldata(pdata, pdata.width, pdata.height / 2);
			}
		}

		HppCheckGlErrors();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pdata.min_filter);
		HppCheckGlErrors();

		if (pdata.min_filter != GL_NEAREST && pdata.min_filter != GL_LINEAR &&
		    (GlSystem::openglVersionAtLeast(1, 4) || GlSystem::extensionExists("SGIS_generate_mipmap"))) {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			HppCheckGlErrors();
		}

		// Check if texture can be created
// TODO: On intel GPU, this enables GL_TEXTURE_2D. Is it normal?
		bool tex2d_enabled = glIsEnabled(GL_TEXTURE_2D);
		glTexImage2D(GL_PROXY_TEXTURE_2D, 0, gl_format,
			     pdata.width, pdata.height,
			     0, gl_format, GL_UNSIGNED_BYTE,
			     &pdata.buf[0]);
		if (!tex2d_enabled && glIsEnabled(GL_TEXTURE_2D)) {
			std::cerr << "WARNING: Unexpected enable of GL_TEXTURE_2D after calling glTexImage2D()!" << std::endl;
			glDisable(GL_TEXTURE_2D);
		}
		glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &valid_tex);

	} while (!valid_tex);

	GLint old_alignment = GlSystem::GetInteger(GL_UNPACK_ALIGNMENT);
	GLint new_alignment = old_alignment;
	GLint scanline = pdata.width * pdata.bytes_per_pixel;
	while (scanline % new_alignment != 0) {
		new_alignment /= 2;
	}
	if (new_alignment != old_alignment) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, new_alignment);
	}

// TODO: On intel GPU, this enables GL_TEXTURE_2D. Is it normal?
	bool tex2d_enabled = glIsEnabled(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, gl_format,
		     pdata.width, pdata.height,
		     0, gl_format, GL_UNSIGNED_BYTE,
		     &pdata.buf[0]);
	if (!tex2d_enabled && glIsEnabled(GL_TEXTURE_2D)) {
		std::cerr << "WARNING: Unexpected enable of GL_TEXTURE_2D after calling glTexImage2D()!" << std::endl;
		glDisable(GL_TEXTURE_2D);
	}
	if (new_alignment != old_alignment) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, old_alignment);
	}
	HppCheckGlErrors();

	// Update dataholder
	texture = new_texture;
	is_loaded = true;
	width = pdata.width;
	height = pdata.height;
	format = final_format;

}

inline void Texture::releaseGlTexture(void)
{
	if (Display::isThisRenderingThread()) {
		glDeleteTextures(1, &texture);
	} else {
		Texturemanager::addReleasableTexture(texture);
	}
}

}

#endif
