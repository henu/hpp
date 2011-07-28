#ifndef HPP_FONT_H
#define HPP_FONT_H

#include "viewport.h"
#include "unicodestring.h"
#include "texture.h"
#include "color.h"
#include "path.h"
#include "vector2.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>

namespace Hpp
{

class Font
{

public:

	// ----------------------------------------
	// Public types
	// ----------------------------------------

	// Type for character
	struct Character
	{
		Texture* tex;
		Vector2 tex_pos;
		Vector2 tex_size;
		Vector2 tex_offset;
		Real adv;
	};


	// ----------------------------------------
	// Public functions
	// ----------------------------------------

	inline Font(uint32_t font_default_size = 0, Path const& path = Path::getUnknown());
	inline ~Font(void);

	inline void setDefaultHeight(uint32_t font_default_size);

	// Loads new source
	inline void loadMore(Path const& path);

	inline Real getStringWidth(UnicodeString const& str, Real font_size);

	// Position is measured inside viewport
	inline void renderString(UnicodeString const& str,
	                         Real font_size,
	                         Color const& color,
	                         Viewport const* viewport,
	                         Vector2 const& pos);

private:

	// ----------------------------------------
	// Private constants and types
	// ----------------------------------------

	static uint16_t const DEFAULT_TEXTURE_WIDTH = 512;

	// Size of border between fonts
	static uint32_t const BORDER = 4;

	// Container for font sources
	typedef std::vector< FT_Face > Sources;

	typedef std::vector< Texture* > Textures;

	// Container of all loaded characters.
	typedef std::map< UChr, Character > Characters;


	// ----------------------------------------
	// Data
	// ----------------------------------------

	// Sources of font and its default size
	Sources sources;
	uint32_t font_default_size;

	// Options
	uint16_t texture_width;

	// Textures that are being used as a storage for glyphs
	Texture* tex;
	Textures old_texs;
	// Texture using specs
	uint16_t tex_pos_x;
	uint16_t tex_pos_y;
	uint16_t tex_row_maxheight;

	// Loaded characters
	Characters chrs;

	// Static counter for Fonts and static Freetype handle.
	// Freetype handle is initialized when first Font is
	// created and deinitialized when last one is destroyed.
	static size_t fonts_counter;
	static FT_Library ft_library;


	// ----------------------------------------
	// Private functions
	// ----------------------------------------

	// Initializes/deinitializes freetype
	static void initFreetype(void);
	static void deinitFreetype(void);

	// Ensures that specific character exists. If character does not exist,
	// then it is created.
	inline void ensureCharacterExists(UChr chr);

	// Clears all textures and characters
	inline void clear(void);

};

inline Font::Font(uint32_t font_default_size, Path const& path) :
font_default_size(font_default_size),
texture_width(DEFAULT_TEXTURE_WIDTH),
tex(NULL)
{
	// Check if Freetype needs initialization
	HppCheckForCorrectThread();
	if (fonts_counter == 0) {
		initFreetype();
	}
	fonts_counter ++;

	// Load possible given path
	if (!path.isUnknown()) {
		loadMore(path);
	}
}

inline Font::~Font(void)
{
	clear();
	// Clear fonts
	for (Sources::iterator sources_it = sources.begin();
	     sources_it != sources.end();
	     sources_it ++) {
		FT_Face& source = *sources_it;
		FT_Done_Face(source);
	}
	// Check if Freetype needs deinitialization
	HppCheckForCorrectThread();
	fonts_counter --;
	if (fonts_counter == 0) {
		deinitFreetype();
	}
}

inline void Font::setDefaultHeight(uint32_t font_default_size)
{
	if (font_default_size == 0) {
		throw Exception("Font size must be greater than zero!");
	}

	this->font_default_size = font_default_size;
	clear();

	// Update font heights
	for (Sources::iterator sources_it = sources.begin();
	     sources_it != sources.end();
	     sources_it ++) {
		FT_Face& source = *sources_it;
		FT_Error error = FT_Set_Pixel_Sizes(source, 0, font_default_size);
		if (error) {
			FT_Done_Face(source);
			this->font_default_size = 0;
			throw Exception("Unable to update size of font! Freetype error code: " + ssizeToStr(error));
		}
	}
}

inline void Font::loadMore(Path const& path)
{
	FT_Face new_source;

	// Load the font
	FT_Error error = FT_New_Face(ft_library, path.toString().c_str(), 0, &new_source);
	if (error == FT_Err_Unknown_File_Format) {
		throw Exception("Unknown file format \"" + path.toString() + "\"");
	} else if (error) {
		throw Exception("Unable to open font \"" + path.toString() + "\"! Freetype error code: " + ssizeToStr(error));
	}

	// Set character size for font
	if (font_default_size > 0) {
		error = FT_Set_Pixel_Sizes(new_source, 0, font_default_size);
		if (error) {
			FT_Done_Face(new_source);
			throw Exception("Unable to set size for font \"" + path.toString() + "\"! Freetype error code: " + ssizeToStr(error));
		}
	}

	sources.push_back(new_source);
}

inline Real Font::getStringWidth(UnicodeString const& str, Real font_size)
{
	Real result = 0.0;
	for (UnicodeString::const_iterator str_it = str.begin();
	     str_it != str.end();
	     str_it ++) {
		UChr chr = *str_it;
		ensureCharacterExists(chr);
		result += chrs[chr].adv * (font_size / font_default_size);
	}
	return result;
}

inline void Font::renderString(UnicodeString const& str,
                               Real font_size,
                               Color const& color,
                               Viewport const* viewport,
                               Vector2 const& pos)
{
	// Ensure all characters exist
	for (UnicodeString::const_iterator str_it = str.begin();
	     str_it != str.end();
	     str_it ++) {
		ensureCharacterExists(*str_it);
	}

	Vector2 rendpos = pos;
	Real mult = font_size / font_default_size;

	// Prepare rendering
	HppCheckGlErrors();

	for (UnicodeString::const_iterator str_it = str.begin();
	     str_it != str.end();
	     str_it ++) {
		UChr chr_code = *str_it;
		Character& chr = chrs[chr_code];
		// Render
		HppCheckGlErrors();
		viewport->renderSprite(*chr.tex, rendpos + chr.tex_offset * mult, chr.tex_size * mult, chr.tex_pos / texture_width, chr.tex_size / texture_width, color);
		HppCheckGlErrors();
		rendpos.x += chr.adv * mult;
	}
}

inline void Font::ensureCharacterExists(UChr chr)
{

	if (chrs.find(chr) != chrs.end()) {
		return;
	}

	// Ensure texture is available.
	if (!tex) {
		tex = new Texture;
		tex->createNew(texture_width, texture_width, ALPHA, Texture::LINEAR);
		tex_pos_x = 0;
		tex_pos_y = 0;
		tex_row_maxheight = 0;
	}

	// Try to find glyph from sources
	FT_UInt glyph_index = 0;
	Sources::iterator sources_it = sources.begin();
	FT_Face source;
	while (sources_it != sources.end()) {
		source = *sources_it;
		glyph_index = FT_Get_Char_Index(source, chr);
		if (glyph_index > 0) {
			break;
		}
	}
	// If glyph was not found, then select first font. The
	// missings replacement image is then got from it.
// TODO: Ensure this really gets replacement image!
	if (glyph_index == 0) {
		if (sources.empty()) {
			throw Exception("No font sources defined!");
		}
		source = sources[0];
	}

	FT_Error error = FT_Load_Glyph(source, glyph_index, FT_LOAD_RENDER);
	if (error) {
		throw Exception("Unable to load glyph image! Freetype error code: " + sizeToStr(error));
	}
	HppAssert(source->glyph->format == FT_GLYPH_FORMAT_BITMAP, "Invalid freetype format!");

	// Get glyph properties
	size_t glyph_width = source->glyph->bitmap.width;
	size_t glyph_height = source->glyph->bitmap.rows;
	unsigned char* glyph_data = source->glyph->bitmap.buffer;

	// Try to fit glyph to current texure.
	if (tex_pos_x + BORDER + glyph_width > tex->getWidth()) {
		tex_pos_y += tex_row_maxheight + BORDER;
		tex_pos_x = 0;
		tex_row_maxheight = 0;
	}
	if (tex_pos_y + BORDER + glyph_height > tex->getHeight()) {
		HppAssert(glyph_width + BORDER <= tex->getWidth(), "Texture does not have enough width to fit glyph into it!");
		HppAssert(glyph_height + BORDER <= tex->getHeight(), "Texture does not have enough height to fit glyph into it!");
		// Put old texture aside
		old_texs.push_back(tex);
		// Create new empty texture
		tex = new Texture;
		tex->createNew(texture_width, texture_width, ALPHA, Texture::LINEAR);
		tex_pos_x = 0;
		tex_pos_y = 0;
		tex_row_maxheight = 0;
	}
	tex->addPixeldata(glyph_width, glyph_height,
	                  tex_pos_x, tex_pos_y,
	                  glyph_data, true);

	// Create new Character from glyph data.
	Character new_chr;
	new_chr.tex = tex;
	new_chr.tex_pos = Vector2(tex_pos_x, tex_pos_y);
	new_chr.tex_size = Vector2(glyph_width, glyph_height);
// TODO: Get Baseline from somewhere else!
Real const BASELINE = font_default_size * 0.15;
	new_chr.tex_offset = Vector2(source->glyph->bitmap_left, BASELINE + source->glyph->bitmap_top - glyph_height);
	new_chr.adv = source->glyph->advance.x / 64.0;
	chrs[chr] = new_chr;

	// Update texture position and max row height.
	tex_pos_x += glyph_width + BORDER;
	if (tex_row_maxheight < glyph_height) {
		tex_row_maxheight = glyph_height;
	}

	HppAssert(tex->getFormat() == ALPHA, "Texture has invalid format!");
}

inline void Font::clear(void)
{
	// Delete textures
	delete tex;
	for (Textures::iterator old_texs_it = old_texs.begin();
	     old_texs_it != old_texs.end();
	     old_texs_it ++) {
		delete *old_texs_it;
	}
	old_texs.clear();
	// Mark texture uninitialized
	tex = NULL;
	tex_pos_x = 0;
	tex_pos_y = 0;
	tex_row_maxheight = 0;
	// Clear characters
	chrs.clear();

}

}

#endif
