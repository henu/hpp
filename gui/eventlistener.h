#ifndef HPP_GUI_EVENTLISTENER_H
#define HPP_GUI_EVENTLISTENER_H

#include "guievent.h"

namespace Hpp
{

namespace Gui
{

class Widget;

class Eventlistener
{

	friend class Widget;

private:

	virtual bool handleGuiEvent(GuiEvent const& event) = 0;

};

}

}

#endif
