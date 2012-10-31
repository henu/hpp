#ifndef HPP_GUI_AREAWITHMENUBAR_H
#define HPP_GUI_AREAWITHMENUBAR_H

#include "container.h"

#include <vector>
#include <stdint.h>

namespace Hpp
{

namespace Gui
{

class Menu;

class AreaWithMenubar : public Container
{

	friend class Engine;

public:

	AreaWithMenubar(void);
	virtual ~AreaWithMenubar(void);

	void addMenu(Menu* menu);

	void setContent(Containerwidget* widget);

	virtual uint32_t getMinWidth(void) const;
	virtual uint32_t getMinHeight(uint32_t width) const;

private:

	typedef std::vector< Menu* > Menus;

	Menus menus;

	Containerwidget* content;

	// Virtual functions for Widget
	virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual void onSizeChange(void) { updateWidgetSizesAndPositions(); }
	inline virtual void onChildSizeChange(void) { updateWidgetSizesAndPositions(); }
	virtual void onChildRemoved(Widget* child);
	inline virtual void onEnvironmentUpdated(void) { updateWidgetSizesAndPositions(); }

	void updateWidgetSizesAndPositions(void);

};

}

}

#endif
