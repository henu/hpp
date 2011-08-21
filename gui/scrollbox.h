#ifndef HPP_GUI_SCROLLBOX_H
#define HPP_GUI_SCROLLBOX_H

#include "scrollbar.h"
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
	Scrollbar* scrollbar_horiz;
	Scrollbar* scrollbar_vert;

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
scrollbar_horiz(NULL),
scrollbar_vert(NULL)
{
}

inline Scrollbox::~Scrollbox(void)
{
	delete scrollbar_horiz;
	delete scrollbar_vert;
}

inline void Scrollbox::setVerticalScrollbar(Scrollbartype type)
{
	scrollbar_vert_type = type;
	updateContent();
}

inline void Scrollbox::setHorizontalScrollbar(Scrollbartype type)
{
	scrollbar_horiz_type = type;
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
	Renderer const* rend = getRenderer();
	if (!content || !rend) {
		return;
	}

	// Get real size of area for this Scrollbox
	area_width = getWidth();
	area_height = getHeight();

	bool scrollbar_horiz_needed = (scrollbar_horiz_type == ALWAYS);
	bool scrollbar_vert_needed = (scrollbar_vert_type == ALWAYS);
	if (scrollbar_horiz_needed) area_height -= rend->getScrollbarWidth();
	if (scrollbar_vert_needed) area_width -= rend->getScrollbarHeight();

	bool vertical_scrollbar_added;
	do {
		vertical_scrollbar_added = false;
		// Calculate width, and check if horizontal scrollbar is needed.
		content_width = std::max(content->getMinWidth(), area_width);
		if (content_width > area_width && !scrollbar_horiz_needed && scrollbar_horiz_type == ON_DEMAND) {
			scrollbar_horiz_needed = true;
			area_height -= rend->getScrollbarHeight();
		}

		// Calculate height, and check if vertical scrollbar is needed.
		// If this brings new scrollbar, then the width must be
		// calculated again, because new scrollbar changes width.
		content_height = content->getMinHeight(content_width);
		if (content_height > area_height && !scrollbar_vert_needed && scrollbar_vert_type == ON_DEMAND) {
			scrollbar_vert_needed = true;
			area_width -= rend->getScrollbarWidth();
			vertical_scrollbar_added = true;
		}

	} while (vertical_scrollbar_added);

	// Check if there are two scrollbars. They
	// require little box to bottomleft.
	bool two_scrollbars = (scrollbar_horiz_needed && scrollbar_vert_needed);

	// Check if new scrollbars should be added or removed
	if (!scrollbar_horiz && scrollbar_horiz_needed) {
		scrollbar_horiz = new Scrollbar(Scrollbar::HORIZONTAL);
		addChild(scrollbar_horiz);
	} else if (scrollbar_horiz && !scrollbar_horiz_needed) {
		delete scrollbar_horiz;
		scrollbar_horiz = NULL;
	}
	if (!scrollbar_vert && scrollbar_vert_needed) {
		scrollbar_vert = new Scrollbar(Scrollbar::VERTICAL);
		addChild(scrollbar_vert);
	} else if (scrollbar_vert && !scrollbar_vert_needed) {
		delete scrollbar_vert;
		scrollbar_vert = NULL;
	}

	// Set sizes of scrollbars
	if (scrollbar_horiz) {
		setChildPosition(scrollbar_horiz, 0, getHeight() - rend->getScrollbarHeight());
		uint32_t scrollbar_width = getWidth();
		if (two_scrollbars) scrollbar_width -= rend->getScrollbarWidth();
		setChildSize(scrollbar_horiz, scrollbar_width, rend->getScrollbarHeight());
	}
	if (scrollbar_vert) {
		setChildPosition(scrollbar_vert, getWidth() - rend->getScrollbarWidth(), 0);
		uint32_t scrollbar_height = getHeight();
		if (two_scrollbars) scrollbar_height -= rend->getScrollbarHeight();
		setChildSize(scrollbar_vert, rend->getScrollbarWidth(), scrollbar_height);
	}

	setChildSize(content, content_width, content_height);
	updateContentPosition();

	// Limit renderarea
	setChildRenderarealimit(content, 0, 0, area_width, area_height);

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
