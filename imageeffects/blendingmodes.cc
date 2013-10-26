#include "blendingmodes.h"

namespace Hpp
{

namespace Imageeffects
{

float doDivision(float dividend, float divisor)
{
	if (divisor == 0) {
		return 1;
	} else {
		return std::min< float >(dividend / divisor, 1);
	}
}

Image divide(Image const& bottom, Image const& top)
{
	// Ensure both images have same dimensions
	size_t w = bottom.getWidth();
	size_t h = bottom.getHeight();
	size_t area = w * h;
	if (w != top.getWidth() ||
	    h != top.getHeight()) {
		throw Exception("Divided images must have same dimensions!");
	}

	// Decide pixelformat of result
	Pixelformat bottom_format = bottom.getFormat();
	Pixelformat top_format = top.getFormat();
	Pixelformat final_format;
	bool top_format_has_alpha = pixelformatHasAlpha(top_format);
	// If format has RGB
	if (bottom_format == RGBA ||
	    bottom_format == RGB ||
	    top_format == RGBA ||
	    top_format == RGB) {
		if (pixelformatHasAlpha(bottom_format)) {
			final_format = RGBA;
		} else {
			final_format = RGB;
		}
	}
	// If format has grayscale
	else if (bottom_format == GRAYSCALE_ALPHA ||
	         bottom_format == GRAYSCALE ||
	         top_format == GRAYSCALE_ALPHA ||
	         top_format == GRAYSCALE) {
		if (pixelformatHasAlpha(bottom_format)) {
			final_format = GRAYSCALE_ALPHA;
		} else {
			final_format = GRAYSCALE;
		}
	} else if (bottom_format == ALPHA) {
		final_format = ALPHA;
	} else {
		throw Hpp::Exception("Invalid format!");
	}

	Image result(w, h, final_format);

	for (size_t ofs = 0; ofs < area; ++ ofs) {
		Color c1 = bottom.getPixel(ofs);
		Color c2 = top.getPixel(ofs);
		Color p;
		switch (final_format) {
		case RGBA:
			p = Color(doDivision(c1.getRed(), c2.getRed()),
			          doDivision(c1.getGreen(), c2.getGreen()),
			          doDivision(c1.getBlue(), c2.getBlue()),
			          c1.getAlpha());
			break;
		case RGB:
			p = Color(doDivision(c1.getRed(), c2.getRed()),
			          doDivision(c1.getGreen(), c2.getGreen()),
			          doDivision(c1.getBlue(), c2.getBlue()));
			break;
		case GRAYSCALE_ALPHA:
			p = Color(doDivision(c1.getValue(), c2.getValue()),
			          c1.getAlpha());
			break;
		case GRAYSCALE:
			p = Color(doDivision(c1.getValue(), c2.getValue()),
			          GRAYSCALE);
			break;
		case ALPHA:
			p = Color(doDivision(c1.getValue(), c2.getValue()),
			          ALPHA);
			break;
		default:
			HppAssert(false, "Fail!");
		}
		if (top_format_has_alpha) {
			float alpha = c2.getAlpha();
			p = c1 * (1 - alpha) + p * alpha;
		}
		result.setPixel(ofs, p);
	}
	return result;
}

}

}
