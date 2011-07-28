#include "font.h"

#include "cast.h"
#include "exception.h"

#include <iostream>
namespace Hpp
{

size_t Font::fonts_counter = 0;
FT_Library Font::ft_library;

void Font::initFreetype(void)
{
	FT_Error error = FT_Init_FreeType(&ft_library);
	if (error) {
		throw Exception("Unable to initialize Freetype! Freetype error code: " + ssizeToStr(error));
	}
}

void Font::deinitFreetype(void)
{
	FT_Error error = FT_Done_FreeType(ft_library);
	if (error) {
		std::cerr << "Unable to deinitialized Freetype! Freetype error code: " << error << std::endl;
	}
}

}
