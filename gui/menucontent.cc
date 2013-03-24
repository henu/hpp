#include "menucontent.h"

#include "menu.h"

namespace Hpp
{

namespace Gui
{

Menucontent::Menucontent(void)
{
}

Menucontent::~Menucontent(void)
{
}

void Menucontent::hide(void)
{
	setState(HIDDEN);
}

void Menucontent::reveal(void)
{
	setState(ENABLED);
}

void Menucontent::close(void)
{
	Menu* parent = dynamic_cast< Menu* >(getParent());
	parent->close();
}

uint32_t Menucontent::getMinWidth(void) const
{
	uint32_t min_width = 0;
	for (Menuitems::const_iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		Menuitembase const* item = *items_it;
		min_width = std::max(min_width, item->getMinWidth());
	}
	return min_width;
}

uint32_t Menucontent::getMinHeight(uint32_t width) const
{
	uint32_t min_height = 0;
	for (Menuitems::const_iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		Menuitembase const* item = *items_it;
		min_height += item->getMinHeight(width);
	}
	return min_height;
}

void Menucontent::doRepositioning(void)
{
	int32_t y = 0;
	uint32_t width = getWidth();
	for (Menuitems::iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		Menuitembase* item = *items_it;
		uint32_t item_height = item->getMinHeight(width);
		repositionChild(item, 0, y, width, item_height);
		y += item->getMinHeight(width);
	}
}

}

}
