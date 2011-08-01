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

void Widget::listenMouseClicks(Mousekey::KeycodeFlags flags)
{
	engine->registerMouseClickListener(this, flags);
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
	if (!mouse_over) {
		onMouseOver(event.x, event.y);
		mouse_over = true;
		engine->setMouseOver(this);
	}
	// Check if button was pressed
	if (event.type == Event::MOUSE_KEY_DOWN) {
		return onMouseKeyDown(event.x, event.y, event.mousekey);
	} else if (event.type == Event::MOUSE_KEY_UP) {
		return onMouseKeyUp(event.x, event.y, event.mousekey);
	}
	return false;
}

}

}
