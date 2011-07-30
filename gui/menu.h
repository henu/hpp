#ifndef HPP_GUI_MENU_H
#define HPP_GUI_MENU_H

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
	~Menu(void);

// TODO: Mark size changed!
	inline void setLabel(UnicodeString const& label) { this->label = label; }

// TODO: Mark size changed!
	inline void addItem(Menuitembase* item) { items.push_back(item); }

private:

	// Called by friend class Menubar
	void updatePosAndSize(Renderer* rend, int32_t x, int32_t y);
	uint32_t getLabelWidth(Renderer* rend);

private:

	typedef std::vector< Menuitembase* > Menuitems;

	UnicodeString label;

	Menuitems items;

	// Virtual functions for Widget
	virtual void doRendering(Renderer* rend);

};

}

}

#endif
