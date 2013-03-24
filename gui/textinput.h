#ifndef HPP_GUI_TEXTINPUT_H
#define HPP_GUI_TEXTINPUT_H

#include "textinputcontents.h"
#include "scrollbox.h"
#include "widget.h"
#include "renderer.h"

namespace Hpp
{

namespace Gui
{

class Textinput : public Widget
{

	friend class TextinputContents;

public:

	inline Textinput(void);
	inline virtual ~Textinput(void);

	inline void setValue(UnicodeString const& value);
	inline UnicodeString getValue(void) const;

	inline void setCols(size_t cols);
	inline size_t getCols(void) const { return cols; }

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	// Called by TextinputContents
	inline void submitted(void);
	inline void scrollToCursor(uint32_t cursor_pos_x, uint32_t cursor_pos_y, uint32_t cursor_width, uint32_t cursor_height);

private:

	Scrollbox scrollbox;
	TextinputContents contents;

	size_t cols;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);

	inline virtual void doRepositioning(void);

};

inline Textinput::Textinput(void) :
contents(this),
cols(12)
{
	addChild(&scrollbox);
	scrollbox.setHorizontalScrollbar(Scrollbox::NEVER);
	scrollbox.setVerticalScrollbar(Scrollbox::NEVER);
	scrollbox.setContent(&contents);
}

inline Textinput::~Textinput(void)
{
}

inline void Textinput::setValue(UnicodeString const& value)
{
	contents.setValue(value);
}

inline UnicodeString Textinput::getValue(void) const
{
	return contents.getValue();
}

inline void Textinput::setCols(size_t cols)
{
	this->cols = cols;
	markToNeedReposition();
}

inline uint32_t Textinput::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getTextinputWidth(cols);
}

inline uint32_t Textinput::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getTextinputHeight();
}

inline void Textinput::submitted(void)
{
	fireEvent();
}

inline void Textinput::scrollToCursor(uint32_t cursor_pos_x, uint32_t cursor_pos_y, uint32_t cursor_width, uint32_t cursor_height)
{
	scrollbox.scrollTo(cursor_pos_x, cursor_pos_y, cursor_width, cursor_height);
}

inline void Textinput::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	return rend->renderTextinput(x_origin, y_origin, this);
}

inline void Textinput::doRepositioning(void)
{
	Renderer const* rend = getRenderer();
	if (!rend) return;
	// Get edge sizes
	uint32_t edge_top, edge_left, edge_right, edge_bottom;
	rend->getTextinputEdgeSizes(edge_top, edge_left, edge_right, edge_bottom);
	repositionChild(&scrollbox, edge_left, edge_top, getWidth() - edge_left - edge_right, getHeight() - edge_top - edge_bottom);
}

}

}

#endif

