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

	typedef std::vector< Menu* > Menus;

	Menus menus;

	// Virtual functions for Widget
	virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual void onChildSizeChange(void) { updateSizeAndPositionOfMenus(); }
	inline virtual void onSizeChange(void) { updateSizeAndPositionOfMenus(); }

	void updateSizeAndPositionOfMenus(void);

};

}

}

#endif
