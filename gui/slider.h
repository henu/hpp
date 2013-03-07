#ifndef HPP_GUI_SLIDER_H
#define HPP_GUI_SLIDER_H

#include "widget.h"

namespace Hpp
{

namespace Gui
{

class Slider : public Widget
{

public:

	enum Orientation { HORIZONTAL, VERTICAL };

	inline Slider(Orientation ori = HORIZONTAL);
	inline virtual ~Slider(void);

	inline void setValue(Real value) { this->value = value; }
	inline void setButtonMove(Real move) { button_move = move; }
	inline Real getValue(void) const { return value; }
	inline Real getButtonMove(void) const { return button_move; }

	inline void setOrientation(Orientation ori);
	inline Orientation getOrientation(void) const { return ori; }

	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	enum Dragtype { NOTHING, MIDDLE_BUTTON, SLIDER, BG_BEFORE, BG_AFTER };

	Orientation ori;

	Real value;
	Real button_move;
	Real bg_move;

	Dragtype drag;
	Real drag_slider_pos;

	bool slider_pressed;

	// These variables may be called every time getElementOver()
	// is called, so it is valid only immediately after the call.
	Real mouse_over_slider;
	Real mouse_over_bg;

	// Virtual functions for Widget
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual bool onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onMouseMove(int32_t mouse_x, int32_t mouse_y);
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);

	inline Dragtype getElementOver(int32_t mouse_x, int32_t mouse_y);

};

inline Slider::Slider(Orientation ori) :
ori(ori),
value(0),
button_move(0.1),
bg_move(0.2),
drag(NOTHING),
slider_pressed(false),
mouse_over_slider(0.0),
mouse_over_bg(0.0)
{
}

inline Slider::~Slider(void)
{
}

inline void Slider::setOrientation(Orientation ori)
{
	this->ori = ori;
	markSizeChanged();
}

inline uint32_t Slider::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	if (ori == HORIZONTAL) {
		return rend->getHorizSliderMinWidth();
	} else {
		return rend->getVertSliderMinHeight();
	}
}

inline uint32_t Slider::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	if (ori == HORIZONTAL) {
		return rend->getHorizSliderHeight();
	} else {
		return rend->getVertSliderWidth();
	}
}

inline bool Slider::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	Dragtype element_over = getElementOver(mouse_x, mouse_y);

	// If dragging is already done, then do nothing more now.
	if (drag != NOTHING) {
		return true;
	}

	if (mouse_key == Mousekey::LEFT) {
		if (element_over == SLIDER) {
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
		value = mouse_over_bg;
		if (value < 0.0) value = 0.0;
		else if (value > 1.0) value = 1.0;

		fireEvent();

		drag = MIDDLE_BUTTON;
		listenMouseReleases(Mousekey::FLAG_MIDDLE);
		listenMouseMoves();
	}

	return true;
}

inline bool Slider::onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	Dragtype element_over = getElementOver(mouse_x, mouse_y);
	if (mouse_key == Mousekey::LEFT) {
		if (drag == BG_BEFORE) {
			if (element_over == BG_BEFORE) {
				value -= bg_move;
				if (value < 0.0) value = 0.0;

				fireEvent();
			}
			drag = NOTHING;
			listenMouseReleases(0);
		} else if (drag == BG_AFTER) {
			if (element_over == BG_AFTER) {
				value += bg_move;
				if (value > 1.0) value = 1.0;

				fireEvent();
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

inline void Slider::onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	if (widget != this) {
		(void)mouse_x;
		(void)mouse_y;
		(void)mouse_key;
		drag = NOTHING;
		slider_pressed = false;
		listenMouseReleases(0);
		listenMouseMoves(false);
	}
}

inline void Slider::onMouseMove(int32_t mouse_x, int32_t mouse_y)
{
	getElementOver(mouse_x, mouse_y);
	if (drag == SLIDER) {
		Renderer* const rend = getRenderer();
		if (!rend) return;

		int32_t pos;
		if (ori == HORIZONTAL) pos = mouse_x;
		else pos = mouse_y;

		// Calculate sizes of different parts
		uint32_t area_size;
		uint32_t slider_size;
		if (ori == HORIZONTAL) {
			area_size = getWidth();
			slider_size = rend->getHorizSliderWidth();
		} else {
			area_size = getHeight();
			slider_size = rend->getVertSliderHeight();
		}
		Real bg_size = area_size - slider_size;

		Real pos_slider_begin = slider_size * drag_slider_pos;
		value = (pos - pos_slider_begin) / bg_size;
		if (value < 0.0) value = 0.0;
		else if (value > 1.0) value = 1.0;

		fireEvent();

	} else if (drag == MIDDLE_BUTTON) {
		value = mouse_over_bg;
		if (value < 0.0) value = 0.0;
		else if (value > 1.0) value = 1.0;

		fireEvent();
	}
}

inline void Slider::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	rend->renderSlider(x_origin, y_origin, this, ori == HORIZONTAL, slider_pressed);
}

inline Slider::Dragtype Slider::getElementOver(int32_t mouse_x, int32_t mouse_y)
{
	Renderer* const rend = getRenderer();
	if (!rend) return NOTHING;

	int32_t pos;
	if (ori == HORIZONTAL) pos = mouse_x;
	else pos = mouse_y;

	// Calculate sizes of different parts
	uint32_t area_size;
	uint32_t slider_size;
	if (ori == HORIZONTAL) {
		area_size = getWidth();
		slider_size = rend->getHorizSliderWidth();
	} else {
		area_size = getHeight();
		slider_size = rend->getVertSliderHeight();
	}
	Real bg_only_size = area_size - slider_size;

	if (pos < bg_only_size * value) {
		mouse_over_bg = (pos - slider_size/2.0) / bg_only_size;
		return BG_BEFORE;
	} else if (area_size - pos < bg_only_size * (1.0 - value)) {
		mouse_over_bg = (pos - slider_size/2.0) /  bg_only_size;
		return BG_AFTER;
	} else {
		Real slider_begin = bg_only_size * value;
		mouse_over_bg = (pos - slider_size/2.0) / bg_only_size;
		mouse_over_slider = (pos - slider_begin) / slider_size;
		return SLIDER;
	}
}

}

}

#endif
