#ifndef HPP_GUI_VECTORCONTAINER_H
#define HPP_GUI_VECTORCONTAINER_H

#include "widget.h"

#include <vector>

namespace Hpp
{

namespace Gui
{

class Vectorcontainer : public Widget
{

public:

	enum Direction { HORIZONTAL, VERTICAL };

	inline Vectorcontainer(void);
	inline virtual ~Vectorcontainer(void);

	inline void setDirection(Direction dir);

	inline void addWidget(Widget* widget);

	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMaxWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	typedef std::vector< Widget* > Widgets;

	Widgets widgets;

	Direction dir;

	// Virtual functions for Widget
	inline virtual void onSizeChange(void);
	inline virtual void onChildSizeChange(void);

	inline void updateWidgetSizesAndPositions();

};

inline Vectorcontainer::Vectorcontainer(void) :
dir(HORIZONTAL)
{
}

inline Vectorcontainer::~Vectorcontainer(void)
{
}

inline void Vectorcontainer::setDirection(Direction dir)
{
	this->dir = dir;
	markSizeChanged();
}

inline void Vectorcontainer::addWidget(Widget* widget)
{
	widgets.push_back(widget);
	addChild(widget);
	markSizeChanged();
}

inline uint32_t Vectorcontainer::getMinWidth(void) const
{
	uint32_t min_width = 0;
	if (dir == HORIZONTAL) {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Widget const* widget = *widgets_it;
			min_width += widget->getMinWidth();
		}
	} else {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Widget const* widget = *widgets_it;
			min_width = std::max(min_width, widget->getMinWidth());
		}
	}
	return min_width;
}

inline uint32_t Vectorcontainer::getMaxWidth(void) const
{
	uint32_t max_width = 0;
	if (dir == HORIZONTAL) {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Widget const* widget = *widgets_it;
			max_width += widget->getMaxWidth();
		}
	} else {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Widget const* widget = *widgets_it;
			max_width = std::min(max_width, widget->getMaxWidth());
		}
		max_width = std::max(getMinWidth(), max_width);
	}
	return max_width;
}

inline uint32_t Vectorcontainer::getMinHeight(uint32_t width) const
{
	uint32_t min_height = 0;
	if (dir == HORIZONTAL) {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Widget const* widget = *widgets_it;
			min_height = std::max(min_height, widget->getMinHeight(width));
		}
	} else {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Widget const* widget = *widgets_it;
			min_height += widget->getMinHeight(width);
		}
	}
	return min_height;
}

inline void Vectorcontainer::onSizeChange(void)
{
	updateWidgetSizesAndPositions();
}

inline void Vectorcontainer::onChildSizeChange(void)
{
	// Inform parent, and expect it to change my size
	markSizeChanged();
	// Now my size should be changed, so it is
	// time to update the size of my children.
	updateWidgetSizesAndPositions();
}

inline void Vectorcontainer::updateWidgetSizesAndPositions(void)
{
	if (widgets.empty()) {
		return;
	}
	if (dir == HORIZONTAL) {
		int32_t min_width = getMinWidth();
		int32_t width_left = getWidth() - min_width;
		if (width_left < 0) width_left = 0;
// TODO: Use max width too!
		uint32_t extra_width_per_widget = width_left / widgets.size();
		uint32_t extra_width_for_first = width_left - extra_width_per_widget * (widgets.size() - 1);
		uint32_t pos_x = 0;
		for (Widgets::iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Widget* widget = *widgets_it;
			uint32_t child_width = widget->getMinWidth();
			// In case of first widget, use different extra value
			if (widgets_it == widgets.begin()) child_width += extra_width_for_first;
			else child_width += extra_width_per_widget;
			setChildSize(widget, child_width, getHeight());
			setChildPosition(widget, pos_x, 0);
			pos_x += child_width;
		}
	} else {
		int32_t min_height = getMinHeight(getWidth());
		int32_t height_left = getHeight() - min_height;
		if (height_left < 0) height_left = 0;
		uint32_t extra_height_per_widget = height_left / widgets.size();
		uint32_t extra_height_for_first = height_left - extra_height_per_widget * (widgets.size() - 1);
		uint32_t pos_y = 0;
		for (Widgets::iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Widget* widget = *widgets_it;
			uint32_t child_height = widget->getMinHeight(getWidth());
			// In case of first widget, use different extra value
			if (widgets_it == widgets.begin()) child_height += extra_height_for_first;
			else child_height += extra_height_per_widget;
			setChildSize(widget, getWidth(), child_height);
			setChildPosition(widget, 0, pos_y);
			pos_y += child_height;
		}
	}
}

}

}

#endif


