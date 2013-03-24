#ifndef HPP_GUI_WINDOWAREA_H
#define HPP_GUI_WINDOWAREA_H

#include "widget.h"

#include <vector>

namespace Hpp
{

namespace Gui
{

class Window;

class Windowarea : public Widget
{

public:

	Windowarea(void);
	virtual ~Windowarea(void);

	void addWindow(Window* window);

private:

	typedef std::vector< Window* > Windows;

	Windows windows;

	// Virtual functions for Widget
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual bool onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	virtual void doRepositioning(void);

};

inline bool Windowarea::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	(void)mouse_key;
	return false;
}

inline bool Windowarea::onMouseKeyUp(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	(void)mouse_key;
	return false;
}

}

}

#endif


