#ifndef HPP_IMAGEEFFECTS_EDGES_H
#define HPP_IMAGEEFFECTS_EDGES_H

#include "../image.h"

namespace Hpp
{

namespace Imageeffects
{

// If threads is zero, then amount of cores is used
Image detectEdges(Image const& img, size_t threads = 0);

}

}

#endif
