#ifndef HPP_GUI_VIEWPORTRENDERER_H
#define HPP_GUI_VIEWPORTRENDERER_H

#include "renderer.h"

#include "../texture.h"
#include "../font.h"

namespace Hpp
{

class Viewport;

namespace Gui
{

class ViewportRenderer : public Renderer
{

public:

	ViewportRenderer(Viewport const* viewport = NULL);
	virtual ~ViewportRenderer(void);

	// Font size setting
	void setFontSize(uint32_t font_size);

	// Texture loading functions
	inline void loadTextureMenubarBg(Path const& path) { tex_menubar_bg.loadFromFile(path, DEFAULT); }
	inline void loadTextureMenuseparator(Path const& path) { tex_menuseparator.loadFromFile(path, DEFAULT); }
	inline void loadTextureMenuseparatorLeftend(Path const& path) { tex_menuseparator_leftend.loadFromFile(path, DEFAULT); }
	inline void loadTextureMenuseparatorRightend(Path const& path) { tex_menuseparator_rightend.loadFromFile(path, DEFAULT); }
	inline void loadTextureMenuitemBg(Path const& path) { tex_menuitem_bg.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowBg(Path const& path) { tex_window_bg.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowEdgeTop(Path const& path) { tex_window_edge_top.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowEdgeTopLeft(Path const& path) { tex_window_edge_topleft.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowEdgeTopRight(Path const& path) { tex_window_edge_topright.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowEdgeLeft(Path const& path) { tex_window_edge_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowEdgeRight(Path const& path) { tex_window_edge_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowEdgeBottom(Path const& path) { tex_window_edge_bottom.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowEdgeBottomLeft(Path const& path) { tex_window_edge_bottomleft.loadFromFile(path, DEFAULT); }
	inline void loadTextureWindowEdgeBottomRight(Path const& path) { tex_window_edge_bottomright.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldBg(Path const& path) { tex_field_bg.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldEdgeTop(Path const& path) { tex_field_edge_top.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldEdgeTopLeft(Path const& path) { tex_field_edge_topleft.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldEdgeTopRight(Path const& path) { tex_field_edge_topright.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldEdgeLeft(Path const& path) { tex_field_edge_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldEdgeRight(Path const& path) { tex_field_edge_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldEdgeBottom(Path const& path) { tex_field_edge_bottom.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldEdgeBottomLeft(Path const& path) { tex_field_edge_bottomleft.loadFromFile(path, DEFAULT); }
	inline void loadTextureFieldEdgeBottomRight(Path const& path) { tex_field_edge_bottomright.loadFromFile(path, DEFAULT); }
	inline void loadTextureButtonLeft(Path const& path) { tex_button_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureButton(Path const& path) { tex_button.loadFromFile(path, DEFAULT); }
	inline void loadTextureButtonRight(Path const& path) { tex_button_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureButtonPressedLeft(Path const& path) { tex_button_pressed_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureButtonPressed(Path const& path) { tex_button_pressed.loadFromFile(path, DEFAULT); }
	inline void loadTextureButtonPressedRight(Path const& path) { tex_button_pressed_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureFolder(Path const& path) { tex_folder.loadFromFile(path, DEFAULT); }
	inline void loadTextureFile(Path const& path) { tex_file.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarHoriz(Path const& path) { tex_scrollbar_horiz.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarHorizLeft(Path const& path) { tex_scrollbar_horiz_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarHorizRight(Path const& path) { tex_scrollbar_horiz_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarVert(Path const& path) { tex_scrollbar_vert.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarVertTop(Path const& path) { tex_scrollbar_vert_top.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarVertBottom(Path const& path) { tex_scrollbar_vert_bottom.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarPressedHoriz(Path const& path) { tex_scrollbar_pressed_horiz.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarPressedHorizLeft(Path const& path) { tex_scrollbar_pressed_horiz_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarPressedHorizRight(Path const& path) { tex_scrollbar_pressed_horiz_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarPressedVert(Path const& path) { tex_scrollbar_pressed_vert.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarPressedVertTop(Path const& path) { tex_scrollbar_pressed_vert_top.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarPressedVertBottom(Path const& path) { tex_scrollbar_pressed_vert_bottom.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarButtonUp(Path const& path) { tex_scrollbar_button_up.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarButtonLeft(Path const& path) { tex_scrollbar_button_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarButtonRight(Path const& path) { tex_scrollbar_button_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarButtonDown(Path const& path) { tex_scrollbar_button_down.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarButtonPressedUp(Path const& path) { tex_scrollbar_button_pressed_up.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarButtonPressedLeft(Path const& path) { tex_scrollbar_button_pressed_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarButtonPressedRight(Path const& path) { tex_scrollbar_button_pressed_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarButtonPressedDown(Path const& path) { tex_scrollbar_button_pressed_down.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarBgHoriz(Path const& path) { tex_scrollbar_bg_horiz.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarBgHorizLeft(Path const& path) { tex_scrollbar_bg_horiz_left.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarBgHorizRight(Path const& path) { tex_scrollbar_bg_horiz_right.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarBgVert(Path const& path) { tex_scrollbar_bg_vert.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarBgVertTop(Path const& path) { tex_scrollbar_bg_vert_top.loadFromFile(path, DEFAULT); }
	inline void loadTextureScrollbarBgVertBottom(Path const& path) { tex_scrollbar_bg_vert_bottom.loadFromFile(path, DEFAULT); }

	// Font loading functions. Multiple fonts can be used. The firstly
	// loaded font will always be preferred, but if some character is not
	// found from there, then it is loaded from second font, and if its
	// missing there too, then third is tried and so on.
	void loadFont(Path const& path);

	inline void setViewport(Viewport const* viewport) { this->viewport = viewport; sizeChanged(); }

private:

	enum Alignment { LEFT, RIGHT, TOP, BOTTOM, CENTER };

	Viewport const* viewport;

	// Textures
	Texture tex_menubar_bg;
	Texture tex_menuseparator;
	Texture tex_menuseparator_leftend;
	Texture tex_menuseparator_rightend;
	Texture tex_menuitem_bg;
	Texture tex_window_bg;
	Texture tex_window_edge_top;
	Texture tex_window_edge_topleft;
	Texture tex_window_edge_topright;
	Texture tex_window_edge_left;
	Texture tex_window_edge_right;
	Texture tex_window_edge_bottom;
	Texture tex_window_edge_bottomleft;
	Texture tex_window_edge_bottomright;
	Texture tex_field_bg;
	Texture tex_field_edge_top;
	Texture tex_field_edge_topleft;
	Texture tex_field_edge_topright;
	Texture tex_field_edge_left;
	Texture tex_field_edge_right;
	Texture tex_field_edge_bottom;
	Texture tex_field_edge_bottomleft;
	Texture tex_field_edge_bottomright;
	Texture tex_button_left;
	Texture tex_button;
	Texture tex_button_right;
	Texture tex_button_pressed_left;
	Texture tex_button_pressed;
	Texture tex_button_pressed_right;
	Texture tex_folder;
	Texture tex_file;
	Texture tex_scrollbar_horiz;
	Texture tex_scrollbar_horiz_left;
	Texture tex_scrollbar_horiz_right;
	Texture tex_scrollbar_vert;
	Texture tex_scrollbar_vert_top;
	Texture tex_scrollbar_vert_bottom;
	Texture tex_scrollbar_pressed_horiz;
	Texture tex_scrollbar_pressed_horiz_left;
	Texture tex_scrollbar_pressed_horiz_right;
	Texture tex_scrollbar_pressed_vert;
	Texture tex_scrollbar_pressed_vert_top;
	Texture tex_scrollbar_pressed_vert_bottom;
	Texture tex_scrollbar_button_up;
	Texture tex_scrollbar_button_left;
	Texture tex_scrollbar_button_right;
	Texture tex_scrollbar_button_down;
	Texture tex_scrollbar_button_pressed_up;
	Texture tex_scrollbar_button_pressed_left;
	Texture tex_scrollbar_button_pressed_right;
	Texture tex_scrollbar_button_pressed_down;
	Texture tex_scrollbar_bg_horiz;
	Texture tex_scrollbar_bg_horiz_left;
	Texture tex_scrollbar_bg_horiz_right;
	Texture tex_scrollbar_bg_vert;
	Texture tex_scrollbar_bg_vert_top;
	Texture tex_scrollbar_bg_vert_bottom;

	// Font and its sizes
	Font font;
	uint32_t font_menu_size;
	uint32_t font_menuitem_size;
	uint32_t font_titlebar_size;
	uint32_t font_label_size;
	uint32_t font_input_size;
	uint32_t font_button_size;

	// Padding and other sizes
	Real padding_menu_h;
	Real padding_menuitem_h, padding_menuitem_v;
	Real textinput_min_size;
	Real folderview_min_width;
	uint32_t folderview_min_rows;

	// Sprite rendering functions
	Real spr_x_origin, spr_y_origin;
	Color text_color;
	Alignment text_align, text_valign;

	// Virtual functions required by superclass Renderer
	virtual uint32_t getWidth(void) const;
	virtual uint32_t getHeight(void) const;
	virtual void initRendering(void);
	virtual void deinitRendering(void);
	virtual void renderMenubarBackground(int32_t x_origin, int32_t y_origin, Menubar const* menubar);
	virtual void renderMenuLabel(int32_t x_origin, int32_t y_origin, Menu const* menu, UnicodeString const& label, bool mouse_over);
	virtual void renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep);
	virtual void renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over);
	virtual void renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title);
	virtual void renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str);
	virtual void renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput, UnicodeString const& value);
	virtual void renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed);
	virtual void renderFolderview(int32_t x_origin, int32_t y_origin, Folderview const* folderview);
	virtual void renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, FolderChildren const& items);
	virtual void renderScrollbar(int32_t x_origin, int32_t y_origin, Scrollbar const* scrollbar, bool horizontal, bool up_or_left_key_pressed, bool down_or_right_key_pressed);
	virtual uint32_t getMenubarHeight(void) const;
	virtual uint32_t getMenuLabelWidth(UnicodeString const& label) const;
	virtual uint32_t getMenuseparatorMinWidth(void) const;
	virtual uint32_t getMenuseparatorHeight(void) const;
	virtual uint32_t getMenuitemWidth(UnicodeString const& label) const;
	virtual uint32_t getMenuitemHeight(void) const;
	virtual uint32_t getWindowTitlebarHeight(void) const;
	virtual uint32_t getWindowEdgeLeftWidth(void) const;
	virtual uint32_t getWindowEdgeRightWidth(void) const;
	virtual uint32_t getWindowEdgeBottomHeight(void) const;
	virtual uint32_t getLabelWidth(UnicodeString const& label) const;
	virtual uint32_t getLabelHeight(void) const;
	virtual uint32_t getMinimumTextinputWidth(void) const;
	virtual uint32_t getTextinputHeight(void) const;
	virtual uint32_t getButtonWidth(UnicodeString const& label) const;
	virtual uint32_t getButtonHeight(void) const;
	virtual uint32_t getMinimumFolderviewWidth(void) const;
	virtual uint32_t getFolderviewHeight(void) const;
	virtual uint32_t getMinimumFolderviewContentsWidth(UnicodeString const& label) const;
	virtual uint32_t getFolderviewContentsHeight(size_t items) const;
	virtual void getFolderviewEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const;
	virtual uint32_t getScrollbarWidth(void) const;
	virtual uint32_t getScrollbarHeight(void) const;
	virtual void setRenderareaLimit(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	virtual void removeRenderareaLimit(void);

	// Fixed sprite rendering functions. These use coordinate
	// system from topleft, rather than from bottomleft.
	void prepareSprites(Real x_origin, Real y_origin);
	inline void textSetColor(Color const& color) { text_color = color; }
	inline void textSetHorizontalAlign(Alignment align) { text_align = align; }
	inline void textSetVerticalAlign(Alignment align) { text_valign = align; }
	void renderSprite(Texture& tex, Vector2 const& pos, Vector2 const& size = Vector2::ZERO);
	void renderString(UnicodeString const& str, Real fontsize, Vector2 const& pos, Vector2 const& size);

};

}

}

#endif


