#ifndef HPP_GUI_BUTTON_H
#define HPP_GUI_BUTTON_H

#include "widget.h"
#include "renderer.h"
#include "callback.h"

#include "../unicodestring.h"

namespace Hpp
{

namespace Gui
{

class Button : public Widget
{

public:

	inline Button(void);
	inline virtual ~Button(void);

	inline void setLabel(UnicodeString const& label) { this->label = label; markSizeChanged(); }

	inline void setCallbackFunc(CallbackFunc callback, void* data);

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	UnicodeString label;

	CallbackFunc callback;
	void* callback_data;

	bool pressed;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onEnvironmentUpdated(void);

};

inline Button::Button(void) :
callback(NULL),
pressed(false)
{
}

inline Button::~Button(void)
{
}

inline void Button::setCallbackFunc(CallbackFunc callback, void* data)
{
	this->callback = callback;
	callback_data = data;
}

inline uint32_t Button::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getButtonWidth(label);
}

inline uint32_t Button::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getButtonHeight();
}

inline void Button::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	return rend->renderButton(x_origin, y_origin, this, label, pressed && isMouseOver());
}

inline bool Button::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	if (mouse_key == Mousekey::LEFT) {
		pressed = true;
		listenMouseReleases(Mousekey::FLAG_LEFT);
	}
	return true;
}

inline void Button::onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	HppAssert(mouse_key == Mousekey::LEFT, "Unexpected mouse release!");
	pressed = false;
	if (widget == this) {
		// Do callback if it has been set
		if (callback) {
			callback(widget, callback_data);
		}
	}
}

inline void Button::onEnvironmentUpdated(void)
{
	markSizeChanged();
}

}

}

#endif
