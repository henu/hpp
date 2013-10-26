#ifndef HPP_IMAGEEFFECTS_BLENDINGMODES_H
#define HPP_IMAGEEFFECTS_BLENDINGMODES_H

#include "../image.h"

namespace Hpp
{

namespace Imageeffects
{

// If threads is zero, then amount of cores is used
Image divide(Image const& bottom, Image const& top);

}

}

#endif
