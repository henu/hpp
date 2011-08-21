#ifndef HPP_GUI_SCROLLBAR_H
#define HPP_GUI_SCROLLBAR_H

#include "widget.h"

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
	inline Real getValue(void) const { return value; }
	inline Real getSliderSize(void) const { return size; }

private:

	Orientation ori;

	Real value;
	Real size;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);

};

inline Scrollbar::Scrollbar(Orientation ori) :
ori(ori),
value(0),
size(1)
{
}

inline Scrollbar::~Scrollbar(void)
{
}

inline void Scrollbar::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	rend->renderScrollbar(x_origin, y_origin, this, ori == HORIZONTAL, false, false, false);
}

}

}

#endif
