#ifndef HPP_GUI_AREAWITHMENUBAR_H
#define HPP_GUI_AREAWITHMENUBAR_H

#include "widget.h"

#include <vector>
#include <stdint.h>

namespace Hpp
{

namespace Gui
{

class Menu;

class AreaWithMenubar : public Widget
{

	friend class Engine;

public:

	AreaWithMenubar(void);
	virtual ~AreaWithMenubar(void);

	void addMenu(Menu* menu);

	void setContent(Widget* widget);

	virtual uint32_t getMinWidth(void) const;
	virtual uint32_t getMinHeight(uint32_t width) const;

private:

	typedef std::vector< Menu* > Menus;

	Menus menus;

	Widget* content;

	// Virtual functions for Widget
	virtual void doRendering(int32_t x_origin, int32_t y_origin);
	virtual void onChildRemoved(Widget* child);

	virtual void doRepositioning(void);

};

}

}

#endif
