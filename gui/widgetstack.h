#ifndef HPP_GUI_WIDGETSTACK
#define HPP_GUI_WIDGETSTACK

#include "widget.h"

namespace Hpp
{

namespace Gui
{

class Widgetstack : public Widget
{

public:

	inline Widgetstack(void);
	inline virtual ~Widgetstack(void);

	inline void addWidget(Widget* widget);

	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	typedef std::vector< Widget* > Widgets;

	Widgets widgets;

	// Virtual functions for Widget
	inline virtual void doRepositioning(void);

};

inline Widgetstack::Widgetstack(void)
{
	setVerticalExpanding(1);
	setHorizontalExpanding(1);
}

inline Widgetstack::~Widgetstack(void)
{
}

inline void Widgetstack::addWidget(Widget* widget)
{
	widgets.push_back(widget);
	addChild(widget);
	markToNeedReposition();
}

inline uint32_t Widgetstack::getMinWidth(void) const
{
	uint32_t min_width = 0;
	for (Widgets::const_iterator widgets_it = widgets.begin();
	     widgets_it != widgets.end();
	     widgets_it ++) {
		Widget const* widget = *widgets_it;
		min_width = std::max(min_width, widget->getMinWidth());
	}
	return min_width;
}

inline uint32_t Widgetstack::getMinHeight(uint32_t width) const
{
	uint32_t min_height = 0;
	for (Widgets::const_iterator widgets_it = widgets.begin();
	     widgets_it != widgets.end();
	     widgets_it ++) {
		Widget const* widget = *widgets_it;
		min_height = std::max(min_height, widget->getMinHeight(width));
	}
	return min_height;
}

inline void Widgetstack::doRepositioning()
{
	for (Widgets::iterator widgets_it = widgets.begin();
	     widgets_it != widgets.end();
	     widgets_it ++) {
		Widget* widget = *widgets_it;
		repositionChild(widget, getPositionX(), getPositionY(), getWidth(), getHeight());
	}
}

}

}

#endif
