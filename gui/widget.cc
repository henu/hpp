#include "widget.h"

#include "engine.h"

namespace Hpp
{

namespace Gui
{

Widget::~Widget(void)
{
	setParent(NULL);
	while (!children.empty()) {
		Widget* child = children.back();
		#ifndef NDEBUG
		size_t children_size = children.size();
		#endif
		child->setParent(NULL);
		#ifndef NDEBUG
		HppAssert(children.size() == children_size - 1, "No child was removed!");
		#endif
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

void Widget::stopListeningKeyboard(void)
{
	engine->registerKeyboardListener(NULL);
}

bool Widget::isListeningKeyboard(void) const
{
	return engine->amIListeningKeyboard(this);
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
// TODO: Is it necessary to call this to All Widgets?
markToNeedReposition();
}

void Widget::repositionIfNeeded(int32_t x, int32_t y, int32_t w, int32_t h)
{
	if (shrunken) {
		width = 0;
		height = 0;
		this->x = 0;
		this->y = 0;
		reposition_needed = false;
		return;
	}

	// Apply margin
	x += margin;
	y += margin;
	w -= margin * 2;
	h -= margin * 2;

	// Get real dimensions
	int32_t real_w, real_h;
	if (expanding_horiz == 0) real_w = getMinWidth() - margin*2;
	else real_w = w;
	if (expanding_vert == 0) real_h = getMinHeight(real_w) - margin*2;
	else real_h = h;

	// Calculate real position
	int32_t real_x, real_y;
	if (expanding_horiz > 0 || align == LEFT) real_x = x;
	else if (align == RIGHT) real_x = x + (w - real_w);
	else real_x = x + (w - real_w) / 2;
	if (expanding_vert > 0 || valign == TOP) real_y = y;
	else if (valign == BOTTOM) real_y = y + (h - real_h);
	else real_y = y + (h - real_h) / 2;

	// If position or size differs, then reposition is needed
	if (real_x != x || real_y != y || real_w != int32_t(width) || real_h != int32_t(height)) {
		reposition_needed = true;
	}

	// Reposition only if needed
	if (!reposition_needed) {
		return;
	}

	// Do actual setting of size and position
	width = real_w;
	height = real_h;
	this->x = real_x;
	this->y = real_y;

	// Do actual repositioning
	reposition_needed = false;
	doRepositioning();
}

void Widget::render(int32_t x_origin, int32_t y_origin)
{
	if (state == HIDDEN || !visible || shrunken) {
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
	if (state != ENABLED || !visible) {
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
