#ifndef HPP_GUI_RENDERER_H
#define HPP_GUI_RENDERER_H

#include "engine.h"

#include "../unicodestring.h"
#include "../path.h"

#include <stdint.h>

namespace Hpp
{

namespace Gui
{

class Menuitem;
class Menuseparator;
class Menubar;
class Menu;
class Window;
class Label;
class Textinput;
class Button;
class Folderview;
class FolderviewContents;

class Renderer
{

	friend class Engine;

public:

	inline Renderer(void);
	inline virtual ~Renderer(void);

	virtual uint32_t getWidth(void) const = 0;
	virtual uint32_t getHeight(void) const = 0;

	virtual void initRendering(void) = 0;
	virtual void deinitRendering(void) = 0;

	// Virtual rendering functions
	virtual void renderMenubarBackground(int32_t x_origin, int32_t y_origin, Menubar const* menubar) = 0;
	virtual void renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep) = 0;
	virtual void renderMenuLabel(int32_t x_origin, int32_t y_origin, Menu const* menu, UnicodeString const& label, bool mouse_over) = 0;
	virtual void renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over) = 0;
	virtual void renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title) = 0;
	virtual void renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str) = 0;
	virtual void renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput, UnicodeString const& value) = 0;
	virtual void renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed) = 0;
	virtual void renderFolderview(int32_t x_origin, int32_t y_origin, Folderview const* folderview) = 0;
	virtual void renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, FolderChildren const& items) = 0;

	// Non-virtual dimension getters
	inline uint32_t getMinimumFolderviewWidth(void) const;
	inline uint32_t getFolderviewHeight(void) const;

	// Virtual dimension getters
	virtual uint32_t getMenubarHeight(void) const = 0;
	virtual uint32_t getMenuLabelWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getMenuseparatorMinWidth(void) const = 0;
	virtual uint32_t getMenuseparatorHeight(void) const = 0;
	virtual uint32_t getMenuitemWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getMenuitemHeight(void) const = 0;
	virtual uint32_t getWindowTitlebarHeight(void) const = 0;
	virtual uint32_t getWindowEdgeLeftWidth(void) const = 0;
	virtual uint32_t getWindowEdgeRightWidth(void) const = 0;
	virtual uint32_t getWindowEdgeBottomHeight(void) const = 0;
	virtual uint32_t getLabelWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getLabelHeight(void) const = 0;
	virtual uint32_t getMinimumTextinputWidth(void) const = 0;
	virtual uint32_t getTextinputHeight(void) const = 0;
	virtual uint32_t getButtonWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getButtonHeight(void) const = 0;
	virtual uint32_t getMinimumFolderviewContentsWidth(void) const = 0;
	virtual uint32_t getFolderviewContentsHeight(void) const = 0;
	virtual void getFolderviewEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const = 0;
	virtual uint32_t getScrollbarWidth(void) const = 0;
	virtual uint32_t getScrollbarHeight(void) const = 0;

private:

	// Called by Engine
	inline void setEngine(Engine* engine) { this->engine = engine; }

protected:

	inline void sizeChanged(void);

private:

	Engine* engine;

};

inline Renderer::Renderer(void) :
engine(NULL)
{
}

inline Renderer::~Renderer(void)
{
	if (engine) {
		engine->rendererIsBeingDestroyed();
	}
}

inline uint32_t Renderer::getMinimumFolderviewWidth(void) const
{
	uint32_t edge_top, edge_left, edge_right, edge_bottom;
	getFolderviewEdgeSizes(edge_top, edge_left, edge_right, edge_bottom);
	return getMinimumFolderviewContentsWidth() + edge_left + edge_right;
}

inline uint32_t Renderer::getFolderviewHeight(void) const
{
	uint32_t edge_top, edge_left, edge_right, edge_bottom;
	getFolderviewEdgeSizes(edge_top, edge_left, edge_right, edge_bottom);
	return getFolderviewContentsHeight() + edge_top + edge_bottom;
}

inline void Renderer::sizeChanged(void)
{
	if (engine) {
		engine->sizeOfRendererChanged();
	}
}

}

}

#endif


