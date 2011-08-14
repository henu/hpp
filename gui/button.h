#ifndef HPP_GUI_BUTTON_H
#define HPP_GUI_BUTTON_H

#include "containerwidget.h"
#include "renderer.h"

#include "../unicodestring.h"

namespace Hpp
{

namespace Gui
{

class Button : public Containerwidget
{

public:

	inline Button(void);
	inline virtual ~Button(void);

	inline void setLabel(UnicodeString const& label) { this->label = label; markSizeChanged(); }

	// Virtual functions for Widget
	inline virtual uint32_t getMaxWidth(void) const;
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	UnicodeString label;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual void onEnvironmentUpdated(void);

};

inline Button::Button(void)
{
}

inline Button::~Button(void)
{
}

inline uint32_t Button::getMaxWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getButtonWidth(label);
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
	bool pressed = false;
	return rend->renderButton(x_origin, y_origin, this, label, pressed);
}

inline void Button::onEnvironmentUpdated(void)
{
	markSizeChanged();
}

}

}

#endif
