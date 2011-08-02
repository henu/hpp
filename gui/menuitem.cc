#include "menuitem.h"

#include "renderer.h"

namespace Hpp
{

namespace Gui
{

Menuitem::Menuitem(void) :
callback(NULL)
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

bool Menuitem::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	// If user starts clicking with left key, then
	// start listening where he/she releases mouse
	if (mouse_key == Mousekey::LEFT) {
		listenMouseReleases(Mousekey::FLAG_LEFT);
	}
	return true;
}

void Menuitem::onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	if (mouse_key == Mousekey::LEFT) {
		listenMouseReleases(0);
		if (widget == this) {
			// Do callback if it has been set
			if (callback) {
				callback(widget, callback_data);
			}
		}
	}
}

void Menuitem::doRendering(void)
{
	getRenderer()->renderMenuitem(this, label, isMouseOver());
}

}

}
