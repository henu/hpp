#ifndef HPP_GUI_SCROLLBOX_H
#define HPP_GUI_SCROLLBOX_H

#include "scrollbar.h"
#include "widget.h"

#include "../vector2.h"

namespace Hpp
{

namespace Gui
{

class Scrollbox : public Widget, public Eventlistener
{

public:

	enum Scrollbartype { NEVER, ON_DEMAND, ALWAYS };

	inline Scrollbox(void);
	inline virtual ~Scrollbox(void);

	inline void setHorizontalScrollbar(Scrollbartype type);
	inline void setVerticalScrollbar(Scrollbartype type);
	inline void setHorizontalScrollbarButtonmove(uint32_t amount);
	inline void setVerticalScrollbarButtonmove(uint32_t amount);
	inline void setContent(Widget* widget);

	// Scrolls using
	inline void scrollHorizontallyAsButtons(Real amount);
	inline void scrollVerticallyAsButtons(Real amount);

	// Tries to scroll view so that given area becomes visible. If
	// are does not fit to view, then it centers area to view.
	inline void scrollTo(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	uint32_t area_width, area_height;
	Widget* content;
	uint32_t content_width, content_height;
	Vector2 content_scroll;

	Scrollbartype scrollbar_horiz_type, scrollbar_vert_type;
	Scrollbar* scrollbar_horiz;
	Scrollbar* scrollbar_vert;
	uint32_t scrollbar_horiz_buttonmove;
	uint32_t scrollbar_vert_buttonmove;

	// Virtual functions for Widget
	inline virtual void onChildSizeChange(void);
	inline virtual void onSizeChange(void);
	inline virtual void onEnvironmentUpdated(void);

	// Updates size of content
	inline void updateContent(void);

	inline void updateContentPosition(void);

	inline virtual bool handleGuiEvent(GuiEvent const& event);

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
scrollbar_vert(NULL),
scrollbar_horiz_buttonmove(1),
scrollbar_vert_buttonmove(1)
{
}

inline Scrollbox::~Scrollbox(void)
{
	delete scrollbar_horiz;
	delete scrollbar_vert;
}

inline void Scrollbox::setHorizontalScrollbar(Scrollbartype type)
{
	scrollbar_horiz_type = type;
	updateContent();
}

inline void Scrollbox::setVerticalScrollbar(Scrollbartype type)
{
	scrollbar_vert_type = type;
	updateContent();
}

inline void Scrollbox::setHorizontalScrollbarButtonmove(uint32_t amount)
{
	scrollbar_horiz_buttonmove = amount;
	if (scrollbar_horiz) {
		scrollbar_horiz->setButtonMove(amount);
	}
}

inline void Scrollbox::setVerticalScrollbarButtonmove(uint32_t amount)
{
	scrollbar_vert_buttonmove = amount;
	if (scrollbar_vert) {
		scrollbar_vert->setButtonMove(amount);
	}
}

inline void Scrollbox::setContent(Widget* widget)
{
	content = widget;
	addChild(widget);
	updateContent();
}

inline void Scrollbox::scrollHorizontallyAsButtons(Real amount)
{
	content_scroll.x += (amount * scrollbar_horiz_buttonmove) / (content_width - area_width);
	if (content_scroll.x < 0.0) content_scroll.x = 0.0;
	if (content_scroll.x > 1.0) content_scroll.x = 1.0;
	if (scrollbar_horiz) {
		scrollbar_horiz->setValue(content_scroll.x);
	}
	updateContentPosition();
}

inline void Scrollbox::scrollVerticallyAsButtons(Real amount)
{
	content_scroll.y += (amount * scrollbar_vert_buttonmove) / (content_height - area_height);
	if (content_scroll.y < 0.0) content_scroll.y = 0.0;
	if (content_scroll.y > 1.0) content_scroll.y = 1.0;
	if (scrollbar_vert) {
		scrollbar_vert->setValue(content_scroll.y);
	}
	updateContentPosition();
}

inline void Scrollbox::scrollTo(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	// X
	if (area_width < content_width) {
		if (width > area_width) {
			x += (area_width - width) / 2;
			width = area_width;
		}
		if ((content_width - area_width) * content_scroll.x > x) {
			content_scroll.x = x / (Real)(content_width - area_width);
		}
		if ((content_width - area_width) * content_scroll.x + area_width < x + width) {
			content_scroll.x = (x + width - area_width) / (Real)(content_width - area_width);
		}
	}
	// Y
	if (area_height < content_height) {
		if (height > area_height) {
			y += (area_height - height) / 2;
			height = area_height;
		}
		if ((content_height - area_height) * content_scroll.y > y) {
			content_scroll.y = y / (Real)(content_height - area_height);
		}
		if ((content_height - area_height) * content_scroll.y + area_height < y + height) {
			content_scroll.y = (y + height - area_height) / (Real)(content_height - area_height);
		}
	}
	updateContentPosition();
}

inline uint32_t Scrollbox::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getScrollboxMinWidth();
}

inline uint32_t Scrollbox::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getScrollboxMinHeight();
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

	content_height = std::max(content_height, area_height);

	// Check if there are two scrollbars. They
	// require little box to bottomleft.
	bool two_scrollbars = (scrollbar_horiz_needed && scrollbar_vert_needed);

	// Check if new scrollbars should be added or removed
	if (!scrollbar_horiz && scrollbar_horiz_needed) {
		scrollbar_horiz = new Scrollbar(Scrollbar::HORIZONTAL);
		scrollbar_horiz->setEventlistener(this);
		addChild(scrollbar_horiz);
	} else if (scrollbar_horiz && !scrollbar_horiz_needed) {
		delete scrollbar_horiz;
		scrollbar_horiz = NULL;
	}
	if (!scrollbar_vert && scrollbar_vert_needed) {
		scrollbar_vert = new Scrollbar(Scrollbar::VERTICAL);
		scrollbar_vert->setEventlistener(this);
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
		// Slider
		if (area_width > content_width) {
			scrollbar_horiz->setSliderSize(1.0);
		} else {
			scrollbar_horiz->setSliderSize(area_width / (Real)content_width);
			scrollbar_horiz->setButtonMove(scrollbar_horiz_buttonmove / (Real)content_width);
		}
	}
	if (scrollbar_vert) {
		setChildPosition(scrollbar_vert, getWidth() - rend->getScrollbarWidth(), 0);
		uint32_t scrollbar_height = getHeight();
		if (two_scrollbars) scrollbar_height -= rend->getScrollbarHeight();
		setChildSize(scrollbar_vert, rend->getScrollbarWidth(), scrollbar_height);
		// Slider
		if (area_height > content_height) {
			scrollbar_vert->setSliderSize(1.0);
		} else {
			scrollbar_vert->setSliderSize(area_height / (Real)content_height);
			scrollbar_vert->setButtonMove(scrollbar_vert_buttonmove / (Real)content_height);
		}
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

inline bool Scrollbox::handleGuiEvent(GuiEvent const& event)
{
	if (scrollbar_horiz) {
		content_scroll.x = scrollbar_horiz->getValue();
	}
	if (scrollbar_vert) {
		content_scroll.y = scrollbar_vert->getValue();
	}
	updateContentPosition();
	return false;
}

}

}

#endif
