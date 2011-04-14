#include "image_draw.h"

namespace Hpp
{

// TODO: Make antialiasing and line width!
void drawLine(Image* img, Vector2 const& begin, Vector2 const& end, Color const& color)
{
	ssize_t w = img->getWidth();
	ssize_t h = img->getHeight();

	IVector2 b(round(begin.x), round(begin.y));
	IVector2 e(round(end.x), round(end.y));

	if (abs(b.x - e.x) > abs(b.y - e.y)) {
		if (b.x > e.x) {
			std::swap(b, e);
		}
		ssize_t b_x = b.x;
		ssize_t e_x = e.x;
		if (e_x < 0 || b_x >= w) return;
		if (b_x < 0) b_x = 0;
		if (e_x >= w) e_x = w - 1;

		for (ssize_t x = b_x; x <= e_x; x ++) {
			float y_r = (x - b.x) / (float)(e.x - b.x);
			ssize_t y = round((e.y - b.y) * y_r) + b.y;
			if (y >= 0 && y < h) {
				img->addPixel(IVector2(x, y), color);
			}
		}
	} else {
		if (b.y > e.y) {
			std::swap(b, e);
		}
		ssize_t b_y = b.y;
		ssize_t e_y = e.y;
		if (e_y < 0 || b_y >= h) return;
		if (b_y < 0) b_y = 0;
		if (e_y >= h) e_y = h - 1;

		for (ssize_t y = b_y; y <= e_y; y ++) {
			float x_r = (y - b.y) / (float)(e.y - b.y);
			ssize_t x = round((e.x - b.x) * x_r) + b.x;
			if (x >= 0 && x < w) {
				img->addPixel(IVector2(x, y), color);
			}
		}
	}

}

}
