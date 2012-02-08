#ifndef HPP_GUI_SCROLLBAR_H
#define HPP_GUI_SCROLLBAR_H

#include "callback.h"
#include "widget.h"
#include "renderer.h"
#include "../real.h"

namespace Hpp
{

namespace Gui
{

class Scrollbar : public Widget
{

public:

	enum Orientation { HORIZONTAL, VERTICAL };

	inline Scrollbar(Orientation ori);
	inline virtual ~Scrollbar(void);

	inline void setValue(Real value) { this->value = value; }
	inline void setSliderSize(Real size) { this->size = size; }
	inline void setButtonMove(Real move) { button_move = move; }
	inline Real getValue(void) const { return value; }
	inline Real getSliderSize(void) const { return size; }
	inline Real getButtonMove(void) const { return button_move; }

	inline void setCallbackFunc(CallbackFunc callback, void* data);

private:

	enum Dragtype { NOTHING, BUTTON1, BUTTON2, MIDDLE_BUTTON, SLIDER, BG_BEFORE, BG_AFTER };

	Orientation ori;

	Real value;
	Real size;
	Real button_move;

	Dragtype drag;
	Real drag_slider_pos;

	bool button1_pressed;
	bool button2_pressed;
	bool slider_pressed;

	// These variables may be called every time getElementOver()
	// is called, so it is valid only immediately after the call.
	Real mouse_over_slider;
	Real mouse_over_bg;

	CallbackFunc callback;
	void* callback_data;

	// Virtual functions for Widget
	inline virtual void onMouseOver(int32_t mouse_x, int32_t mouse_y);
	inline virtual void onMouseOut(int32_t mouse_x, int32_t mouse_y);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual bool onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onMouseMove(int32_t mouse_x, int32_t mouse_y);
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);

	inline Dragtype getElementOver(int32_t mouse_x, int32_t mouse_y);

};

inline Scrollbar::Scrollbar(Orientation ori) :
ori(ori),
value(0),
size(1),
button_move(0.1),
drag(NOTHING),
button1_pressed(false),
button2_pressed(false),
slider_pressed(false),
mouse_over_slider(0.0),
mouse_over_bg(0.0),
callback(NULL),
callback_data(NULL)
{
}

inline Scrollbar::~Scrollbar(void)
{
}

inline void Scrollbar::setCallbackFunc(CallbackFunc callback, void* data)
{
	this->callback = callback;
	callback_data = data;
}

inline void Scrollbar::onMouseOver(int32_t mouse_x, int32_t mouse_y)
{
	Dragtype element_over = getElementOver(mouse_x, mouse_y);

	if (drag == BUTTON1) {
		button1_pressed = (element_over == BUTTON1);
	} else if (drag == BUTTON2) {
		button2_pressed = (element_over == BUTTON2);
	}
}

inline void Scrollbar::onMouseOut(int32_t mouse_x, int32_t mouse_y)
{
	(void)mouse_x;
	(void)mouse_y;
	button1_pressed = false;
	button2_pressed = false;
}

inline bool Scrollbar::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	Dragtype element_over = getElementOver(mouse_x, mouse_y);

	// If dragging is already done, then do nothing more now.
	if (drag != NOTHING) {
		return true;
	}

	if (mouse_key == Mousekey::LEFT) {
		if (element_over == BUTTON1) {
			button1_pressed = true;
			drag = BUTTON1;
		} else if (element_over == BUTTON2) {
			button2_pressed = true;
			drag = BUTTON2;
		} else if (element_over == SLIDER) {
			slider_pressed = true;
			listenMouseMoves();
			drag = SLIDER;
			drag_slider_pos = mouse_over_slider;
		} else if (element_over == BG_BEFORE) {
			drag = BG_BEFORE;
		} else if (element_over == BG_AFTER) {
			drag = BG_AFTER;
		}
		listenMouseReleases(Mousekey::FLAG_LEFT);
	} else if (mouse_key == Mousekey::MIDDLE) {
		if (element_over != BUTTON1 && element_over != BUTTON2) {
			value = mouse_over_bg;
			if (value < 0.0) value = 0.0;
			else if (value > 1.0) value = 1.0;
			// Do ack if it has been set
			if (callback) {
				callback(this, callback_data);
			}
			drag = MIDDLE_BUTTON;
			listenMouseReleases(Mousekey::FLAG_MIDDLE);
			listenMouseMoves();
		}
	}

	return true;
}

inline bool Scrollbar::onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	Dragtype element_over = getElementOver(mouse_x, mouse_y);
	if (mouse_key == Mousekey::LEFT) {
		if (drag == BUTTON1) {
			if (element_over == BUTTON1) {
				value -= button_move;
				if (value < 0.0) value = 0.0;
				// Do callback if it has been set
				if (callback) {
					callback(this, callback_data);
				}
			}
			button1_pressed = false;
			drag = NOTHING;
			listenMouseReleases(0);
		} else if (drag == BUTTON2) {
			if (element_over == BUTTON2) {
				value += button_move;
				if (value > 1.0) value = 1.0;
				// Do callback if it has been set
				if (callback) {
					callback(this, callback_data);
				}
			}
			button2_pressed = false;
			drag = NOTHING;
			listenMouseReleases(0);
		} else if (drag == BG_BEFORE) {
			if (element_over == BG_BEFORE) {
				value -= size;
				if (value < 0.0) value = 0.0;
				// Do callback if it has been set
				if (callback) {
					callback(this, callback_data);
				}
			}
			drag = NOTHING;
			listenMouseReleases(0);
		} else if (drag == BG_AFTER) {
			if (element_over == BG_AFTER) {
				value += size;
				if (value > 1.0) value = 1.0;
				// Do callback if it has been set
				if (callback) {
					callback(this, callback_data);
				}
			}
			drag = NOTHING;
			listenMouseReleases(0);
		} else if (drag == SLIDER) {
			slider_pressed = false;
			drag = NOTHING;
			listenMouseReleases(0);
		}
	} else if (mouse_key == Mousekey::MIDDLE) {
		drag = NOTHING;
		listenMouseReleases(0);
		listenMouseMoves(false);
	}
	return true;
}

inline void Scrollbar::onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	if (widget != this) {
		(void)mouse_x;
		(void)mouse_y;
		(void)mouse_key;
		drag = NOTHING;
		button1_pressed = false;
		button2_pressed = false;
		slider_pressed = false;
		listenMouseReleases(0);
		listenMouseMoves(false);
	}
}

inline void Scrollbar::onMouseMove(int32_t mouse_x, int32_t mouse_y)
{
	Dragtype element_over = getElementOver(mouse_x, mouse_y);

	if (drag == SLIDER) {
		Renderer* const rend = getRenderer();
		if (!rend) return;

		int32_t pos;
		if (ori == HORIZONTAL) pos = mouse_x;
		else pos = mouse_y;

		// Calculate sizes of different parts
		uint32_t button1_size, button2_size;
		uint32_t area_size;
		if (ori == HORIZONTAL) {
			button1_size = rend->getScrollbarButtonLeftWidth();
			button2_size = rend->getScrollbarButtonRightWidth();
			area_size = getWidth();
		} else {
			button1_size = rend->getScrollbarButtonUpHeight();
			button2_size = rend->getScrollbarButtonDownHeight();
			area_size = getHeight();
		}
		Real slider_size = area_size * size;
		Real bg_size = area_size - button1_size - button2_size - slider_size;

		Real pos_slider_begin = button1_size + slider_size * drag_slider_pos;
		value = (pos - pos_slider_begin) / bg_size;
		if (value < 0.0) value = 0.0;
		else if (value > 1.0) value = 1.0;
		// Do callback if it has been set
		if (callback) {
			callback(this, callback_data);
		}

	} else if (drag == MIDDLE_BUTTON) {
		if (element_over != BUTTON1 && element_over != BUTTON2) {
			value = mouse_over_bg;
			if (value < 0.0) value = 0.0;
			else if (value > 1.0) value = 1.0;
			// Do callback if it has been set
			if (callback) {
				callback(this, callback_data);
			}
		}
	}
}

inline void Scrollbar::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	rend->renderScrollbar(x_origin, y_origin, this, ori == HORIZONTAL, button1_pressed, button2_pressed, slider_pressed);
}

inline Scrollbar::Dragtype Scrollbar::getElementOver(int32_t mouse_x, int32_t mouse_y)
{
	Renderer* const rend = getRenderer();
	if (!rend) return NOTHING;

	int32_t pos;
	if (ori == HORIZONTAL) pos = mouse_x;
	else pos = mouse_y;

	// Calculate sizes of different parts
	uint32_t button1_size, button2_size;
	uint32_t area_size;
	if (ori == HORIZONTAL) {
		button1_size = rend->getScrollbarButtonLeftWidth();
		button2_size = rend->getScrollbarButtonRightWidth();
		area_size = getWidth();
	} else {
		button1_size = rend->getScrollbarButtonUpHeight();
		button2_size = rend->getScrollbarButtonDownHeight();
		area_size = getHeight();
	}
	Real slider_size = area_size * size;
	Real bg_only_size = area_size - button1_size - button2_size - slider_size;

	if (pos < (int32_t)button1_size) {
		return BUTTON1;
	} else if (pos >= (int32_t)area_size - (int32_t)button2_size) {
		return BUTTON2;
	} else if (pos - button1_size < bg_only_size * value) {
		mouse_over_bg = (pos - button1_size - slider_size/2.0) / (area_size - button1_size - button2_size - slider_size);
		return BG_BEFORE;
	} else if (-pos + (area_size - button2_size) < bg_only_size * (1.0 - value)) {
		mouse_over_bg = (pos - button1_size - slider_size/2.0) / (area_size - button1_size - button2_size - slider_size);
		return BG_AFTER;
	} else {
		Real slider_begin = button1_size + bg_only_size * value;
		mouse_over_bg = (pos - button1_size - slider_size/2.0) / (area_size - button1_size - button2_size - slider_size);
		mouse_over_slider = (pos - slider_begin) / slider_size;
		return SLIDER;
	}
}

}

}

#endif
