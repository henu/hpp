#ifndef HPP_GUI_CONTAINERWIDGET_H
#define HPP_GUI_CONTAINERWIDGET_H

#include "widget.h"

namespace Hpp
{

namespace Gui
{

class Containerwidget : public Widget
{

public:

	enum Alignment { LEFT, RIGHT, TOP, BOTTOM, CENTER };

	inline Containerwidget(void);
	inline virtual ~Containerwidget(void);

	// Alignment
	inline Alignment getHorizontalAlignment(void) const { return align; }
	inline Alignment getVerticalAlignment(void) const { return valign; }
	inline void setHorizontalAlignment(Alignment align) { this->align = align; markSizeChanged(); }
	inline void setVerticalAlignment(Alignment align) { valign = align; markSizeChanged(); }

	// Expanding
	inline uint8_t getHorizontalExpanding(void) const { return expanding_horiz; }
	inline uint8_t getVerticalExpanding(void) const { return expanding_vert; }
	inline void setHorizontalExpanding(uint8_t expanding) { expanding_horiz = expanding; markSizeChanged(); }
	inline void setVerticalExpanding(uint8_t expanding) { expanding_vert = expanding; markSizeChanged(); }

private:

	Alignment align, valign;
	uint8_t expanding_horiz, expanding_vert;

};

inline Containerwidget::Containerwidget(void) :
align(CENTER),
valign(CENTER),
expanding_horiz(0),
expanding_vert(0)
{
}

inline Containerwidget::~Containerwidget(void)
{
}

}

}

#endif
