#ifndef HPP_GUI_AUTOGRIDCONTAINER_H
#define HPP_GUI_AUTOGRIDCONTAINER_H

#include "widget.h"

#include <vector>
#include <algorithm>
#include <numeric>

namespace Hpp
{

namespace Gui
{

class Autogridcontainer : public Widget
{

public:

	inline Autogridcontainer(void);
	inline virtual ~Autogridcontainer(void);

	inline void addWidget(Widget* widget);

	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	typedef std::vector< Widget* > Widgets;
	typedef std::vector< uint32_t > Sizes;

	Widgets widgets;

	// Virtual functions for Widget
	inline virtual void doRepositioning(void);

	inline Sizes calculateWidthsOfColumns(uint32_t width) const;
	inline Sizes calculateHeightsOfRows(Sizes const& widths) const;

};

inline Autogridcontainer::Autogridcontainer(void)
{
}

inline Autogridcontainer::~Autogridcontainer(void)
{
}
inline void Autogridcontainer::addWidget(Widget* widget)
{
	widgets.push_back(widget);
	addChild(widget);
	markToNeedReposition();
}

inline uint32_t Autogridcontainer::getMinWidth(void) const
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

inline uint32_t Autogridcontainer::getMinHeight(uint32_t width) const
{
	Sizes widths = calculateWidthsOfColumns(width);
	Sizes heights = calculateHeightsOfRows(widths);

	uint32_t min_height = 0;

	for (Sizes::const_iterator heights_it = heights.begin();
	     heights_it != heights.end();
	     heights_it ++) {
		min_height += *heights_it;
	}

	return min_height;
}

inline void Autogridcontainer::doRepositioning(void)
{
	if (widgets.empty()) {
		return;
	}

	// Calculate widths of columns
	Sizes widths = calculateWidthsOfColumns(getWidth());
	Sizes heights = calculateHeightsOfRows(widths);

	// Calculate minimum width and height with these
	// options, and then calculate remainings
	size_t min_width = std::accumulate(widths.begin(), widths.end(), 0);
	size_t min_height = std::accumulate(heights.begin(), heights.end(), 0);
	size_t remaining_width = getWidth() - min_width;
	size_t remaining_height = getHeight() - min_height;

	// Share remainings among sizes
	size_t extra_width_per_column = remaining_width / widths.size();
	size_t extra_height_per_row = remaining_height / heights.size();
	for (size_t width_id = 0; width_id < widths.size(); width_id ++) {
		if (width_id < widths.size() - 1) {
			widths[width_id] += extra_width_per_column;
		} else {
			widths[width_id] += remaining_width - extra_width_per_column * (widths.size() - 1);
		}
	}
	for (size_t height_id = 0; height_id < heights.size(); height_id ++) {
		if (height_id < heights.size() - 1) {
			heights[height_id] += extra_height_per_row;
		} else {
			heights[height_id] += remaining_height - extra_height_per_row * (heights.size() - 1);
		}
	}

	// Go widgets through
	size_t column_id = 0;
	size_t row_id = 0;
	uint32_t pos_x = 0, pos_y = 0;
	for (Widgets::iterator widgets_it = widgets.begin();
	     widgets_it != widgets.end();
	     widgets_it ++) {
		Widget* widget = *widgets_it;

		uint32_t cell_width = widths[column_id];
		uint32_t cell_height = heights[row_id];
		repositionChild(widget, pos_x, pos_y, cell_width, cell_height);

		column_id ++;
		pos_x += cell_width;
		if (column_id == widths.size()) {
			column_id = 0;
			pos_x = 0;
			row_id ++;
			pos_y += cell_height;
		}
	}

}

inline Autogridcontainer::Sizes Autogridcontainer::calculateWidthsOfColumns(uint32_t width) const
{
	if (widgets.empty()) {
		return Sizes();
	}

	// First check what is biggest amount of columns there could be
	Sizes widgets_widths;
	for (Widgets::const_iterator widgets_it = widgets.begin();
	     widgets_it != widgets.end();
	     widgets_it ++) {
		Widget const* widget = *widgets_it;
		widgets_widths.push_back(widget->getMinWidth());
	}
	// Sort widths, so smallest thinnest can be got
	std::sort(widgets_widths.begin(), widgets_widths.end());
	if (widgets_widths[0] > width) {
		Sizes result_fail;
		result_fail.push_back(width);
		return result_fail;
	}
	size_t max_columns = 0;
	size_t max_columns_width = 0;
	while (max_columns < widgets_widths.size() && max_columns_width + widgets_widths[max_columns] <= width) {
		max_columns_width += widgets_widths[max_columns];
		max_columns ++;
	}

	// Now we know how many columns there can be in maximum.
	// Now it is time to go all column counts through and
	// check what would result to shortest height.
	Sizes best_columns_count_widths;
	uint32_t best_columns_count_height = -1;
	for (size_t columns_count = 1; columns_count <= max_columns; columns_count ++) {
		// First ensure that this column count would fit horizontally.
		Sizes widths_temp(columns_count, 0);
		for (size_t widget_id = 0;
		     widget_id < widgets.size();
		     widget_id ++) {
			Widget const* widget = widgets[widget_id];
			size_t column_id = widget_id % columns_count;
			widths_temp[column_id] = std::max(widths_temp[column_id], widget->getMinWidth());
		}
		uint32_t widths_temp_total = std::accumulate(widths_temp.begin(), widths_temp.end(), 0);
		// If this would result to too wide
		// width, then skip this column count.
		if (widths_temp_total > width) {
			continue;
		}

		// Now calculate height of this column count
		Sizes heights_temp = calculateHeightsOfRows(widths_temp);
		uint32_t heights_temp_total = std::accumulate(heights_temp.begin(), heights_temp.end(), 0);
		if (heights_temp_total < best_columns_count_height) {
			best_columns_count_widths = widths_temp;
			best_columns_count_height = heights_temp_total;
		}

	}

	return best_columns_count_widths;
}

inline Autogridcontainer::Sizes Autogridcontainer::calculateHeightsOfRows(Sizes const& widths) const
{
	Sizes result;
	for (size_t rowbegin = 0; rowbegin < widgets.size(); rowbegin += widths.size()) {
		uint32_t row_height = 0;
		for (size_t cell_id = rowbegin;
		     cell_id < std::min(rowbegin + widths.size(), widgets.size());
		     cell_id ++) {
			size_t column_id = cell_id - rowbegin;
			uint32_t column_width = widths[column_id];
			row_height = std::max(row_height, widgets[cell_id]->getMinHeight(column_width));
		}
		result.push_back(row_height);
	}
	return result;
}

}

}

#endif
