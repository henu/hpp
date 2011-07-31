#ifndef HPP_GUI_MENUSEPARATOR_H
#define HPP_GUI_MENUSEPARATOR_H

#include "menuitembase.h"

namespace Hpp
{

namespace Gui
{

class Menuseparator : public Menuitembase
{

public:

	Menuseparator(void);
	virtual ~Menuseparator(void);

private:

	// Virtual functions for Widget
	virtual uint32_t getMaxWidth(void) const;
	virtual uint32_t getMinHeight(uint32_t width) const;
	virtual void doRendering(void);

};

}

}

#endif
