#include "menubar.h"

#include "menu.h"
#include "renderer.h"

#include "../assert.h"

namespace Hpp
{

namespace Gui
{

Menubar::Menubar(void)
{
}

Menubar::~Menubar(void)
{
}

void Menubar::addMenu(Menu* menu)
{
	addChild(menu);
	menus.push_back(menu);
	updateSizeAndPositionOfMenus();
}

void Menubar::doRendering(int32_t x_origin, int32_t y_origin)
{
	// Background
	getRenderer()->renderMenubarBackground(x_origin, y_origin, this);
}

void Menubar::updateSizeAndPositionOfMenus(void)
{
	int32_t menu_x = 0;
	for (Menus::iterator menus_it = menus.begin();
	     menus_it != menus.end();
	     menus_it ++) {
		Menu* menu = *menus_it;
		setChildPosition(menu, menu_x, 0);
		uint32_t menu_width = menu->getMinWidth();
		uint32_t menu_height = menu->getMinHeight(0);
		setChildSize(menu, menu_width, menu_height);
		menu_x += menu_width;
	}
}

}

}
