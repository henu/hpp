#include "flip.h"

namespace Hpp
{

namespace Imageeffects
{

Image flipHorizontally(Image const& img)
{
	Image result(NULL, img.getWidth(), img.getHeight(), img.getFormat());

	for (size_t y = 0; y < img.getHeight(); ++ y) {
		for (size_t x = 0; x < img.getWidth(); ++ x) {
			Color pixel = img.getPixel(x, y);
			result.setPixel(img.getWidth() - x - 1, y, pixel);
		}
	}

	return result;
}

Image flipVertically(Image const& img)
{
	Image result(NULL, img.getWidth(), img.getHeight(), img.getFormat());

	for (size_t y = 0; y < img.getHeight(); ++ y) {
		for (size_t x = 0; x < img.getWidth(); ++ x) {
			Color pixel = img.getPixel(x, y);
			result.setPixel(x, img.getHeight() - y - 1, pixel);
		}
	}

	return result;
}

}

}
