#ifndef HPP_GUI_NCURSESRENDERER_H
#define HPP_GUI_NCURSESRENDERER_H

#include "renderer.h"
#include "../ncios.h"

namespace Hpp
{

namespace Gui
{

class NCursesRenderer : public Renderer
{

public:

	NCursesRenderer(void);
	virtual ~NCursesRenderer(void);

	// Helper functions for writing stuff to screen. It is not
	// allowed to print characters with ASCII code less than 0x20.
	void setCursor(int32_t col, int32_t row);
	void setColors(NC::Color text, NC::Color bg);
	void printChar(char c);
	void printString(UnicodeString const& str);
	inline void printString(std::string const& str) { printString(UnicodeString(str)); }

	virtual void renderTextCursor(int32_t x_origin, int32_t y_origin);

private:

	// Position of drawing cursor
	int32_t cursor_col, cursor_row;

	// Renderarea limit
	bool limit;
	uint32_t limit_x, limit_y;
	uint32_t limit_w, limit_h;

	bool textcursor_visible;
	int32_t textcursor_col;
	int32_t textcursor_row;

	void positionCursorIfAtRenderarea(void);

	void advanceCursor(size_t amount);

	size_t getVisibleLeft(void) const;
	size_t getHiddenLeft(void) const;

	bool isAtRenderarea(int32_t col, int32_t row) const;

	// Tries to hide textcursor by printing specific amount
	// of characters from current print cursor.
	void hideTextCursor(size_t chars);

	// Virtual functions required by superclass Renderer
	virtual uint32_t getWidth(void) const;
	virtual uint32_t getHeight(void) const;
	virtual void initRendering(void);
	virtual void deinitRendering(void);
	virtual void renderMenubarBackground(int32_t x_origin, int32_t y_origin, uint32_t width, AreaWithMenubar const* areawithmenubar);
	virtual void renderMenuLabel(int32_t x_origin, int32_t y_origin, Menu const* menu, UnicodeString const& label, bool mouse_over);
	virtual void renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep);
	virtual void renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over);
	virtual void renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title);
	virtual void renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str);
	virtual void renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput);
	virtual void renderTextinputContents(int32_t x_origin, int32_t y_origin, TextinputContents const* textinputcontents, ssize_t cursor);
	virtual void renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed);
	virtual void renderFolderview(int32_t x_origin, int32_t y_origin, Folderview const* folderview);
	virtual void renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, Path::DirChildren const& items);
	virtual void renderScrollbar(int32_t x_origin, int32_t y_origin, Scrollbar const* scrollbar, bool horizontal, bool up_or_left_key_pressed, bool down_or_right_key_pressed, bool slider_pressed);
	virtual void renderSlider(int32_t x_origin, int32_t y_origin, Slider const* slider, bool horizontal, bool slider_pressed);
	virtual void renderTabs(int32_t x_origin, int32_t y_origin, Tabs const* tabs);
	inline virtual uint32_t getMenubarHeight(void) const { return 1; }
	inline virtual uint32_t getMenuLabelWidth(UnicodeString const& label) const { return label.size() + 2; }
	inline virtual uint32_t getMenuseparatorMinWidth(void) const { return 1; }
	inline virtual uint32_t getMenuseparatorHeight(void) const { return 1; }
	inline virtual uint32_t getMenuitemWidth(UnicodeString const& label) const { return label.size(); }
	inline virtual uint32_t getMenuitemHeight(void) const { return 1; }
	inline virtual uint32_t getWindowTitlebarHeight(void) const { return 1; }
	inline virtual uint32_t getWindowEdgeTopHeight(void) const { return 0; }
	inline virtual uint32_t getWindowEdgeLeftWidth(void) const { return 0; }
	inline virtual uint32_t getWindowEdgeRightWidth(void) const { return 0; }
	inline virtual uint32_t getWindowEdgeBottomHeight(void) const { return 0; }
	inline virtual uint32_t getWindowDragcornerSize(void) const { return 0; }
	inline virtual uint32_t getLabelWidth(UnicodeString const& label) const { return label.size(); }
	inline virtual uint32_t getLabelHeight(size_t lines) const { return lines; }
	inline virtual uint32_t getTextinputWidth(size_t cols) const { return cols; }
	inline virtual uint32_t getTextinputHeight(void) const { return 1; }
	inline virtual uint32_t getMinimumTextinputContentsWidth(UnicodeString const& value) const { (void)value; return 1; }
	inline virtual uint32_t getTextinputContentsHeight(void) const { return 1; }
	inline virtual void getTextinputContentsCursorProps(uint32_t& cursor_pos_x, uint32_t& cursor_pos_y, uint32_t& cursor_width, uint32_t& cursor_height, UnicodeString const& value, ssize_t cursor) const { (void)value; cursor_pos_x = cursor; cursor_pos_y = 0; cursor_width = 1; cursor_height = 1; }
	inline virtual void getTextinputEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const { edge_top = 0; edge_left = 0; edge_right = 0; edge_bottom = 0; }
	inline virtual uint32_t getButtonWidth(UnicodeString const& label) const { return label.size(); }
	inline virtual uint32_t getButtonHeight(void) const { return 1; }
	inline virtual uint32_t getMinimumFolderviewWidth(void) const { return 0; }
	inline virtual uint32_t getFolderviewHeight(void) const { return 1; }
	inline virtual uint32_t getMinimumFolderviewContentsWidth(UnicodeString const& label) const { (void)label; return 0; }
	inline virtual uint32_t getFolderviewContentsHeight(size_t items) const { (void)items; return 1; }
	inline virtual void getFolderviewEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const { edge_top = 0; edge_left = 0; edge_right = 0; edge_bottom = 0; }
	inline virtual uint32_t getScrollbarWidth(void) const { return 1; }
	inline virtual uint32_t getScrollbarHeight(void) const { return 1; }
	inline virtual uint32_t getScrollbarButtonLeftWidth(void) const { return 1; }
	inline virtual uint32_t getScrollbarButtonRightWidth(void) const { return 1; }
	inline virtual uint32_t getScrollbarButtonUpHeight(void) const { return 1; }
	inline virtual uint32_t getScrollbarButtonDownHeight(void) const { return 1; }
	inline virtual uint32_t getScrollboxMinWidth(void) const { return 2; }
	inline virtual uint32_t getScrollboxMinHeight(void) const { return 2; }
	inline virtual uint32_t getHorizSliderWidth(void) const { return 1; }
	inline virtual uint32_t getVertSliderHeight(void) const { return 1; }
	inline virtual uint32_t getHorizSliderMinWidth(void) const { return 2; }
	inline virtual uint32_t getVertSliderMinHeight(void) const { return 2; }
	inline virtual uint32_t getHorizSliderHeight(void) const { return 1; }
	inline virtual uint32_t getVertSliderWidth(void) const { return 1; }
	inline virtual uint32_t getTabbarHeight(void) const { return 1; }
	inline virtual uint32_t getTabsLeftEdgeWidth(void) const { return 0; }
	inline virtual uint32_t getTabsRightEdgeWidth(void) const { return 0; }
	inline virtual uint32_t getTabsBottomEdgeHeight(void) const { return 0; }
	inline virtual uint32_t getTablabelWidth(UnicodeString const& label) const { return label.size() + 2; }
	virtual void setRenderareaLimit(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	virtual void removeRenderareaLimit(void);

};

}

}

#endif
