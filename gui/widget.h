#ifndef HPP_GUI_WIDGET_H
#define HPP_GUI_WIDGET_H

#include "eventlistener.h"
#include "guievent.h"
#include "../event.h"
#include "../unicode.h"
#include "../assert.h"
#include "../exception.h"

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <algorithm>
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

	enum Alignment { LEFT, RIGHT, TOP, BOTTOM, CENTER };

	inline Widget(void);
	virtual ~Widget(void);

	inline int32_t getPositionX(void) const { return x; }
	inline int32_t getPositionY(void) const { return y; }
	inline int32_t getAbsolutePositionX(void) const;
	inline int32_t getAbsolutePositionY(void) const;
	inline uint32_t getWidth(void) const { return width; }
	inline uint32_t getHeight(void) const { return height; }

	// Size getters
// TODO: Make these abstract, or better, make them be used through doGetMinWidth, like in my java gui library!
	inline virtual uint32_t getMinWidth(void) const { return 0; }
	inline virtual uint32_t getMinHeight(uint32_t width) const { (void)width; return 0; }

	// Alignment
	inline Alignment getHorizontalAlignment(void) const { return align; }
	inline Alignment getVerticalAlignment(void) const { return valign; }
	inline void setHorizontalAlignment(Alignment align) { this->align = align; markToNeedReposition(); }
	inline void setVerticalAlignment(Alignment align) { valign = align; markToNeedReposition(); }

	// Expanding
	inline uint8_t getHorizontalExpanding(void) const { return expanding_horiz; }
	inline uint8_t getVerticalExpanding(void) const { return expanding_vert; }
	inline void setHorizontalExpanding(uint8_t expanding) { expanding_horiz = expanding; markToNeedReposition(); }
	inline void setVerticalExpanding(uint8_t expanding) { expanding_vert = expanding; markToNeedReposition(); }

	inline void setEventlistener(Eventlistener* eventlistener) { this->eventlistener = eventlistener; }

private:

	// Called by Engine and other Widgets
	void setEngine(Engine* engine);
	inline void setParent(Widget* parent);

	// Repositions this Widget and all of its children, if needed. This may
	// ONLY be called by Widget itself through repositionChild or by Engine!
	void repositionIfNeeded(int32_t x, int32_t y, uint32_t w, uint32_t h);

	// Called by Engine and parent Widgets. Origin means absolute position
	// of parent, or topleft corner of the total area, if called by Engine.
	void render(int32_t x_origin, int32_t y_origin);

	// Called by Engine and other Widgets. Second function returns Widget
	// that was under mouse or NULL if no Widget could be found. Mouse
	// coordinates are in absolute coordinate system, and origin tells the
	// absolute coordinate of parent Widget.
	inline bool mouseOver(int32_t x_origin, int y_origin, int32_t mouse_x, int32_t mouse_y) const;
	inline Widget const* mouseOverRecursive(int32_t x_origin, int y_origin, int32_t mouse_x, int32_t mouse_y) const;
	inline Widget* mouseOverRecursive(int32_t x_origin, int y_origin, int32_t mouse_x, int32_t mouse_y);

	// Called by Engine and other Widgets
	bool mouseEvent(Event const& event);

	// Called by Engine
	inline void setMouseOut(int32_t mouse_x, int32_t mouse_y) { HppAssert(mouse_over, "Unable to set mouse out, because mouse is not over!"); mouse_over = false; onMouseOut(mouse_x, mouse_y); }
	inline void setMouseOver(int32_t mouse_x, int32_t mouse_y) { HppAssert(!mouse_over, "Unable to set mouse over, because mouse is already over!"); mouse_over = true; onMouseOver(mouse_x, mouse_y); }

protected:

	enum State { ENABLED, DISABLED, HIDDEN };

	// This functions is called, when content of child changes,
	// so that it also might change its size.
	inline void markToNeedReposition(void);

	inline Engine* getEngine(void) { return engine; }
	inline Widget* getParent(void) { return parent; }

	// Adds/removes child
	inline void addChild(Widget* child) { child->setEngine(engine); child->setParent(this); }
	inline void removeChild(Widget* child) { child->setEngine(NULL); child->setParent(NULL); }

	// Functions to handle order of children in child stack
	inline void moveWidgetBack(Widget* child);
	inline void moveWidgetFront(Widget* child);

	inline void repositionChild(Widget* child, int32_t x, int32_t y, uint32_t width, uint32_t height);

	inline void setChildState(Widget* child, State state);
	inline void setChildRenderarealimit(Widget* child, int32_t x, int32_t y, uint32_t width, uint32_t height);
	inline void removeChildRenderarealimit(Widget* child);

	// Some getters
	inline bool isMouseOver(void) const { return mouse_over; }
	Renderer const* getRenderer(void) const;
	Renderer* getRenderer(void);
	inline bool isMyChild(Widget const* widget) const;

	// Setters
	inline void setState(State state);

	// Listens for mouse clicks, and moves
	void listenMouseClicks(Mousekey::KeycodeFlags flags);
	void listenMouseReleases(Mousekey::KeycodeFlags flags);
	void listenMouseMoves(bool listen = true);
	void listenKeyboard(void);
	void stopListeningKeyboard(void);

	// Query functions for listenings
	bool isListeningKeyboard(void) const;

	inline void fireEvent(void);
	inline void fireEvent(int action);

private:

	typedef std::vector< Widget* > Children;

	Engine* engine;
	Widget* parent;
	Children children;

	// State, position and size
	State state;
	int32_t x, y;
	uint32_t width, height;
	bool mouse_over;

	bool reposition_needed;

	Alignment align, valign;
	uint8_t expanding_horiz, expanding_vert;

	Eventlistener* eventlistener;

	// Renderarea limitation. This is measured in parent's coordinates.
	bool renderarealimit;
	int32_t renderarealimit_x;
	int32_t renderarealimit_y;
	uint32_t renderarealimit_width;
	uint32_t renderarealimit_height;

	inline void registerChild(Widget* child);
	inline void unregisterChild(Widget* child);

	// Set/remove vieware limitation. These are called by
	// setChildRenderarealimit() and removeChildRenderarealimit().
	inline void setRenderarealimit(int32_t x, int32_t y, uint32_t width, uint32_t height);
	inline void removeRenderarealimit(void);

	inline bool positionOutsideRenderarealimit(int32_t x_origin, int32_t y_origin, int32_t x_abs, int32_t y_abs) const;

	// Real rendering function
	inline virtual void doRepositioning(void) { }
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin) { (void)x_origin; (void)y_origin; }

	// Real handler for events
	inline virtual void onMouseOver(int32_t mouse_x, int32_t mouse_y) { (void)mouse_x; (void)mouse_y; }
	inline virtual void onMouseOut(int32_t mouse_x, int32_t mouse_y) { (void)mouse_x; (void)mouse_y; }
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key) { (void)mouse_x; (void)mouse_y; (void)mouse_key; return false; }
	inline virtual bool onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key) { (void)mouse_x; (void)mouse_y; (void)mouse_key; return false; }
	inline virtual void onMouseKeyDownOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key) { (void)widget; (void)mouse_x; (void)mouse_y; (void)mouse_key; }
	inline virtual void onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key) { (void)widget; (void)mouse_x; (void)mouse_y; (void)mouse_key; }
	inline virtual void onMouseMove(int32_t mouse_x, int32_t mouse_y) { (void)mouse_x; (void)mouse_y; }
	inline virtual void onKeyDown(Key::Keycode keycode, UChr uchr) { (void)keycode; (void)uchr; }
	inline virtual void onChildSizeChange(void) { }
	inline virtual void onChildRemoved(Widget* child) { (void)child; }
	inline virtual void onPositionChange(void) { }
	inline virtual void onKeyboardListeningStop(void) { }

};

inline Widget::Widget(void) :
engine(NULL),
parent(NULL),
state(ENABLED),
x(0), y(0),
width(0), height(0),
mouse_over(false),
reposition_needed(true),
align(CENTER),
valign(CENTER),
expanding_horiz(0),
expanding_vert(0),
eventlistener(NULL),
renderarealimit(false)
{
}

inline int32_t Widget::getAbsolutePositionX(void) const
{
	if (!parent) return x;
	return x + parent->getAbsolutePositionX();
}

inline int32_t Widget::getAbsolutePositionY(void) const
{
	if (!parent) return y;
	return y + parent->getAbsolutePositionY();
}

inline void Widget::setParent(Widget* parent)
{
	if (this->parent) {
		this->parent->unregisterChild(this);
		this->parent->markToNeedReposition();
	}
	this->parent = parent;
	if (parent) {
		this->parent->registerChild(this);
	}
	markToNeedReposition();
}

inline void Widget::markToNeedReposition(void)
{
	reposition_needed = true;
	if (parent) {
		parent->markToNeedReposition();
	}
}

inline void Widget::moveWidgetBack(Widget* child)
{
	Children::reverse_iterator children_rfind = std::find(children.rbegin(), children.rend(), child);
	if (children_rfind == children.rend()) {
		throw Exception("Unable to move widget back, because it is not my child!");
	}
	for (Children::reverse_iterator children_rit = children_rfind;
	     children_rit != children.rend() - 1;
	     children_rit ++) {
		Widget* prev_child = *(children_rit + 1);
		*children_rit = prev_child;
	}
	children.front() = child;
}

inline void Widget::moveWidgetFront(Widget* child)
{
	Children::iterator children_find = std::find(children.begin(), children.end(), child);
	if (children_find == children.end()) {
		throw Exception("Unable to move widget front, because it is not my child!");
	}
	for (Children::iterator children_it = children_find;
	     children_it != children.end() - 1;
	     children_it ++) {
		Widget* next_child = *(children_it + 1);
		*children_it = next_child;
	}
	children.back() = child;
}

inline void Widget::repositionChild(Widget* child, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	HppAssert(std::find(children.begin(), children.end(), child) != children.end(), "This is not my child!");
	child->repositionIfNeeded(x, y, width, height);
}

inline void Widget::setChildState(Widget* child, State state)
{
	HppAssert(std::find(children.begin(), children.end(), child) != children.end(), "Unable to set child state, because it is really not our child!");
	child->setState(state);
}

inline void Widget::setChildRenderarealimit(Widget* child, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	HppAssert(std::find(children.begin(), children.end(), child) != children.end(), "Unable to limit child renderarea, because it is really not our child!");
	child->setRenderarealimit(x, y, width, height);
}

inline void Widget::removeChildRenderarealimit(Widget* child)
{
	HppAssert(std::find(children.begin(), children.end(), child) != children.end(), "Unable to limit child renderarea, because it is really not our child!");
	child->removeRenderarealimit();
}

inline bool Widget::isMyChild(Widget const* widget) const
{
	if (std::find(children.begin(), children.end(), (Widget*)widget) != children.end()) {
		return true;
	}
	for (Children::const_iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Widget const* child = *children_it;
		if (child->isMyChild(widget)) {
			return true;
		}
	}
	return false;
}

inline void Widget::setState(State state)
{
	State old_state = this->state;
	this->state = state;
	// If visibility will change, then inform parent about it
	if ((old_state != HIDDEN && state == HIDDEN) ||
	    (old_state == HIDDEN && state != HIDDEN)) {
		markToNeedReposition();
	}
}

inline void Widget::fireEvent(void)
{
	if (eventlistener) {
		eventlistener->handleGuiEvent(GuiEvent::fromWidget(this, 0));
	}
}

inline void Widget::fireEvent(int action)
{
	if (eventlistener) {
		eventlistener->handleGuiEvent(GuiEvent::fromWidget(this, action));
	}
}

inline bool Widget::mouseOver(int32_t x_origin, int y_origin, int32_t mouse_x, int32_t mouse_y) const
{
	if (state != HIDDEN &&
	    mouse_x >= x_origin + x && mouse_x < x_origin + x + (int32_t)width &&
	    mouse_y >= y_origin + y && mouse_y < y_origin + y + (int32_t)height) {
		if (positionOutsideRenderarealimit(x_origin, y_origin, mouse_x, mouse_y)) {
			return false;
		}
		return true;
	}
	return false;
}

inline Widget const* Widget::mouseOverRecursive(int32_t x_origin, int y_origin, int32_t mouse_x, int32_t mouse_y) const
{
	if (state == HIDDEN) {
		return NULL;
	}
	if (positionOutsideRenderarealimit(x_origin, y_origin, mouse_x, mouse_y)) {
		return NULL;
	}
	for (Children::const_reverse_iterator children_rit = children.rbegin();
	     children_rit != children.rend();
	     children_rit ++) {
		Widget const* child = *children_rit;
		Widget const* widget_under_mouse = child->mouseOverRecursive(x_origin + x, y_origin + y, mouse_x, mouse_y);
		if (widget_under_mouse) {
			return widget_under_mouse;
		}
	}
	if (mouseOver(x_origin, y_origin, mouse_x, mouse_y)) {
		return this;
	}
	return NULL;
}

inline Widget* Widget::mouseOverRecursive(int32_t x_origin, int y_origin, int32_t mouse_x, int32_t mouse_y)
{
	if (state == HIDDEN) {
		return NULL;
	}
	if (positionOutsideRenderarealimit(x_origin, y_origin, mouse_x, mouse_y)) {
		return NULL;
	}
	for (Children::reverse_iterator children_rit = children.rbegin();
	     children_rit != children.rend();
	     children_rit ++) {
		Widget* child = *children_rit;
		Widget* widget_under_mouse = child->mouseOverRecursive(x_origin + x, y_origin + y, mouse_x, mouse_y);
		if (widget_under_mouse) {
			return widget_under_mouse;
		}
	}
	if (mouseOver(x_origin, y_origin, mouse_x, mouse_y)) {
		return this;
	}
	return NULL;
}

inline void Widget::registerChild(Widget* child)
{
	HppAssert(std::find(children.begin(), children.end(), child) == children.end(), "Child of Widget already registered!");
	children.push_back(child);
}

inline void Widget::unregisterChild(Widget* child)
{
	Children::iterator children_find = std::find(children.begin(), children.end(), child);
	HppAssert(children_find != children.end(), "Child of Widget not found!");
	children.erase(children_find);
	onChildRemoved(child);
}

inline void Widget::setRenderarealimit(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	renderarealimit = true;
	renderarealimit_x = x;
	renderarealimit_y = y;
	renderarealimit_width = width;
	renderarealimit_height = height;
}

inline void Widget::removeRenderarealimit(void)
{
	renderarealimit = false;
}

inline bool Widget::positionOutsideRenderarealimit(int32_t x_origin, int32_t y_origin, int32_t x_abs, int32_t y_abs) const
{
	if (!renderarealimit) {
		return false;
	}
	int32_t x_rel = x_abs - x_origin;
	int32_t y_rel = y_abs - y_origin;
	if (x_rel < renderarealimit_x ||
	    x_rel >= renderarealimit_x + (int32_t)renderarealimit_width ||
	    y_rel < renderarealimit_y ||
	    y_rel >= renderarealimit_y + (int32_t)renderarealimit_height) {
		return true;
	}
	return false;
}

}

}

#endif
