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
	menus.push_back(menu);
}

void Menubar::updatePosAndSize(Renderer* rend, int32_t x, int32_t y, uint32_t width)
{
	setPosition(x, y);
	setSize(width, rend->getMenubarHeight());
	// Update menu items too
	int32_t menu_x = x;
	for (Menus::iterator menus_it = menus.begin();
	     menus_it != menus.end();
	     menus_it ++) {
		Menu* menu = *menus_it;
		menu->updatePosAndSize(rend, menu_x, y);
		menu_x += menu->getLabelWidth(rend);
	}
}

void Menubar::render(Renderer* rend)
{
	// Background
	rend->renderMenubarBackground(this);
	// Menu items
	for (Menus::iterator menus_it = menus.begin();
	     menus_it != menus.end();
	     menus_it ++) {
		Menu* menu = *menus_it;
		menu->render(rend);
	}
}

}

}
