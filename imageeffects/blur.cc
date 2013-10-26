#include "blur.h"

#include "../cores.h"
#include "../thread.h"
#include "../misc.h"
#include "../ivector2.h"

#include <vector>

namespace Hpp
{

namespace Imageeffects
{

typedef std::vector< float > Floats;

struct Filter
{
	IVector2 size;
	IVector2 begin;
	IVector2 end;
	Floats floats;
};

struct BlurInfo
{
	Image const* src;
	Image* dest;
	size_t step;
	size_t offset;
	Filter const* filter;
};

void blurThread(void* bi_raw)
{
	BlurInfo* bi = reinterpret_cast< BlurInfo* >(bi_raw);

	int src_w = bi->src->getWidth();
	int src_h = bi->src->getHeight();
	int src_a = src_w * src_h;

	for (int pixel_ofs = bi->offset; pixel_ofs < src_a; pixel_ofs += bi->step) {

		int ofs_x = pixel_ofs % src_w;
		int ofs_y = pixel_ofs / src_w;

		Color final_pixel(0, 0, 0, 0);
		float total_weight = 0;

		// Loop Y axis
		int y_begin = ofs_y + bi->filter->begin.y;
		int y_end = ofs_y + bi->filter->end.y;
		if (y_begin < 0) y_begin = 0;
		if (y_end >= src_h) y_end = src_h - 1;
		for (int y = y_begin; y <= y_end; ++ y) {
			size_t floats_ofs = (y - ofs_y - bi->filter->begin.y) * bi->filter->size.x;
			size_t src_ofs = y * src_w;

			// Loop in X axis
			int x_begin = ofs_x + bi->filter->begin.x;
			int x_end = ofs_x + bi->filter->end.x;
			if (x_begin < 0) x_begin = 0;
			if (x_end >= src_w) x_end = src_w - 1;
			floats_ofs += x_begin - ofs_x - bi->filter->begin.x;
			src_ofs += x_begin;
			for (int x = x_begin; x <= x_end; ++ x) {

				float weight = bi->filter->floats[floats_ofs];
				if (weight > 0) {
					final_pixel += bi->src->getPixel(src_ofs) * weight;
					total_weight += weight;
				}

				++ floats_ofs;
				++ src_ofs;
			}
		}
		if (total_weight > 0) {
			final_pixel /= total_weight;
		}

		bi->dest->setPixel(pixel_ofs, final_pixel);
	}
}

Image blur(Image const& img, Real radius, size_t threads)
{
	if (threads == 0) {
		threads = getNumberOfCores();
	}

	// Construct filter
	Filter filter;
	int radius_i = iCeil(radius);
	filter.begin.x = -radius_i;
	filter.begin.y = -radius_i;
	filter.end.x = radius_i;
	filter.end.y = radius_i;
	filter.size.x = filter.end.x - filter.begin.x + 1;
	filter.size.y = filter.end.y - filter.begin.y + 1;
	size_t area = filter.size.x * filter.size.y;
	filter.floats.reserve(area);
	for (int y = filter.begin.y; y <= filter.end.y; ++ y) {
		int y_to_2 = y*y;
		for (int x = filter.begin.x; x <= filter.end.x; ++ x) {
			int x_to_2 = x*x;
			float dst = sqrt(x_to_2 + y_to_2);
			if (dst <= radius - 1) {
				filter.floats.push_back(1);
			} else if (dst <= radius) {
				filter.floats.push_back(radius - dst);
			} else {
				filter.floats.push_back(0);
			}
		}
	}

	size_t img_w = img.getWidth();
	size_t img_h = img.getHeight();
	Image result(img_w, img_h, img.getFormat());

	if (threads == 1) {
		BlurInfo bi;
		bi.src = &img;
		bi.dest = &result;
		bi.step = 1;
		bi.offset = 0;
		bi.filter = &filter;
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
			bi->filter = &filter;
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

