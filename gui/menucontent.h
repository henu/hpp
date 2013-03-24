#ifndef HPP_GUI_MENUCONTENT_H
#define HPP_GUI_MENUCONTENT_H

#include "menuitembase.h"
#include "widget.h"

#include <vector>

namespace Hpp
{

namespace Gui
{

class Menucontent : public Widget
{

	friend class Menu;
	friend class Menuitem;

public:

	Menucontent(void);
	virtual ~Menucontent(void);

private:

	// Called by friend class Menu
	void hide(void);
	void reveal(void);

	// Called by friend class Menu
	inline void addItem(Menuitembase* item) { items.push_back(item); addChild(item); markToNeedReposition(); }

	// Called by friend class Menuitem
	void close(void);

private:

	typedef std::vector< Menuitembase* > Menuitems;

	Menuitems items;

	// Virtual functions for Widget
	virtual uint32_t getMinWidth(void) const;
	virtual uint32_t getMinHeight(uint32_t width) const;

	virtual void doRepositioning(void);

};

}

}

#endif

