#ifndef HPP_GUI_MENUBAR_H
#define HPP_GUI_MENUBAR_H

#include "widget.h"

#include <vector>
#include <stdint.h>

namespace Hpp
{

namespace Gui
{

class Menu;

class Menubar : public Widget
{

	friend class Engine;

public:

	Menubar(void);
	virtual ~Menubar(void);

	void addMenu(Menu* menu);

private:

	// Called by friend class Engine
	void updatePosAndSize(int32_t x, int32_t y, uint32_t width);

private:

	typedef std::vector< Menu* > Menus;

	Menus menus;

	// Virtual functions for Widget
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key) { (void)mouse_x; (void)mouse_y; (void)mouse_key; return true; }
	inline virtual bool onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key) { (void)mouse_x; (void)mouse_y; (void)mouse_key; return true; }
	virtual void doRendering(void);

};

}

}

#endif
