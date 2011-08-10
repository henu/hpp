#ifndef HPP_GUI_WINDOW_H
#define HPP_GUI_WINDOW_H

#include "widget.h"
#include "renderer.h"
#include "windowarea.h"

#include <string>

namespace Hpp
{

namespace Gui
{

class Window : public Widget
{

	friend class Windowarea;

public:

	inline Window(void);
	inline virtual ~Window(void);

// TODO: Inform position changed!
	inline void setPosition(int32_t x, int32_t y);
	inline void setPositionCenter(void);
	inline void setSize(int32_t width, int32_t height) { Widget::setSize(width, height); }

	inline void setTitle(std::string const& title) { this->title = title; }
	inline void setMovable(bool movable = true) { this->movable = movable; }
	inline void setResizable(bool resizable = true) { this->resizable = resizable; }
	inline void setDisablesOtherWidgets(bool disables_other_widgets = true) { this->disables_other_widgets = disables_other_widgets; }
	inline void hide(void) { setState(HIDDEN); }
	inline void reveal(void) { setState(ENABLED); }

private:

	// Called by friend class Windowaread
	inline void setWindowarea(Windowarea* windowarea) { this->windowarea = windowarea; }

private:

	Windowarea* windowarea;

	// Position relative to parent
	int32_t x_rel, y_rel;

	// Options
	std::string title;
	bool movable;
	bool resizable;
	bool disables_other_widgets;
	bool visible;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);

};

inline Window::Window(void) :
windowarea(NULL),
x_rel(0), y_rel(0),
movable(true),
resizable(true),
disables_other_widgets(false),
visible(true)
{
}

inline Window::~Window(void)
{
}

inline void Window::setPosition(int32_t x, int32_t y)
{
	x_rel = x;
	y_rel = y;
	windowarea->windowPositionChanged(this, x_rel, y_rel);
}

inline void Window::setPositionCenter(void)
{
	uint32_t windowarea_width;
	uint32_t windowarea_height;
	windowarea->getSize(windowarea_width, windowarea_height);
	setPosition(((int32_t)windowarea_width - (int32_t)getWidth()) / 2, ((int32_t)windowarea_height - (int32_t)getHeight()) / 2);
}

inline void Window::doRendering(int32_t x_origin, int32_t y_origin)
{
	getRenderer()->renderWindow(x_origin, y_origin, this, title);
}

}

}

#endif
