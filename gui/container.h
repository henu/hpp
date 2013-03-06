#ifndef HPP_GUI_CONTAINER_H
#define HPP_GUI_CONTAINER_H

#include "widget.h"

namespace Hpp
{

namespace Gui
{

class Container : public Widget
{

public:

	inline Container(void);
	inline virtual ~Container(void);

protected:

	// Takes care of alignments, and positions widget to given area.
	// Also expands to full size, if expanding is wanted.
	inline void positionWidget(Widget* widget, int32_t pos_x, int32_t pos_y, uint32_t width, uint32_t height, bool force_expanding = false);

private:

};

inline Container::Container(void)
{
}

inline Container::~Container(void)
{
}

inline void Container::positionWidget(Widget* widget, int32_t pos_x, int32_t pos_y, uint32_t width, uint32_t height, bool force_expanding)
{
	// Calculate size
	uint32_t widget_width = widget->getMinWidth();
	if (force_expanding || widget->getHorizontalExpanding() > 0) widget_width = width;
	uint32_t widget_height = widget->getMinHeight(widget_width);
	if (force_expanding || widget->getVerticalExpanding() > 0) widget_height = height;

	// Calculate position
	int32_t pos_x_rel;
	int32_t pos_y_rel;
	if (widget->getHorizontalAlignment() == Widget::CENTER) {
		pos_x_rel = (width - widget_width) / 2;
	} else if (widget->getHorizontalAlignment() == Widget::RIGHT) {
		pos_x_rel = width - widget_width;
	} else {
		pos_x_rel = 0;
	}
	if (widget->getVerticalAlignment() == Widget::CENTER) {
		pos_y_rel = (height - widget_height) / 2;
	} else if (widget->getVerticalAlignment() == Widget::BOTTOM) {
		pos_y_rel = height - widget_height;
	} else {
		pos_y_rel = 0;
	}
	setChildPosition(widget, pos_x + pos_x_rel, pos_y + pos_y_rel);
	setChildSize(widget, widget_width, widget_height);
}

}

}

#endif


