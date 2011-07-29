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

	inline void setLabel(UnicodeString const& label) { this->label = label; }

private:

	UnicodeString label;

};

}

}

#endif
