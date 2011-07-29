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
class Renderer;

class Menubar : public Widget
{

	friend class Engine;

public:

	Menubar(void);
	virtual ~Menubar(void);

	void addMenu(Menu* menu);

private:

	// Called by friend class Engine
	void updatePosAndSize(Renderer* rend, int32_t x, int32_t y, uint32_t width);
	void render(Renderer* rend);

private:

	typedef std::vector< Menu* > Menus;

	Menus menus;

};

}

}

#endif
