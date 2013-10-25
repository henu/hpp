#include "blur.h"

#include "../cores.h"
#include "../thread.h"
#include "../misc.h"

namespace Hpp
{

namespace Imageeffects
{

struct BlurInfo
{
	Image const* src;
	Image* dest;
	size_t step;
	size_t offset;
	Real radius;
};

void blurThread(void* bi_raw)
{
	BlurInfo* bi = reinterpret_cast< BlurInfo* >(bi_raw);

	int src_w = bi->src->getWidth();
	int src_h = bi->src->getHeight();
	int src_a = src_w * src_h;

	Real radius_to_2 = bi->radius * bi->radius;
	int radius_i_ceil = iCeil(bi->radius);

	for (int pixel_ofs = bi->offset; pixel_ofs < src_a; pixel_ofs += bi->step) {

		int ofs_x = pixel_ofs % src_w;
		int ofs_y = pixel_ofs / src_w;

		Color final_pixel(0, 0, 0, 0);
		size_t samples = 0;

		// Loop Y axis
		int y_begin = ofs_y - radius_i_ceil;
		int y_end = ofs_y + radius_i_ceil;
		if (y_begin < 0) y_begin = 0;
		if (y_end >= src_h) y_end = src_h - 1;
		for (int y = y_begin; y <= y_end; ++ y) {
			int y_diff = y - ofs_y;
			int y_diff_to_2 = y_diff * y_diff;

			// Loop in X axis
			int x_radius = iCeil(sqrt(radius_to_2 - y_diff * y_diff));
			int x_begin = ofs_x - x_radius;
			int x_end = ofs_x + x_radius;
			if (x_begin < 0) x_begin = 0;
			if (x_end >= src_w) x_end = src_w - 1;
			for (int x = x_begin; x <= x_end; ++ x) {
				int x_diff = x - ofs_x;

				if (x_diff*x_diff + y_diff_to_2 <= radius_to_2) {
// TODO: If pixel is at the edge, then do not use 100 % of it!
					final_pixel += bi->src->getPixel(x, y);
					++ samples;
				}
			}
		}
		if (samples > 0) {
			final_pixel /= samples;
		}

		bi->dest->setPixel(pixel_ofs, final_pixel);
	}
}

Image blur(Image const& img, Real radius, size_t threads)
{
	if (threads == 0) {
		threads = getNumberOfCores();
	}

	size_t img_w = img.getWidth();
	size_t img_h = img.getHeight();
	uint8_t* pixels_random = new uint8_t[img_w * img_h * 3];
	Image result(pixels_random, img_w, img_h, img.getFormat());
	delete[] pixels_random;

	if (threads == 1) {
		BlurInfo bi;
		bi.src = &img;
		bi.dest = &result;
		bi.step = 1;
		bi.offset = 0;
		bi.radius = radius;
		blurThread(&bi);
	} else {
		std::vector< Thread > ts;
		std::vector< BlurInfo > bis(threads);
		// Start threads
		for (size_t t_id = 0; t_id < threads; ++ t_id) {
			BlurInfo* bi = &bis[t_id];
			bi->src = &img;
			bi->dest = &result;
			bi->step = threads;
			bi->offset = t_id;
			bi->radius = radius;
			ts.push_back(Thread(blurThread, bi));
		}
		// Wait for threads to stop
		for (size_t t_id = 0; t_id < threads; ++ t_id) {
			ts[t_id].wait();
		}
	}
	return result;
}

}

}

