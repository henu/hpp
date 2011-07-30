#include "widget.h"

#include "engine.h"

namespace Hpp
{

namespace Gui
{

Widget::~Widget(void)
{
	setParent(NULL);
	if (!children.empty()) {
		throw Exception("There are still children at one of widgets when it is being destroyed!");
	}
	if (engine) {
		engine->setMouseOver(NULL);
	}
}

bool Widget::mouseEvent(Event const& event)
{
	// If hidden or disabled, then do nothing
	if (state != ENABLED) {
		return false;
	}
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Widget* child = *children_it;
		if (child->mouseEvent(event)) {
			return true;
		}
	}
	// Check if mouse has come over this widget
	if (mouseOver(event.x, event.y) && !mouse_over) {
		onMouseOver(event.x, event.y);
		mouse_over = true;
		engine->setMouseOver(this);
	}
	// Check if button was pressed
	if (event.MOUSE_KEY_DOWN) {
		if (onMouseKeyDown(event.x, event.y, event.mousekey)) {
			return true;
		}
	} else if (event.MOUSE_KEY_UP) {
		if (onMouseKeyUp(event.x, event.y, event.mousekey)) {
			return true;
		}
	}
	return false;
}

}

}
