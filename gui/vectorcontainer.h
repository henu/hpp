#ifndef HPP_GUI_VECTORCONTAINER_H
#define HPP_GUI_VECTORCONTAINER_H

#include "containerwidget.h"

#include <vector>

namespace Hpp
{

namespace Gui
{

class Vectorcontainer : public Containerwidget
{

public:

	enum Direction { HORIZONTAL, VERTICAL };

	inline Vectorcontainer(void);
	inline virtual ~Vectorcontainer(void);

	inline void setDirection(Direction dir);

	inline void addWidget(Containerwidget* widget);

	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMaxWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	typedef std::vector< Containerwidget* > Widgets;

	Widgets widgets;

	Direction dir;

	// Virtual functions for Widget
	inline virtual void onSizeChange(void);
	inline virtual void onChildSizeChange(void);

	inline void updateWidgetSizesAndPositions();

	// Takes care of alignments, and positions widget to given area.
	// Also expands to full size, if expanding is wanted.
	inline void positionWidget(Containerwidget* widget, int32_t pos_x, int32_t pos_y, uint32_t width, uint32_t height);

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

inline void Vectorcontainer::addWidget(Containerwidget* widget)
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
			Containerwidget const* widget = *widgets_it;
			min_width += widget->getMinWidth();
		}
	} else {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Containerwidget const* widget = *widgets_it;
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
			Containerwidget const* widget = *widgets_it;
			max_width += widget->getMaxWidth();
		}
	} else {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Containerwidget const* widget = *widgets_it;
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
			Containerwidget const* widget = *widgets_it;
			min_height = std::max(min_height, widget->getMinHeight(width));
		}
	} else {
		for (Widgets::const_iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Containerwidget const* widget = *widgets_it;
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
		int32_t space_left = getWidth() - min_width;
		if (space_left < 0) space_left = 0;
		// Calculate total expanding, so relative expandings
		// of widgets can be calculated.
		uint32_t total_expanding = 0;
		for (Widgets::iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Containerwidget* widget = *widgets_it;
			total_expanding += widget->getHorizontalExpanding();
		}
		uint32_t pos_x = 0;
		uint32_t space_distributed = 0;
		for (Widgets::iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Containerwidget* widget = *widgets_it;
			uint32_t child_width = widget->getMinWidth();
			// If this is last widget, then give
			// all of remaining space to it.
			if (widgets.end() - widgets_it == 1) {
				child_width += space_left;
			}
			// Use expanding value to calculate the size
			// of portion that this widget gets.
			else {
				Real portion;
				// If every Widget has expanding zero, then
				// distribute remaining space equally.
				if (total_expanding == 0) {
					portion = 1.0 / widgets.size();
				} else {
					portion = widget->getHorizontalExpanding() / (Real)total_expanding;
				}
				uint32_t space = round((space_left + space_distributed) * portion);
				if (space > (uint32_t)space_left) space = space_left;
				child_width += space;
				space_left -= space;
				space_distributed += space;
			}
			positionWidget(widget, pos_x, 0, child_width, getHeight());
			pos_x += child_width;
		}
	} else {
		int32_t min_height = getMinHeight(getWidth());
		int32_t space_left = getHeight() - min_height;
		if (space_left < 0) space_left = 0;
		// Calculate total expanding, so relative expandings
		// of widgets can be calculated.
		uint32_t total_expanding = 0;
		for (Widgets::iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Containerwidget* widget = *widgets_it;
			total_expanding += widget->getVerticalExpanding();
		}
		uint32_t pos_y = 0;
		uint32_t space_distributed = 0;
		for (Widgets::iterator widgets_it = widgets.begin();
		     widgets_it != widgets.end();
		     widgets_it ++) {
			Containerwidget* widget = *widgets_it;
			uint32_t child_height = widget->getMinHeight(getWidth());
			// If this is last widget, then give
			// all of remaining space to it.
			if (widgets.end() - widgets_it == 1) {
				child_height += space_left;
			}
			// Use expanding value to calculate the size
			// of portion that this widget gets.
			else {
				Real portion;
				// If every Widget has expanding zero, then
				// distribute remaining space equally.
				if (total_expanding == 0) {
					portion = 1.0 / widgets.size();
				} else {
					portion = widget->getVerticalExpanding() / (Real)total_expanding;
				}
				uint32_t space = round((space_left + space_distributed) * portion);
				if (space > (uint32_t)space_left) space = space_left;
				child_height += space;
				space_left -= space;
				space_distributed += space;
			}
			positionWidget(widget, 0, pos_y, getWidth(), child_height);
			pos_y += child_height;
		}
	}
}

inline void Vectorcontainer::positionWidget(Containerwidget* widget, int32_t pos_x, int32_t pos_y, uint32_t width, uint32_t height)
{
	// Calculate size
	uint32_t widget_width = widget->getMinWidth();
	if (widget->getHorizontalExpanding() > 0) widget_width = width;
	uint32_t widget_height = widget->getMinHeight(widget_width);
	if (widget->getVerticalExpanding() > 0) widget_height = height;

	// Calculate position
	int32_t pos_x_rel;
	int32_t pos_y_rel;
	if (widget->getHorizontalAlignment() == Containerwidget::CENTER) {
		pos_x_rel = (width - widget_width) / 2;
	} else if (widget->getHorizontalAlignment() == Containerwidget::RIGHT) {
		pos_x_rel = width - widget_width;
	} else {
		pos_x_rel = 0;
	}
	if (widget->getVerticalAlignment() == Containerwidget::CENTER) {
		pos_y_rel = (height - widget_height) / 2;
	} else if (widget->getVerticalAlignment() == Containerwidget::BOTTOM) {
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

