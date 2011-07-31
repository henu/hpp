#include "menuitem.h"

#include "renderer.h"

namespace Hpp
{

namespace Gui
{

Menuitem::Menuitem(void)
{
}

Menuitem::~Menuitem(void)
{
}

uint32_t Menuitem::getMaxWidth(void) const
{
	return getRenderer()->getMenuitemWidth(label);
}

uint32_t Menuitem::getMinHeight(uint32_t width) const
{
	(void)width;
	return getRenderer()->getMenuitemHeight();
}

void Menuitem::doRendering(void)
{
	getRenderer()->renderMenuitem(this, label, isMouseOver());
}

}

}
