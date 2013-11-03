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

uint32_t Menu::doGetMinWidth(void) const
{
	Gui::Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getMenuLabelWidth(label);
}

uint32_t Menu::doGetMinHeight(uint32_t width) const
{
	(void)width;
	Gui::Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getMenubarHeight();
}

void Menu::close(void)
{
	content.hide();
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

void Menu::doRepositioning(void)
{
	uint32_t content_width = content.doGetMinWidth();
	uint32_t content_height = content.doGetMinHeight(content_width);
	repositionChild(&content, 0, getHeight(), content_width, content_height);
}

void Menu::doRendering(int32_t x_origin, int32_t y_origin)
{
	// Label
	getRenderer()->renderMenuLabel(x_origin, y_origin, this, label, isMouseOver());
}

}

}
