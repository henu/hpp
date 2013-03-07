#ifndef HPP_GUI_BUTTON_H
#define HPP_GUI_BUTTON_H

#include "widget.h"
#include "renderer.h"

#include "../unicodestring.h"
#include "../color.h"

namespace Hpp
{

namespace Gui
{

class Button : public Widget
{

public:

	inline Button(void);
	inline virtual ~Button(void);

	inline void setColor(Color const& color) { this->color = color; }
	inline void setLabel(UnicodeString const& label) { this->label = label; markSizeChanged(); }
	inline void setLabelColor(Color const& color) { label_color = color; }

	inline Color getColor(void) const { return color; }
	inline Color getLabelColor(void) const { return label_color; }

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	UnicodeString label;
	Color label_color;
	Color color;

	bool pressed;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onEnvironmentUpdated(void);

};

inline Button::Button(void) :
label_color(0, 0, 0),
color(1, 1, 1),
pressed(false)
{
}

inline Button::~Button(void)
{
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
		fireEvent();
	}
}

inline void Button::onEnvironmentUpdated(void)
{
	markSizeChanged();
}

}

}

#endif
