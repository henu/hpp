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
	void loadTextureMenubarBg(Path const& path);
	void loadTextureMenuseparator(Path const& path);
	void loadTextureMenuseparatorLeftend(Path const& path);
	void loadTextureMenuseparatorRightend(Path const& path);
	void loadTextureMenuitemBg(Path const& path);
	void loadTextureWindowBg(Path const& path);
	void loadTextureWindowEdgeTop(Path const& path);
	void loadTextureWindowEdgeTopLeft(Path const& path);
	void loadTextureWindowEdgeTopRight(Path const& path);
	void loadTextureWindowEdgeLeft(Path const& path);
	void loadTextureWindowEdgeRight(Path const& path);
	void loadTextureWindowEdgeBottom(Path const& path);
	void loadTextureWindowEdgeBottomLeft(Path const& path);
	void loadTextureWindowEdgeBottomRight(Path const& path);

	// Font loading functions. Multiple fonts can be used. The firstly
	// loaded font will always be preferred, but if some character is not
	// found from there, then it is loaded from second font, and if its
	// missing there too, then third is tried and so on.
	void loadFont(Path const& path);

	inline void setViewport(Viewport const* viewport) { this->viewport = viewport; sizeChanged(); }

private:

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

	// Font and its sizes
	Font font;
	uint32_t font_menu_size;
	uint32_t font_menuitem_size;
	uint32_t font_titlebar_size;
	uint32_t font_label_size;

	// Padding
	Real padding_menu_h;
	Real padding_menuitem_h, padding_menuitem_v;

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

};

}

}

#endif


