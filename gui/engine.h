#ifndef HPP_GUI_ENGINE_H
#define HPP_GUI_ENGINE_H

#include "../event.h"
#include "../assert.h"

#include <set>
#include <map>

namespace Hpp
{

class Viewport;

namespace Gui
{

class Widget;
class Renderer;
class Menubar;

class Engine
{

	friend class Widget;

public:

	Engine(void);
	~Engine(void);

	inline void setRenderer(Renderer* rend) { this->rend = rend; }
	inline Renderer const* getRenderer(void) const { return rend; }
	inline Renderer* getRenderer(void) { return rend; }

	void render(void);

	void setMenubar(Menubar* menubar);

	// Returns true if event was consumed.
	bool mouseEvent(Event const& event);

private:

	// Called by Widget
	void registerWidget(Widget* widget);
	void unregisterWidget(Widget* widget);

	// Called by Widget
	void setMouseOver(Widget* widget);
	void registerMouseClickListener(Widget* widget, Mousekey::KeycodeFlags flags);

private:

	// ----------------------------------------
	// Private types
	// ----------------------------------------

	typedef std::set< Widget* > Widgets;

	typedef std::map< Widget*, Mousekey::KeycodeFlags > MouseClickListeners;


	// ----------------------------------------
	// Data
	// ----------------------------------------

	Renderer* rend;

	Widgets widgets;
	MouseClickListeners mouseclicklisteners;

	bool pos_or_size_changed;

	Widget* mouseover_widget;
	int32_t mouse_lastpos_x, mouse_lastpos_y;

	Menubar* menubar;


	// ----------------------------------------
	// Private functions
	// ----------------------------------------

	void checkForNewMouseOver(void);

};

}

}

#endif
