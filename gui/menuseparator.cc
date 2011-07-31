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

uint32_t Menuseparator::getMaxWidth(void) const
{
	return getRenderer()->getMenuseparatorMinWidth();
}

uint32_t Menuseparator::getMinHeight(uint32_t width) const
{
	(void)width;
	return getRenderer()->getMenuseparatorHeight();
}

void Menuseparator::doRendering(void)
{
	getRenderer()->renderMenuseparator(this);
}

}

}
