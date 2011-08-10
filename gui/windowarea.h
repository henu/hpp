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

	friend class Window;

public:

	Windowarea(void);
	virtual ~Windowarea(void);

	void addWindow(Window* window);

private:

	// Called by friend class Window
	void windowPositionChanged(Window* window, int32_t rel_x, int32_t rel_y);
	void getSize(uint32_t& width, uint32_t& height) { width = getWidth(); height = getHeight(); }

private:

	typedef std::vector< Window* > Windows;

	Windows windows;

};

}

}

#endif


