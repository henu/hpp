#ifndef HPP_GUI_TABS_H
#define HPP_GUI_TABS_H

#include "widget.h"

#include "../unicodestring.h"
#include "../exception.h"

namespace Hpp
{

namespace Gui
{

class Tabs : public Widget
{

public:

	inline Tabs(void);
	inline virtual ~Tabs(void);

	inline void addTab(UnicodeString const& label, Widget* widget);

	inline size_t getSelected(void) const { return selected; }
	inline size_t getNumOfTabs(void) const { return tabs.size(); }
	inline UnicodeString getTabLabel(size_t tab_id) const;

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	struct Tab
	{
		UnicodeString label;
		Widget* contents;
	};
	typedef std::vector< Tab > TabV;

	TabV tabs;
	size_t selected;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void doRepositioning(void);

};

inline Tabs::Tabs(void) :
selected(0)
{
}

inline Tabs::~Tabs(void)
{
}

inline void Tabs::addTab(UnicodeString const& label, Widget* widget)
{
	Tab new_tab;
	new_tab.label = label;
	new_tab.contents = widget;
	tabs.push_back(new_tab);
	addChild(widget);
}

inline UnicodeString Tabs::getTabLabel(size_t tab_id) const
{
	if (tab_id >= tabs.size()) {
		throw Exception("Tab ID out of range!");
	}
	return tabs[tab_id].label;
}

inline uint32_t Tabs::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	uint32_t min_width = 0;
// TODO: Support scrollable tab bar!
	for (TabV::const_iterator tabs_it = tabs.begin();
	     tabs_it != tabs.end();
	     tabs_it ++) {
		Tab const& tab = *tabs_it;
		min_width += rend->getTablabelWidth(tab.label);
	}
	for (TabV::const_iterator tabs_it = tabs.begin();
	     tabs_it != tabs.end();
	     tabs_it ++) {
		Tab const& tab = *tabs_it;
		min_width = std::max(min_width, tab.contents->getMinWidth());
	}
	return min_width + rend->getTabsLeftEdgeWidth() + rend->getTabsRightEdgeWidth();
}

inline uint32_t Tabs::getMinHeight(uint32_t width) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	uint32_t contents_width = width - rend->getTabsLeftEdgeWidth() - rend->getTabsRightEdgeWidth();
	uint32_t min_height = 0;
	for (TabV::const_iterator tabs_it = tabs.begin();
	     tabs_it != tabs.end();
	     tabs_it ++) {
		Tab const& tab = *tabs_it;
		min_height = std::max(min_height, tab.contents->getMinHeight(contents_width));
	}
	return min_height + rend->getTabbarHeight() + rend->getTabsBottomEdgeHeight();
}

inline void Tabs::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	rend->renderTabs(x_origin, y_origin, this);
}

inline bool Tabs::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	Renderer* rend = getRenderer();
	if (!rend) return false;

	if (mouse_key == Mousekey::LEFT) {
		if (mouse_y >= 0 && mouse_y < (int32_t)rend->getTabbarHeight()) {
			mouse_x -= rend->getTabsLeftEdgeWidth();
			for (size_t tab_id = 0; tab_id < tabs.size(); tab_id ++) {
				if (mouse_x >= 0) {
					uint32_t label_width = rend->getTablabelWidth(tabs[tab_id].label);
					if (mouse_x < (int32_t)label_width) {
						selected = tab_id;
						markToNeedReposition();
						break;
					}
					mouse_x -= label_width;
				}
			}
		}
	}
	return true;
}

inline void Tabs::doRepositioning(void)
{
	Renderer const* rend = getRenderer();
	if (!rend) return;

	if (tabs.empty()) {
		return;
	}

	// Hide all but the selected tab
	for (size_t tab_id = 0; tab_id < tabs.size(); tab_id ++) {
		// Skip selected tab
		if (tab_id == selected) {
			continue;
		}
		Tab& tab = tabs[tab_id];
		setChildState(tab.contents, HIDDEN);
	}

	HppAssert(selected < tabs.size(), "Tab selection overflow!");
	Widget* selected_widget = tabs[selected].contents;

	// Now calculate position and size of child
	uint32_t child_width = getWidth() - rend->getTabsLeftEdgeWidth() - rend->getTabsRightEdgeWidth();
	uint32_t child_height = getHeight() - rend->getTabbarHeight() - rend->getTabsBottomEdgeHeight();
	int32_t child_pos_x = rend->getTabsLeftEdgeWidth();
	int32_t child_pos_y = rend->getTabbarHeight();
// TODO: What if tab contents wants to be disabled?
	setChildState(selected_widget, ENABLED);
	repositionChild(selected_widget, child_pos_x, child_pos_y, child_width, child_height);
}

}

}

#endif

