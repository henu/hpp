#ifndef HPP_GUI_SCROLLBOX_H
#define HPP_GUI_SCROLLBOX_H

#include "widget.h"

namespace Hpp
{

namespace Gui
{

class Scrollbox : public Widget
{

public:

	enum Scrollbartype { NEVER, ON_DEMAND, ALWAYS };

	inline Scrollbox(void);
	inline virtual ~Scrollbox(void);

	inline void setVerticalScrollbar(Scrollbartype type);
	inline void setHorizontalScrollbar(Scrollbartype type);
	inline void setContent(Widget* widget);

private:

	uint32_t area_width, area_height;
	Widget* content;
	uint32_t content_width, content_height;
	Vector2 content_scroll;

	Scrollbartype scrollbar_horiz_type, scrollbar_vert_type;
	bool scrollbar_horiz, scrollbar_vert;

	// Virtual functions for Widget
	inline virtual void onChildSizeChange(void);
	inline virtual void onSizeChange(void);
	inline virtual void onEnvironmentUpdated(void);

	// Updates size of content
	inline void updateContent(void);

	inline void updateContentPosition(void);

};

inline Scrollbox::Scrollbox(void) :
area_width(0),
area_height(0),
content(NULL),
content_width(0),
content_height(0),
content_scroll(0, 0),
scrollbar_horiz_type(NEVER),
scrollbar_vert_type(NEVER),
scrollbar_horiz(false),
scrollbar_vert(false)
{
}

inline Scrollbox::~Scrollbox(void)
{
}

inline void Scrollbox::setVerticalScrollbar(Scrollbartype type)
{
	scrollbar_horiz_type = type;
	updateContent();
}

inline void Scrollbox::setHorizontalScrollbar(Scrollbartype type)
{
	scrollbar_vert_type = type;
	updateContent();
}

inline void Scrollbox::setContent(Widget* widget)
{
	content = widget;
	addChild(widget);
	updateContent();
}

inline void Scrollbox::onChildSizeChange(void)
{
	updateContent();
}

inline void Scrollbox::onSizeChange(void)
{
	updateContent();
}

inline void Scrollbox::onEnvironmentUpdated(void)
{
	updateContent();
}

inline void Scrollbox::updateContent(void)
{
	Renderer* rend = getRenderer();
	if (!content || !rend) {
		return;
	}

	// Get real size of area for this Scrollbox
	area_width = getWidth();
	area_height = getHeight();

	scrollbar_horiz = (scrollbar_horiz_type == ALWAYS);
	scrollbar_vert = (scrollbar_horiz_type == ALWAYS);
	if (scrollbar_horiz) area_width -= rend->getScrollbarWidth();
	if (scrollbar_vert) area_height -= rend->getScrollbarHeight();

	bool vertical_scrollbar_added = false;
	do {
		// Calculate width, and check if horizontal scrollbar is needed.
		content_width = std::max(content->getMinWidth(), area_width);
		if (content_width > area_width && !scrollbar_horiz && scrollbar_horiz_type == ON_DEMAND) {
			scrollbar_horiz = true;
			area_height -= rend->getScrollbarHeight();
		}

		// Calculate height, and check if vertical scrollbar is needed.
		// If this brings new scrollbar, then the width must be
		// calculated again, because new scrollbar changes width.
		content_height = content->getMinHeight(content_width);
		if (content_height > area_height && !scrollbar_vert && scrollbar_vert_type == ON_DEMAND) {
			scrollbar_vert = true;
			area_width -= rend->getScrollbarWidth();
			vertical_scrollbar_added = true;
		}

	} while (vertical_scrollbar_added);

	setChildSize(content, content_width, content_height);
	updateContentPosition();

}

inline void Scrollbox::updateContentPosition(void)
{
	if (!content) {
		return;
	}
	int32_t scroll_x, scroll_y;
	// Horizontal position
	if (area_width < content_width) {
		uint32_t extra_width = content_width - area_width;
		scroll_x = extra_width * -content_scroll.x;
	} else {
		scroll_x = 0;
	}
	// Vertical position
	if (area_height < content_height) {
		uint32_t extra_height = content_height - area_height;
		scroll_y = extra_height * -content_scroll.y;
	} else {
		scroll_y = 0;
	}
	setChildPosition(content, scroll_x, scroll_y);
}

}

}

#endif


