#ifndef HPP_GUI_RENDERER_H
#define HPP_GUI_RENDERER_H

#include "../unicodestring.h"

#include <stdint.h>

namespace Hpp
{

namespace Gui
{

class Menubar;
class Menu;

class Renderer
{

public:

	inline Renderer(void) { }
	inline virtual ~Renderer(void) { }

	virtual uint32_t getWidth(void) const = 0;
	virtual uint32_t getHeight(void) const = 0;

	virtual void initRendering(void) = 0;
	virtual void deinitRendering(void) = 0;

	// Rendering functions
	virtual void renderMenubarBackground(Menubar const* menubar) = 0;
	virtual void renderMenuLabel(Menu const* menu, UnicodeString const& label) = 0;

	// Some getters
	virtual uint32_t getMenubarHeight(void) = 0;
	virtual uint32_t getMenuLabelWidth(UnicodeString const& label) = 0;

private:

};

}

}

#endif


