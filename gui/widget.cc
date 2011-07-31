#include "widget.h"

#include "engine.h"

namespace Hpp
{

namespace Gui
{

Widget::~Widget(void)
{
	setParent(NULL);
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Widget* child = *children_it;
		child->setParent(NULL);
	}
	if (engine) {
		engine->unregisterWidget(this);
	}
}

Renderer const* Widget::getRenderer(void) const
{
	HppAssert(engine, "Unable to get Renderer, because Engine is not found!");
	return engine->getRenderer();
}

Renderer* Widget::getRenderer(void)
{
	HppAssert(engine, "Unable to get Renderer, because Engine is not found!");
	return engine->getRenderer();
}

void Widget::setEngine(Engine* engine)
{
	if (this->engine) {
		this->engine->unregisterWidget(this);
	}
	this->engine = engine;
	if (engine) {
		this->engine->registerWidget(this);
	}
	// Set also for children
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Widget* child = *children_it;
		child->setEngine(engine);
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
