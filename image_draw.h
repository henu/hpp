#ifndef HPP_IMAGE_DRAW_H
#define HPP_IMAGE_DRAW_H

#include "color.h"
#include "vector2.h"
#include "image.h"

namespace Hpp
{

void drawLine(Image* img, Vector2 const& begin, Vector2 const& end, Color const& color);

}

#endif
