#ifndef HPP_GUI_MENU_H
#define HPP_GUI_MENU_H

#include "menucontent.h"
#include "widget.h"

#include "../unicodestring.h"

#include <vector>

namespace Hpp
{

namespace Gui
{

class Renderer;
class Menuitembase;

class Menu : public Widget
{

public:

	Menu(void);
	virtual ~Menu(void);

	inline void setLabel(UnicodeString const& label) { this->label = label; markSizeChanged(); }

	inline void addItem(Menuitembase* item) { content.addItem(item); }

	// Virtual functions for Widget
	virtual uint32_t getMaxWidth(void) const;
	virtual uint32_t getMinHeight(uint32_t width) const;

private:

	Menucontent content;

	UnicodeString label;

	// Virtual functions for Widget
	virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	virtual void onMouseKeyDownOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	virtual void onChildSizeChange(void);
	virtual void doRendering(int32_t x_origin, int32_t y_origin);

};

}

}

#endif