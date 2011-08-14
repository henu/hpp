#ifndef HPP_GUI_TEXTINPUT_H
#define HPP_GUI_TEXTINPUT_H

#include "containerwidget.h"
#include "renderer.h"

#include "../unicodestring.h"

namespace Hpp
{

namespace Gui
{

class Textinput : public Containerwidget
{

public:

	inline Textinput(void);
	inline virtual ~Textinput(void);

	inline void setValue(UnicodeString const& value) { this->value = value; }

	// Virtual functions for Widget
	inline virtual uint32_t getMaxWidth(void) const;
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	UnicodeString value;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual void onEnvironmentUpdated(void);

};

inline Textinput::Textinput(void)
{
}

inline Textinput::~Textinput(void)
{
}

inline uint32_t Textinput::getMaxWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getMinimumTextinputWidth();
}

inline uint32_t Textinput::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getMinimumTextinputWidth();
}

inline uint32_t Textinput::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getTextinputHeight();
}

inline void Textinput::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	return rend->renderTextinput(x_origin, y_origin, this, value);
}

inline void Textinput::onEnvironmentUpdated(void)
{
	markSizeChanged();
}

}

}

#endif

