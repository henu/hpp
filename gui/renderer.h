#ifndef HPP_GUI_RENDERER_H
#define HPP_GUI_RENDERER_H

#include "engine.h"

#include "../unicodestring.h"
#include "../path.h"
#include "../color.h"

#include <stdint.h>

namespace Hpp
{

class Texture;

namespace Gui
{

class Menuitem;
class Menuseparator;
class AreaWithMenubar;
class Menu;
class Window;
class Label;
class Textinput;
class TextinputContents;
class Button;
class Folderview;
class FolderviewContents;
class Scrollbar;
class Slider;
class Tabs;

class Renderer
{

	friend class Engine;

	virtual void renderTextCursor(int32_t x_origin, int32_t y_origin) = 0;

public:

	inline Renderer(void);
	inline virtual ~Renderer(void);

	virtual uint32_t getWidth(void) const = 0;
	virtual uint32_t getHeight(void) const = 0;

	virtual void initRendering(void) = 0;
	virtual void deinitRendering(void) = 0;

	// Virtual rendering functions
	virtual void renderMenubarBackground(int32_t x_origin, int32_t y_origin, uint32_t width, AreaWithMenubar const* areawithmenubar) = 0;
	virtual void renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep) = 0;
	virtual void renderMenuLabel(int32_t x_origin, int32_t y_origin, Menu const* menu, UnicodeString const& label, bool mouse_over) = 0;
	virtual void renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over) = 0;
	virtual void renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title) = 0;
	virtual void renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str) = 0;
	virtual void renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput) = 0;
	virtual void renderTextinputContents(int32_t x_origin, int32_t y_origin, TextinputContents const* textinputcontents, ssize_t cursor) = 0;
	virtual void renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed) = 0;
	virtual void renderFolderview(int32_t x_origin, int32_t y_origin, Folderview const* folderview) = 0;
	virtual void renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, Path::DirChildren const& items) = 0;
	virtual void renderScrollbar(int32_t x_origin, int32_t y_origin, Scrollbar const* scrollbar, bool horizontal, bool up_or_left_key_pressed, bool down_or_right_key_pressed, bool slider_pressed) = 0;
	virtual void renderSlider(int32_t x_origin, int32_t y_origin, Slider const* slider, bool horizontal, bool slider_pressed) = 0;
	virtual void renderTabs(int32_t x_origin, int32_t y_origin, Tabs const* tabs) = 0;
	virtual void renderBackground(int32_t x_origin, int32_t y_origin, int32_t width, int32_t height, Texture* tex, Color const& color) = 0;

	virtual uint32_t getMenubarHeight(void) const = 0;
	virtual uint32_t getMenuLabelWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getMenuseparatorMinWidth(void) const = 0;
	virtual uint32_t getMenuseparatorHeight(void) const = 0;
	virtual uint32_t getMenuitemWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getMenuitemHeight(void) const = 0;
	virtual uint32_t getWindowTitlebarHeight(void) const = 0;
	virtual uint32_t getWindowEdgeTopHeight(void) const = 0;
	virtual uint32_t getWindowEdgeLeftWidth(void) const = 0;
	virtual uint32_t getWindowEdgeRightWidth(void) const = 0;
	virtual uint32_t getWindowEdgeBottomHeight(void) const = 0;
	virtual uint32_t getWindowDragcornerSize(void) const = 0;
	virtual uint32_t getLabelWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getLabelHeight(size_t lines) const = 0;
	virtual uint32_t getTextinputWidth(size_t cols) const = 0;
	virtual uint32_t getTextinputHeight(void) const = 0;
	virtual uint32_t getMinimumTextinputContentsWidth(UnicodeString const& value) const = 0;
	virtual uint32_t getTextinputContentsHeight(void) const = 0;
	virtual void getTextinputContentsCursorProps(uint32_t& cursor_pos_x, uint32_t& cursor_pos_y, uint32_t& cursor_width, uint32_t& cursor_height, UnicodeString const& value, ssize_t cursor) const = 0;
	virtual void getTextinputEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const = 0;
	virtual uint32_t getButtonWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getButtonHeight(void) const = 0;
	virtual uint32_t getMinimumFolderviewWidth(void) const = 0;
	virtual uint32_t getFolderviewHeight(void) const = 0;
	virtual uint32_t getMinimumFolderviewContentsWidth(UnicodeString const& label) const = 0;
	virtual uint32_t getFolderviewContentsHeight(size_t items) const = 0;
	virtual void getFolderviewEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const = 0;
	virtual uint32_t getScrollbarWidth(void) const = 0;
	virtual uint32_t getScrollbarHeight(void) const = 0;
	virtual uint32_t getScrollbarButtonLeftWidth(void) const = 0;
	virtual uint32_t getScrollbarButtonRightWidth(void) const = 0;
	virtual uint32_t getScrollbarButtonUpHeight(void) const = 0;
	virtual uint32_t getScrollbarButtonDownHeight(void) const = 0;
	virtual uint32_t getScrollboxMinWidth(void) const = 0;
	virtual uint32_t getScrollboxMinHeight(void) const = 0;
	virtual uint32_t getHorizSliderWidth(void) const = 0;
	virtual uint32_t getVertSliderHeight(void) const = 0;
	virtual uint32_t getHorizSliderMinWidth(void) const = 0;
	virtual uint32_t getVertSliderMinHeight(void) const = 0;
	virtual uint32_t getHorizSliderHeight(void) const = 0;
	virtual uint32_t getVertSliderWidth(void) const = 0;
	virtual uint32_t getTabbarHeight(void) const = 0;
	virtual uint32_t getTabsLeftEdgeWidth(void) const = 0;
	virtual uint32_t getTabsRightEdgeWidth(void) const = 0;
	virtual uint32_t getTabsBottomEdgeHeight(void) const = 0;
	virtual uint32_t getTablabelWidth(UnicodeString const& label) const = 0;

private:

	// Called by Engine
	inline void setEngine(Engine* engine) { this->engine = engine; }

	// Rendering area limitations. Called by Engine.
	virtual void setRenderareaLimit(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void removeRenderareaLimit(void) = 0;

protected:

	inline void sizeChanged(void);

	inline Engine* getEngine(void) { return engine; }

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

inline void Renderer::sizeChanged(void)
{
	if (engine) {
		engine->sizeOfRendererChanged();
	}
}

}

}

#endif


