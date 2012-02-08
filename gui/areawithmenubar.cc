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
}

AreaWithMenubar::~AreaWithMenubar(void)
{
	HppAssert(false, "Removing of child Widgets not implemented yet!");
}

void AreaWithMenubar::addMenu(Menu* menu)
{
	addChild(menu);
	menus.push_back(menu);
	markSizeChanged();
}

void AreaWithMenubar::setContent(Containerwidget* widget)
{
	if (content) {
		HppAssert(false, "Removing of child Widgets not implemented yet!");
	}
	content = widget;
	if (content) {
		addChild(content);
	}
	markSizeChanged();
}

uint32_t AreaWithMenubar::getMinWidth(void) const
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

uint32_t AreaWithMenubar::getMinHeight(uint32_t width) const
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

void AreaWithMenubar::updateWidgetSizesAndPositions(void)
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
	Renderer const* rend = getRenderer();
	if (content && rend) {
		uint32_t menu_height = rend->getMenubarHeight();
		positionWidget(content, 0, menu_height, getWidth(), getHeight() - menu_height);
	}
}

}

}
