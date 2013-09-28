#ifndef HPP_IMAGEEFFECTS_EDGES_H
#define HPP_IMAGEEFFECTS_EDGES_H

#include "../image.h"

namespace Hpp
{

namespace Imageeffects
{

Image detectEdges(Image const& img, size_t threads = 1);

}

}

#endif
