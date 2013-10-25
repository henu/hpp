#include "edges.h"

#include "../cores.h"
#include "../thread.h"

namespace Hpp
{

namespace Imageeffects
{

struct DetectEdgesInfo
{
	Image const* src;
	Image* dest;
	size_t step;
	size_t offset;
};

void detectEdgesThread(void* dei_raw)
{
	DetectEdgesInfo* dei = reinterpret_cast< DetectEdgesInfo* >(dei_raw);

	int src_w = dei->src->getWidth();
	int src_h = dei->src->getHeight();
	int src_a = src_w * src_h;

	for (int pixel_ofs = dei->offset; pixel_ofs < src_a; pixel_ofs += dei->step) {

		int ofs_x = pixel_ofs % src_w;
		int ofs_y = pixel_ofs / src_w;

		// Get minimum and maximum of pixel
		// and neighbors at the position
		Color min(1, 1, 1);
		Color max(0, 0, 0);
		for (int y = ofs_y - 1; y <= ofs_y + 1; ++ y) {

			if (y < 0 || y >= src_h) continue;

			for (int x = ofs_x - 1; x <= ofs_x + 1; ++ x) {

				if (x < 0 || x >= src_w) continue;

				Color pixel = dei->src->getPixel(y * src_w + x);
				min.setRed(std::min(min.getRed(), pixel.getRed()));
				min.setGreen(std::min(min.getGreen(), pixel.getGreen()));
				min.setBlue(std::min(min.getBlue(), pixel.getBlue()));
				max.setRed(std::max(max.getRed(), pixel.getRed()));
				max.setGreen(std::max(max.getGreen(), pixel.getGreen()));
				max.setBlue(std::max(max.getBlue(), pixel.getBlue()));
			}
		}

		// Compute edge pixel
		Color pixel(max.getRed() - min.getRed(),
		            max.getBlue() - min.getBlue(),
		            max.getGreen() - min.getGreen());

		dei->dest->setPixel(pixel_ofs, pixel);
	}
}

Image detectEdges(Image const& img, size_t threads)
{
	if (threads == 0) {
		threads = getNumberOfCores();
	}

	size_t img_w = img.getWidth();
	size_t img_h = img.getHeight();
	uint8_t* pixels_random = new uint8_t[img_w * img_h * 3];
	Image result(pixels_random, img_w, img_h, RGB);
	delete[] pixels_random;

	if (threads == 1) {
		DetectEdgesInfo dei;
		dei.src = &img;
		dei.dest = &result;
		dei.step = 1;
		dei.offset = 0;
		detectEdgesThread(&dei);
	} else {
		std::vector< Thread > ts;
		std::vector< DetectEdgesInfo > deis(threads);
		// Start threads
		for (size_t t_id = 0; t_id < threads; ++ t_id) {
			DetectEdgesInfo* dei = &deis[t_id];
			dei->src = &img;
			dei->dest = &result;
			dei->step = threads;
			dei->offset = t_id;
			ts.push_back(Thread(detectEdgesThread, dei));
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
