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

	inline void setContent(Widget* widget);

private:

	// Called by friend class Windowaread
	inline void setWindowarea(Windowarea* windowarea) { this->windowarea = windowarea; }

private:

	enum Part { NOTHING,
	            TITLEBAR,
	            TOP_EDGE,
	            TOPLEFT_CORNER,
	            TOPRIGHT_CORNER,
	            LEFT_EDGE,
	            RIGHT_EDGE,
	            BOTTOMLEFT_CORNER,
	            BOTTOMRIGHT_CORNER,
	            BOTTOM_EDGE };

	Windowarea* windowarea;

	Widget* content;

	// Position relative to parent
// TODO: Are these x_rel and y_rel useless? Because now all positions of Widgets are relative, so getPositionX() and getPositionY() should be enough...
	int32_t x_rel, y_rel;

	// Options
	std::string title;
	bool movable;
	bool resizable;
	bool disables_other_widgets;
	bool visible;

	// Dragging and pressing of buttons
	Part drag;
	int32_t drag_start_mouse_x, drag_start_mouse_y;
	int32_t drag_start_pos_x, drag_start_pos_y;
	uint32_t drag_start_width, drag_start_height;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onMouseMove(int32_t mouse_x, int32_t mouse_y);
	inline virtual void onSizeChange(void);
	inline virtual void onEnvironmentUpdated(void);

	inline void updateContentSizeAndPosition(void);

	inline Part getPartUnder(int32_t mouse_x, int32_t mouse_y) const;
	inline void setDraggedSize(Renderer const* rend, int32_t& width, int32_t& height);
	inline void setDraggedPosition(int32_t x, int32_t y);
};

inline Window::Window(void) :
windowarea(NULL),
content(NULL),
x_rel(0), y_rel(0),
movable(true),
resizable(true),
disables_other_widgets(false),
visible(true),
drag(NOTHING),
drag_start_mouse_x(0), drag_start_mouse_y(0),
drag_start_pos_x(0), drag_start_pos_y(0),
drag_start_width(0), drag_start_height(0)
{
}

inline Window::~Window(void)
{
}

inline void Window::setPosition(int32_t x, int32_t y)
{
	if (!windowarea) {
		throw Exception("Unable to set position of Window, because windowarea is not given yet!");
	}
	x_rel = x;
	y_rel = y;
	windowarea->windowPositionChanged(this, x_rel, y_rel);
}

inline void Window::setPositionCenter(void)
{
	if (!windowarea) {
		throw Exception("Unable to position Window to center, because windowarea is not given yet!");
	}
	uint32_t windowarea_width;
	uint32_t windowarea_height;
	windowarea->getSize(windowarea_width, windowarea_height);
	setPosition(((int32_t)windowarea_width - (int32_t)getWidth()) / 2, ((int32_t)windowarea_height - (int32_t)getHeight()) / 2);
}

inline void Window::setContent(Widget* widget)
{
HppAssert(!content, "Removing of content is not implemented yet!");
	content = widget;
	if (widget) {
		addChild(widget);
		updateContentSizeAndPosition();
	}
}

inline void Window::doRendering(int32_t x_origin, int32_t y_origin)
{
	getRenderer()->renderWindow(x_origin, y_origin, this, title);
}

inline bool Window::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	int32_t mouse_parent_x = mouse_x + getPositionX();
	int32_t mouse_parent_y = mouse_y + getPositionY();
	if (mouse_key == Hpp::Mousekey::LEFT) {
		Part part = getPartUnder(mouse_x, mouse_y);
		if (part == TITLEBAR ||
		    part == TOP_EDGE ||
		    part == TOPLEFT_CORNER ||
		    part == TOPRIGHT_CORNER ||
		    part == LEFT_EDGE ||
		    part == RIGHT_EDGE ||
		    part == BOTTOMLEFT_CORNER ||
		    part == BOTTOMRIGHT_CORNER ||
		    part == BOTTOM_EDGE) {
			drag = part;
			drag_start_mouse_x = mouse_parent_x;
			drag_start_mouse_y = mouse_parent_y;
			drag_start_pos_x = x_rel;
			drag_start_pos_y = y_rel;
			drag_start_width = getWidth();
			drag_start_height = getHeight();
			listenMouseMoves();
			listenMouseReleases(Mousekey::FLAG_LEFT);
		}
	}
	return true;
}

inline void Window::onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)widget;
	(void)mouse_x;
	(void)mouse_y;
	(void)mouse_key;
	listenMouseMoves(false);
	listenMouseReleases(0);
}

inline void Window::onMouseMove(int32_t mouse_x, int32_t mouse_y)
{
	Renderer const* rend = getRenderer();
	if (!rend) return;

	int32_t mouse_parent_x = mouse_x + getPositionX();
	int32_t mouse_parent_y = mouse_y + getPositionY();

	// Prevent mouse to move out of windowarea
	uint32_t windowarea_width, windowarea_height;
	windowarea->getSize(windowarea_width, windowarea_height);
	if (mouse_parent_x < 0) mouse_parent_x = 0;
	else if (mouse_parent_x >= (int32_t)windowarea_width) mouse_parent_x = windowarea_width - 1;
	if (mouse_parent_y < 0) mouse_parent_y = 0;
	else if (mouse_parent_y >= (int32_t)windowarea_height) mouse_parent_y = windowarea_height - 1;

	if (drag != NOTHING) {
		int32_t mouse_moved_x = mouse_parent_x - drag_start_mouse_x;
		int32_t mouse_moved_y = mouse_parent_y - drag_start_mouse_y;
		if (drag == TITLEBAR) {
			int32_t new_pos_x = drag_start_pos_x + mouse_moved_x;
			int32_t new_pos_y = drag_start_pos_y + mouse_moved_y;
			setDraggedPosition(new_pos_x, new_pos_y);
		} else if (drag == TOP_EDGE) {
			int32_t new_width = drag_start_width;
			int32_t new_height = drag_start_height - mouse_moved_y;
			setDraggedSize(rend, new_width, new_height);
			int32_t new_pos_x = drag_start_pos_x - (new_width - drag_start_width);
			int32_t new_pos_y = drag_start_pos_y - (new_height - drag_start_height);
			setDraggedPosition(new_pos_x, new_pos_y);
		} else if (drag == LEFT_EDGE) {
			int32_t new_width = drag_start_width - mouse_moved_x;
			int32_t new_height = drag_start_height;
			setDraggedSize(rend, new_width, new_height);
			int32_t new_pos_x = drag_start_pos_x - (new_width - drag_start_width);
			int32_t new_pos_y = drag_start_pos_y - (new_height - drag_start_height);
			setDraggedPosition(new_pos_x, new_pos_y);
		} else if (drag == RIGHT_EDGE) {
			int32_t new_width = drag_start_width + mouse_moved_x;
			int32_t new_height = drag_start_height;
			setDraggedSize(rend, new_width, new_height);
		} else if (drag == BOTTOM_EDGE) {
			int32_t new_width = drag_start_width;
			int32_t new_height = drag_start_height + mouse_moved_y;
			setDraggedSize(rend, new_width, new_height);
		} else if (drag == TOPLEFT_CORNER) {
			int32_t new_width = drag_start_width - mouse_moved_x;
			int32_t new_height = drag_start_height - mouse_moved_y;
			setDraggedSize(rend, new_width, new_height);
			int32_t new_pos_x = drag_start_pos_x - (new_width - drag_start_width);
			int32_t new_pos_y = drag_start_pos_y - (new_height - drag_start_height);
			setDraggedPosition(new_pos_x, new_pos_y);
		} else if (drag == TOPRIGHT_CORNER) {
			int32_t new_width = drag_start_width + mouse_moved_x;
			int32_t new_height = drag_start_height - mouse_moved_y;
			setDraggedSize(rend, new_width, new_height);
			int32_t new_pos_x = drag_start_pos_x;
			int32_t new_pos_y = drag_start_pos_y - (new_height - drag_start_height);
			setDraggedPosition(new_pos_x, new_pos_y);
		} else if (drag == BOTTOMLEFT_CORNER) {
			int32_t new_width = drag_start_width - mouse_moved_x;
			int32_t new_height = drag_start_height + mouse_moved_y;
			setDraggedSize(rend, new_width, new_height);
			int32_t new_pos_x = drag_start_pos_x - (new_width - drag_start_width);
			int32_t new_pos_y = drag_start_pos_y;
			setDraggedPosition(new_pos_x, new_pos_y);
		} else if (drag == BOTTOMRIGHT_CORNER) {
			int32_t new_width = drag_start_width + mouse_moved_x;
			int32_t new_height = drag_start_height + mouse_moved_y;
			setDraggedSize(rend, new_width, new_height);
		}
	}
}

inline void Window::onSizeChange(void)
{
	updateContentSizeAndPosition();
}

inline void Window::onEnvironmentUpdated(void)
{
	updateContentSizeAndPosition();
}

inline void Window::updateContentSizeAndPosition(void)
{
	Renderer* rend = getRenderer();
	if (rend && content) {
		uint32_t titlebar_height = rend->getWindowTitlebarHeight();
		uint32_t edge_left_width = rend->getWindowEdgeLeftWidth();
		uint32_t edge_right_width = rend->getWindowEdgeRightWidth();
		uint32_t edge_bottom_height = rend->getWindowEdgeBottomHeight();
		setChildPosition(content, edge_left_width, titlebar_height);
		if (getWidth() >= edge_left_width + edge_right_width &&
		    getHeight() >= titlebar_height + edge_bottom_height) {
			setChildSize(content, getWidth() - edge_left_width - edge_right_width, getHeight() - titlebar_height - edge_bottom_height);
		}
	}
}

inline Window::Part Window::getPartUnder(int32_t mouse_x, int32_t mouse_y) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return NOTHING;
	int32_t edge_top_height = rend->getWindowEdgeTopHeight();
	int32_t edge_left_width = rend->getWindowEdgeLeftWidth();
	int32_t edge_right_width = rend->getWindowEdgeRightWidth();
	int32_t edge_bottom_height = rend->getWindowEdgeBottomHeight();
	int32_t corner_size = rend->getWindowDragcornerSize();
	int32_t width = getWidth();
	int32_t height = getHeight();
	// Top edge
	if (mouse_y < edge_top_height) {
		if (mouse_x < corner_size) {
			return TOPLEFT_CORNER;
		} else if (mouse_x >= width - corner_size) {
			return TOPRIGHT_CORNER;
		}
		return TOP_EDGE;
	}
	// Bottom edge
	else if (mouse_y >= height - edge_bottom_height) {
		if (mouse_x < corner_size) {
			return BOTTOMLEFT_CORNER;
		} else if (mouse_x >= width - corner_size) {
			return BOTTOMRIGHT_CORNER;
		}
		return BOTTOM_EDGE;
	}
	// Left edge
	else if (mouse_x < edge_left_width) {
		if (mouse_y < corner_size) {
			return TOPLEFT_CORNER;
		} else if (mouse_y >= height - corner_size) {
			return BOTTOMLEFT_CORNER;
		}
		return LEFT_EDGE;
	}
	// Right edge
	else if (mouse_x >= width - edge_right_width) {
		if (mouse_y < corner_size) {
			return TOPRIGHT_CORNER;
		} else if (mouse_y >= height - corner_size) {
			return BOTTOMRIGHT_CORNER;
		}
		return RIGHT_EDGE;
	}
	// The rest will be titlebar, at least for now
	else {
		return TITLEBAR;
	}
}

inline void Window::setDraggedSize(Renderer const* rend, int32_t& width, int32_t& height)
{
	int32_t window_min_width = rend->getWindowEdgeLeftWidth() + rend->getWindowEdgeRightWidth();
	int32_t window_min_height = rend->getWindowTitlebarHeight() + rend->getWindowEdgeBottomHeight();
	if (content) {
		window_min_width += content->getMinWidth();
		if (width < window_min_width) width = window_min_width;
		window_min_height += content->getMinHeight(window_min_width - rend->getWindowEdgeLeftWidth() - rend->getWindowEdgeRightWidth());
		if (height < window_min_height) height = window_min_height;
	} else {
		if (width < window_min_width) width = window_min_width;
		if (height < window_min_height) height = window_min_height;
	}
	setSize(width, height);
}

inline void Window::setDraggedPosition(int32_t x, int32_t y)
{
	HppAssert(windowarea, "There should be Windowarea set!");
	x_rel = x;
	y_rel = y;
	windowarea->windowPositionChanged(this, x, y);
}

}

}

#endif
