#include "menucontent.h"

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

uint32_t Menucontent::getMaxWidth(void) const
{
	uint32_t max_width = 0;
	for (Menuitems::const_iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		Menuitembase const* item = *items_it;
		max_width = std::max(max_width, item->getMaxWidth());
	}
	return max_width;
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

void Menucontent::onSizeChange(void)
{
	int32_t x = getPositionX();
	int32_t y = getPositionY();
	uint32_t width = getWidth();
	for (Menuitems::iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		Menuitembase* item = *items_it;
		uint32_t item_height = item->getMinHeight(width);
		setChildPosition(item, x, y);
		setChildSize(item, width, item_height);
		y += item->getMinHeight(width);
	}
}

}

}
