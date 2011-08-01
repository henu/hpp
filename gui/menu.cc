#include "menu.h"

#include "renderer.h"

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
		// Start listening for clicks. If element outside of menu is
		// clicked, then menu will be closed.
		listenMouseClicks(Mousekey::FLAG_LEFT | Mousekey::FLAG_MIDDLE | Mousekey::FLAG_RIGHT);
	}
	return true;
}

void Menu::onMouseKeyDownOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	(void)mouse_key;
	// If this widget is child of this Menu, then do nothing
	if (widget && isMyChild(widget)) {
		return;
	}
	// Hide content and stop listening for clicks
	content.hide();
	listenMouseClicks(0);
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
