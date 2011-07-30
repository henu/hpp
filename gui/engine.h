#ifndef HPP_GUI_ENGINE_H
#define HPP_GUI_ENGINE_H

#include "../event.h"

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

	void render(Renderer* rend);

	void setMenubar(Menubar* menubar);

	// Returns true if event was consumed.
	bool mouseEvent(Event const& event);

private:

	// Called by Widget
	void setMouseOver(Widget* widget);

private:

	bool pos_or_size_changed;

	Widget* mouseover_widget;
	int32_t mouse_lastpos_x, mouse_lastpos_y;

	Menubar* menubar;

};

}

}

#endif
