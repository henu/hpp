#ifndef HPP_GUI_LABEL_H
#define HPP_GUI_LABEL_H

#include "widget.h"
#include "renderer.h"

#include "../unicodestring.h"

namespace Hpp
{

namespace Gui
{

class Label : public Widget
{

public:

	inline Label(void);
	inline virtual ~Label(void);

	inline void setLabel(UnicodeString const& label) { this->label = label; markToNeedReposition(); }

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	UnicodeString label;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual void onEnvironmentUpdated(void);

};

inline Label::Label(void)
{
}

inline Label::~Label(void)
{
}

inline uint32_t Label::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getLabelWidth(label);
}

inline uint32_t Label::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getLabelHeight();
}

inline void Label::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	return rend->renderLabel(x_origin, y_origin, this, label);
}

inline void Label::onEnvironmentUpdated(void)
{
	markToNeedReposition();
}

}

}

#endif
