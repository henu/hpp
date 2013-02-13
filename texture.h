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
// TODO: Support clamping to edges!
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

	// Function to bind/unbind texture. bind() can be
	// called multiple times. Every call of bind()
	// and unbind() leaves its texture unit active.
	inline void bind(void) const;
	inline void unbind(void) const;
	inline GLenum getBoundTextureunit(void) const;

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
		bool depth;
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

	// Binding stuff
	mutable bool bound;
	mutable GLenum bound_texture_unit;


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

	inline static Pixelformat getFormat(size_t bytes_per_pixel, bool alpha, bool depth);

};

inline Texture::Texture(void) :
is_loaded(false),
tempdata(NULL),
bound(false),
bound_texture_unit(0)
{
}

inline Texture::Texture(Path const& path, Pixelformat format, uint32_t flags) :
is_loaded(false),
tempdata(NULL),
bound(false),
bound_texture_unit(0)
{
	HppCheckGlErrorsIfCorrectThread();
	loadFromFile(path, format, flags);
	HppCheckGlErrorsIfCorrectThread();
}

inline Texture::Texture(ByteV const& data, Pixelformat format, uint32_t flags) :
is_loaded(false),
tempdata(NULL),
bound(false),
bound_texture_unit(0)
{
	HppCheckGlErrorsIfCorrectThread();
	loadFromData(data, format, flags);
	HppCheckGlErrorsIfCorrectThread();
}

inline Texture::~Texture(void)
{
	if (bound) {
		GlSystem::ActiveTexture(GL_TEXTURE0 + bound_texture_unit);
		glDisable(GL_TEXTURE_2D);
		HppCheckGlErrors();
	}
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
	uint8_t bytes_per_pixel = getBppOfPixelformat(format);
	bool alpha = pixelformatHasAlpha(format);
	bool depth = pixelformatHasDepth(format);

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
	pdata.depth = depth;
	pdata.min_filter = min_filter;

	tryToUsePixeldata(pdata);

}

inline void Texture::bind(void) const
{
	// If not yet bound, then find empty texture unit
	if (!bound) {
		bound_texture_unit = 0;
		while (true) {
			HppCheckGlErrors();
			GlSystem::ActiveTexture(GL_TEXTURE0 + bound_texture_unit);
			if (glGetError() != GL_NO_ERROR) {
				throw Exception("No free texture units!");
			}
			if (!glIsEnabled(GL_TEXTURE_2D)) {
				break;
			}
			++ bound_texture_unit;
		}
		bound = true;
		// Enable texture, etc.
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, getGlTexture());
		HppCheckGlErrors();
	} else {
		GlSystem::ActiveTexture(GL_TEXTURE0 + bound_texture_unit);
	}
}

inline void Texture::unbind(void) const
{
	if (!bound) {
		throw Exception("Unable to unbind texture, because it is not bound!");
	}
	GlSystem::ActiveTexture(GL_TEXTURE0 + bound_texture_unit);
	glDisable(GL_TEXTURE_2D);
	bound = false;
}

inline GLenum Texture::getBoundTextureunit(void) const
{
	if (!bound) {
		throw Exception("Unable get texture unit, because texture is not bound!");
	}
	return bound_texture_unit;
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
	GLenum gl_format = pixelformatToGlFormat(format);
	uint8_t bytes_per_pixel = getBppOfPixelformat(format);

	GLenum type = GL_UNSIGNED_BYTE;
	if (pixelformatHasDepth(format)) {
		type = GL_FLOAT;
	}

	// Get image data
	uint8_t* tex_data = new uint8_t[tex_width * tex_height * bytes_per_pixel];
	glGetTexImage(GL_TEXTURE_2D, 0, gl_format, type, tex_data);
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
	GLenum gl_format = pixelformatToGlFormat(format);
	uint8_t bytes_per_pixel = getBppOfPixelformat(format);

	GLenum type = GL_UNSIGNED_BYTE;
	if (pixelformatHasDepth(format)) {
		type = GL_FLOAT;
	}

	// Get image data
	uint8_t* tex_data = new uint8_t[tex_width * tex_height * bytes_per_pixel];
	glGetTexImage(GL_TEXTURE_2D, 0, gl_format, type, tex_data);
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
	pdata.alpha = pixelformatHasAlpha(format);
	pdata.depth = pixelformatHasDepth(format);
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
	result.depth = pixelformatHasDepth(format);
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
		format = getFormat(pdata.bytes_per_pixel, pdata.alpha, pdata.depth);
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
	HppCheckGlErrors();
	glBindTexture(GL_TEXTURE_2D, new_texture);
	HppCheckGlErrors();

	Pixelformat final_format = getFormat(pdata.bytes_per_pixel, pdata.alpha, pdata.depth);
	GLenum gl_format = pixelformatToGlFormat(final_format);
	GLint gl_internalformat = pixelformatToGlInternalFormat(final_format);

	GLenum type = GL_UNSIGNED_BYTE;
	if (pixelformatHasDepth(final_format)) {
		type = GL_FLOAT;
	}

	int valid_tex = 1;
	do {

		// If texture is not valid, then halve pixeldata
		if (!valid_tex) {
			if (pdata.width == pdata.height) {
				resizePixeldata(pdata, pdata.width / 2, pdata.height / 2);
				HppCheckGlErrors();
			} else if (pdata.width > pdata.height) {
				resizePixeldata(pdata, pdata.width / 2, pdata.height);
				HppCheckGlErrors();
			} else {
				resizePixeldata(pdata, pdata.width, pdata.height / 2);
				HppCheckGlErrors();
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
		glTexImage2D(GL_PROXY_TEXTURE_2D, 0, gl_internalformat,
			     pdata.width, pdata.height,
			     0, gl_format, type,
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
	glTexImage2D(GL_TEXTURE_2D, 0, gl_internalformat,
		     pdata.width, pdata.height,
		     0, gl_format, type,
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

inline Pixelformat Texture::getFormat(size_t bytes_per_pixel, bool alpha, bool depth)
{
	if (bytes_per_pixel == 4) {
		if (!depth) {
			HppAssert(alpha, "Alpha expected!");
			return RGBA;
		} else {
			return DEPTH32;
		}
	} else if (bytes_per_pixel == 3) {
		if (!depth) {
			HppAssert(!alpha, "Unexpected alpha!");
			return RGB;
		} else {
			return DEPTH24;
		}
	} else if (bytes_per_pixel == 2) {
		if (!depth) {
			HppAssert(alpha, "Alpha expected!");
			return GRAYSCALE_ALPHA;
		} else {
			return DEPTH16;
		}
	} else if (bytes_per_pixel == 1) {
		if (alpha) {
			return ALPHA;
		} else {
			return GRAYSCALE;
		}
	} else {
		throw Exception("Invalid format!");
	}
}

}

#endif
