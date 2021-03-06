#include "areawithmenubar.h"

#include "menu.h"
#include "renderer.h"

#include "../assert.h"

namespace Hpp
{

namespace Gui
{

AreaWithMenubar::AreaWithMenubar(void) :
content(NULL)
{
	// Enable expanding by default
	setHorizontalExpanding(1);
	setVerticalExpanding(1);
}

AreaWithMenubar::~AreaWithMenubar(void)
{
}

void AreaWithMenubar::addMenu(Menu* menu)
{
	addChild(menu);
	menus.push_back(menu);
	markToNeedReposition();
}

void AreaWithMenubar::setContent(Widget* widget)
{
	if (content) {
		removeChild(content);
	}
	content = widget;
	if (content) {
		addChild(content);
		moveWidgetBack(content);
	}
	markToNeedReposition();
}

uint32_t AreaWithMenubar::doGetMinWidth(void) const
{
	uint32_t result = 0;
	for (Menus::const_iterator menus_it = menus.begin();
	     menus_it != menus.end();
	     menus_it ++) {
		Menu* menu = *menus_it;
		result += menu->getMinWidth();
	}
	if (content) result = std::max(result, content->getMinWidth());
	return result;
}

uint32_t AreaWithMenubar::doGetMinHeight(uint32_t width) const
{
	uint32_t result = 0;
	Renderer const* rend = getRenderer();
	if (rend) result += rend->getMenubarHeight();
	if (content) result += content->getMinHeight(width);
	return result;
}

void AreaWithMenubar::doRendering(int32_t x_origin, int32_t y_origin)
{
	// Background
	getRenderer()->renderMenubarBackground(x_origin, y_origin, getWidth(), this);
}

void AreaWithMenubar::onChildRemoved(Widget* child)
{
	Menus::iterator menus_find = std::find(menus.begin(), menus.end(), child);
	if (menus_find != menus.end()) {
		menus.erase(menus_find);
		markToNeedReposition();
		return;
	}
	if (child == content) {
		content = NULL;
	}
}

void AreaWithMenubar::doRepositioning(void)
{
	int32_t menu_x = 0;
	for (Menus::iterator menus_it = menus.begin();
	     menus_it != menus.end();
	     menus_it ++) {
		Menu* menu = *menus_it;
		uint32_t menu_width = menu->getMinWidth();
		uint32_t menu_height = menu->getMinHeight(0);
		repositionChild(menu, menu_x, 0, menu_width, menu_height);
		menu_x += menu_width;
	}
	Renderer const* rend = getRenderer();
	if (content && rend) {
		uint32_t menu_height = rend->getMenubarHeight();
		repositionChild(content, 0, menu_height, getWidth(), getHeight() - menu_height);
	}
}

}

}
