#include "menuseparator.h"

#include "renderer.h"

namespace Hpp
{

namespace Gui
{

Menuseparator::Menuseparator(void)
{
}

Menuseparator::~Menuseparator(void)
{
}

uint32_t Menuseparator::getMinWidth(void) const
{
	return getRenderer()->getMenuseparatorMinWidth();
}

uint32_t Menuseparator::getMinHeight(uint32_t width) const
{
	(void)width;
	return getRenderer()->getMenuseparatorHeight();
}

void Menuseparator::doRendering(int32_t x_origin, int32_t y_origin)
{
	getRenderer()->renderMenuseparator(x_origin, y_origin, this);
}

}

}
