#ifndef HPP_GUI_ENGINE_H
#define HPP_GUI_ENGINE_H

#include "../event.h"
#include "../assert.h"

#include <set>

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

private:

	typedef std::set< Widget* > Widgets;

	Renderer* rend;

	Widgets widgets;

	bool pos_or_size_changed;

	Widget* mouseover_widget;
	int32_t mouse_lastpos_x, mouse_lastpos_y;

	Menubar* menubar;

	void checkForNewMouseOver(void);

};

}

}

#endif
