#ifndef HPP_GUI_MENUITEM_H
#define HPP_GUI_MENUITEM_H

#include "menuitembase.h"
#include "callback.h"

#include "../unicodestring.h"

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

	inline void setCallbackFunc(CallbackFunc callback, void* data);

private:

	UnicodeString label;

	CallbackFunc callback;
	void* callback_data;

	// Virtual functions for Widget
	virtual uint32_t getMaxWidth(void) const;
	virtual uint32_t getMinHeight(uint32_t width) const;
	virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	virtual void onMouseKeyUpOther(Widget* widget, int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	virtual void doRendering(void);

};

inline void Menuitem::setCallbackFunc(CallbackFunc callback, void* data)
{
	this->callback = callback;
	callback_data = data;
}

}

}

#endif
