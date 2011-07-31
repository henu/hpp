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
class Menubar;
class Menuitembase;

class Menu : public Widget
{

friend class Menubar;

public:

	Menu(void);
	virtual ~Menu(void);

	inline void setLabel(UnicodeString const& label) { this->label = label; markSizeChanged(); }

	inline void addItem(Menuitembase* item) { content.addItem(item); }

private:

	// Called by friend class Menubar
	void updatePosAndSize(Renderer* rend, int32_t x, int32_t y);
	uint32_t getLabelWidth(Renderer* rend);

private:

	Menucontent content;

	UnicodeString label;

	// Virtual functions for Widget
	virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	virtual void onChildSizeChange(void);
	virtual void doRendering(void);

};

}

}

#endif
