#ifndef HPP_IMAGEEFFECTS_BLUR_H
#define HPP_IMAGEEFFECTS_BLUR_H

#include "../image.h"

namespace Hpp
{

namespace Imageeffects
{

// If threads is zero, then amount of cores is used
Image blur(Image const& img, Hpp::Real radius, size_t threads = 0);

}

}

#endif
