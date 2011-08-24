#include "menuitem.h"

#include "menucontent.h"
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

uint32_t Menuitem::getMinWidth(void) const
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
		// Ask container to close
		Menucontent* menucontent = dynamic_cast< Menucontent* >(getParent());
		if (menucontent) {
			menucontent->close();
		}
	}
}

void Menuitem::doRendering(int32_t x_origin, int32_t y_origin)
{
	getRenderer()->renderMenuitem(x_origin, y_origin, this, label, isMouseOver());
}

}

}
