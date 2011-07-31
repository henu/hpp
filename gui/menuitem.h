#ifndef HPP_GUI_MENUITEM_H
#define HPP_GUI_MENUITEM_H

#include "../unicodestring.h"

#include "menuitembase.h"

namespace Hpp
{

namespace Gui
{

class Menuitem : public Menuitembase
{

public:

	Menuitem(void);
	virtual ~Menuitem(void);

	inline void setLabel(UnicodeString const& label) { this->label = label; markSizeChanged(); }

private:

	UnicodeString label;

	// Virtual functions for Widget
	virtual uint32_t getMaxWidth(void) const;
	virtual uint32_t getMinHeight(uint32_t width) const;
	virtual void doRendering(void);

};

}

}

#endif
