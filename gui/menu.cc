#include "menu.h"

#include "renderer.h"

#include <iostream>
namespace Hpp
{

namespace Gui
{

Menu::Menu(void)
{
	content.hide();
	addChild(&content);
}

Menu::~Menu(void)
{
}

void Menu::updatePosAndSize(Renderer* rend, int32_t x, int32_t y)
{
	setPosition(x, y);
	setSize(rend->getMenuLabelWidth(label), rend->getMenubarHeight());
}

uint32_t Menu::getLabelWidth(Renderer* rend)
{
	return rend->getMenuLabelWidth(label);
}

bool Menu::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	if (mouse_key == Mousekey::LEFT) {
		content.reveal();
	}
	return true;
}

void Menu::onChildSizeChange(void)
{
	setChildPosition(&content, getPositionX(), getPositionY() + getHeight());
	uint32_t content_width = content.getMaxWidth();
	uint32_t content_height = content.getMinHeight(content_width);
	setChildSize(&content, content_width, content_height);
}

void Menu::doRendering(void)
{
	// Label
	getRenderer()->renderMenuLabel(this, label, isMouseOver());
}

}

}
