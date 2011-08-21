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
class Window;
class Windowarea;

class Engine
{

	friend class Widget;
	friend class Renderer;

public:

	Engine(void);
	~Engine(void);

	void setRenderer(Renderer* rend);
	inline Renderer const* getRenderer(void) const { return rend; }
	inline Renderer* getRenderer(void) { return rend; }

	void render(void);

	void addWindow(Window* window);

	void setMenubar(Menubar* menubar);

	// Returns true if event was consumed.
	bool mouseEvent(Event const& event);

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	// Called by Widget
	void registerWidget(Widget* widget);
	void unregisterWidget(Widget* widget);

	// Called by Widget
	void setMouseOver(Widget* widget);
	void registerMouseClickListener(Widget* widget, Mousekey::KeycodeFlags flags);
	void registerMouseReleaseListener(Widget* widget, Mousekey::KeycodeFlags flags);

	// Called by Renderer
	inline void sizeOfRendererChanged(void) { updateSizes(); }
	inline void rendererIsBeingDestroyed(void) { rend = NULL; }

private:

	// ----------------------------------------
	// Private types
	// ----------------------------------------

	typedef std::set< Widget* > Widgets;

	typedef std::map< Widget*, Mousekey::KeycodeFlags > MouseEventListeners;

	// ----------------------------------------
	// Data
	// ----------------------------------------

	Renderer* rend;

	Widgets widgets;
	MouseEventListeners mouseclicklisteners;
	MouseEventListeners mousereleaselisteners;

	Widget* mouseover_widget;
	int32_t mouse_lastpos_x, mouse_lastpos_y;

	Windowarea* windowarea;
	Menubar* menubar;


	// ----------------------------------------
	// Private functions
	// ----------------------------------------

	void checkForNewMouseOver(void);

	// Update sizes of Widgets. This is called when area or
	// renderer is changed or when menubar is added/removed
	void updateSizes(void);

};

}

}

#endif