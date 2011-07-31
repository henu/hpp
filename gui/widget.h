#ifndef HPP_GUI_WIDGET_H
#define HPP_GUI_WIDGET_H

#include "../event.h"
#include "../assert.h"
#include "../exception.h"

#include <set>
#include <cstdlib>
#include <stdint.h>

namespace Hpp
{

namespace Gui
{

class Engine;
class Renderer;

class Widget
{

	friend class Engine;

public:

	inline Widget(void);
	virtual ~Widget(void);

	inline int32_t getPositionX(void) const { return x; }
	inline int32_t getPositionY(void) const { return y; }
	inline int32_t getWidth(void) const { return width; }
	inline int32_t getHeight(void) const { return height; }

	// Size getters
	inline virtual uint32_t getMinWidth(void) const { return 0; }
	inline virtual uint32_t getMaxWidth(void) const { return 0; }
	inline virtual uint32_t getMinHeight(uint32_t width) const { (void)width; return 0; }

protected:

	enum State { ENABLED, DISABLED, HIDDEN };

	inline void setPosition(int32_t x, int32_t y) { this->x = x; this->y = y; onPositionChange(); }
	inline void setSize(uint32_t width, uint32_t height) { this->width = width; this->height = height; onSizeChange(); }

	// Some events
	inline void markSizeChanged();

	inline Engine* getEngine(void) { return engine; }

	// Registers child
	inline void addChild(Widget* child) { child->setEngine(engine); child->setParent(this); }

	inline void setChildPosition(Widget* child, int32_t x, int32_t y);
	inline void setChildSize(Widget* child, uint32_t width, uint32_t height);

	// Some getters
	inline bool isMouseOver(void) const { return mouse_over; }
	Renderer const* getRenderer(void) const;
	Renderer* getRenderer(void);

	// Setters
	inline void setState(State state);

private:

	// Called by Engine and other Widgets
	void setEngine(Engine* engine);
	inline void setParent(Widget* parent);

	// Called by Engine
	inline void render(void);

	// Called by Engine and other Widgets. Second function returns Widget
	// that was under mouse or NULL if no Widget could be found.
	inline bool mouseOver(int32_t mouse_x, int32_t mouse_y) const;
	inline Widget const* mouseOverRecursive(int32_t mouse_x, int32_t mouse_y) const;
	inline Widget* mouseOverRecursive(int32_t mouse_x, int32_t mouse_y);

	// Called by Engine and other Widgets
	bool mouseEvent(Event const& event);

	// Called by Engine
	inline void setMouseOut(int32_t mouse_x, int32_t mouse_y) { HppAssert(mouse_over, "Unable to set mouse out, because mouse is not over!"); mouse_over = false; onMouseOut(mouse_x, mouse_y); }
	inline void setMouseOver(int32_t mouse_x, int32_t mouse_y) { HppAssert(!mouse_over, "Unable to set mouse over, because mouse is already over!"); mouse_over = true; onMouseOver(mouse_x, mouse_y); }

private:

	typedef std::set< Widget* > Children;

	Engine* engine;
	Widget* parent;
	Children children;

	// State, position and size
	State state;
	int32_t x, y;
	uint32_t width, height;
	bool mouse_over;

	inline void registerChild(Widget* child);
	inline void unregisterChild(Widget* child);

	// Real rendering function
	inline virtual void doRendering(void) { }

	// Real handler for events
	inline virtual void onMouseOver(int32_t mouse_x, int32_t mouse_y) { (void)mouse_x; (void)mouse_y; }
	inline virtual void onMouseOut(int32_t mouse_x, int32_t mouse_y) { (void)mouse_x; (void)mouse_y; }
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key) { (void)mouse_x; (void)mouse_y; (void)mouse_key; return false; }
	inline virtual bool onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key) { (void)mouse_x; (void)mouse_y; (void)mouse_key; return false; }
	inline virtual void onChildSizeChange(void) { }
	inline virtual void onPositionChange(void) { }
	inline virtual void onSizeChange(void) { }

};

inline Widget::Widget(void) :
engine(NULL),
parent(NULL),
state(ENABLED),
x(0), y(0),
width(0), height(0),
mouse_over(false)
{
}

inline void Widget::setParent(Widget* parent)
{
	if (this->parent) {
		this->parent->unregisterChild(this);
	}
	this->parent = parent;
	if (parent) {
		this->parent->registerChild(this);
	}
}

inline void Widget::markSizeChanged()
{
	if (parent) {
		parent->onChildSizeChange();
	}
}

inline void Widget::setChildPosition(Widget* child, int32_t x, int32_t y)
{
	HppAssert(children.find(child) != children.end(), "Unable to change child position, because it is really not our child!");
	child->setPosition(x, y);
}

inline void Widget::setChildSize(Widget* child, uint32_t width, uint32_t height)
{
	HppAssert(children.find(child) != children.end(), "Unable to change child size, because it is really not our child!");
	child->setSize(width, height);
}

inline void Widget::setState(State state)
{
	// If visibility will change, then inform parent about it
	if ((this->state != HIDDEN && state == HIDDEN) ||
	    (this->state == HIDDEN && state != HIDDEN)) {
		markSizeChanged();
	}
	this->state = state;
}

inline void Widget::render(void)
{
	if (state == HIDDEN) {
		return;
	}
	doRendering();
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Widget* child = *children_it;
		child->render();
	}
}

inline bool Widget::mouseOver(int32_t mouse_x, int32_t mouse_y) const
{
	if (state == ENABLED &&
	    mouse_x >= (int32_t)x && mouse_x < (int32_t)(x + width) &&
	    mouse_y >= (int32_t)y && mouse_y < (int32_t)(y + height)) {
		return true;
	}
	return false;
}

inline Widget const* Widget::mouseOverRecursive(int32_t mouse_x, int32_t mouse_y) const
{
	for (Children::const_iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Widget const* child = *children_it;
		Widget const* widget_under_mouse = child->mouseOverRecursive(mouse_x, mouse_y);
		if (widget_under_mouse) {
			return widget_under_mouse;
		}
	}
	if (mouseOver(mouse_x, mouse_y)) {
		return this;
	}
	return NULL;
}

inline Widget* Widget::mouseOverRecursive(int32_t mouse_x, int32_t mouse_y)
{
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Widget* child = *children_it;
		Widget* widget_under_mouse = child->mouseOverRecursive(mouse_x, mouse_y);
		if (widget_under_mouse) {
			return widget_under_mouse;
		}
	}
	if (mouseOver(mouse_x, mouse_y)) {
		return this;
	}
	return NULL;
}

inline void Widget::registerChild(Widget* child)
{
	HppAssert(children.find(child) == children.end(), "Child of Widget already registered!");
	children.insert(child);
}

inline void Widget::unregisterChild(Widget* child)
{
	HppAssert(children.find(child) != children.end(), "Child of Widget not found!");
	children.erase(child);
}

}

}

#endif
