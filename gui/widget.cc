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
	if (!engine) {
		return NULL;
	}
	return engine->getRenderer();
}

Renderer* Widget::getRenderer(void)
{
	if (!engine) {
		return NULL;
	}
	return engine->getRenderer();
}

void Widget::listenMouseClicks(Mousekey::KeycodeFlags flags)
{
	engine->registerMouseClickListener(this, flags);
}

void Widget::listenMouseReleases(Mousekey::KeycodeFlags flags)
{
	engine->registerMouseReleaseListener(this, flags);
}

void Widget::listenMouseMoves(bool listen)
{
	engine->registerMouseMoveListener(this, listen);
}

void Widget::listenKeyboard(void)
{
	engine->registerKeyboardListener(this);
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
	// Update environment
	updateEnvironment();
}

void Widget::render(int32_t x_origin, int32_t y_origin)
{
	if (state == HIDDEN) {
		return;
	}

	// Check if renderarea is limited
	if (renderarealimit) {
		engine->pushRenderarealimit(x_origin + renderarealimit_x, y_origin + renderarealimit_y, renderarealimit_width, renderarealimit_height);
	}

	x_origin += getPositionX();
	y_origin += getPositionY();
	doRendering(x_origin, y_origin);
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Widget* child = *children_it;
		child->render(x_origin, y_origin);
	}

	if (renderarealimit) {
		engine->popRenderarealimit();
	}
}

bool Widget::mouseEvent(Event const& event)
{
	// If hidden or disabled, then do nothing
	if (state != ENABLED) {
		return false;
	}
	int32_t mouse_x_relative = event.x - getAbsolutePositionX();
	int32_t mouse_y_relative = event.y - getAbsolutePositionY();
	onMouseOver(mouse_x_relative, mouse_y_relative);
	if (!mouse_over) {
		mouse_over = true;
		engine->setMouseOver(this);
	}
	// Check if button was pressed
	if (event.type == Event::MOUSE_KEY_DOWN) {
		return onMouseKeyDown(mouse_x_relative, mouse_y_relative, event.mousekey);
	} else if (event.type == Event::MOUSE_KEY_UP) {
		return onMouseKeyUp(mouse_x_relative, mouse_y_relative, event.mousekey);
	}
	return false;
}

}

}
