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

	// Font loading functions. Multiple fonts can be used. The firstly
	// loaded font will always be preferred, but if some character is not
	// found from there, then it is loaded from second font, and if its
	// missing there too, then third is tried and so on.
	void loadFont(Path const& path);

	inline void setViewport(Viewport const* viewport) { this->viewport = viewport; }

private:

	Viewport const* viewport;

	// Textures
	Texture tex_menubar_bg;
	Texture tex_menuseparator;
	Texture tex_menuseparator_leftend;
	Texture tex_menuseparator_rightend;
	Texture tex_menuitem_bg;

	// Font and its sizes
	Font font;
	uint32_t font_menu_size;
	uint32_t font_menuitem_size;

	// Padding
	Real padding_menu_h;
	Real padding_menuitem_h, padding_menuitem_v;

	// Virtual functions required by superclass Renderer
	virtual uint32_t getWidth(void) const;
	virtual uint32_t getHeight(void) const;
	virtual void initRendering(void);
	virtual void deinitRendering(void);
	virtual void renderMenubarBackground(Menubar const* menubar);
	virtual void renderMenuLabel(Menu const* menu, UnicodeString const& label, bool mouse_over);
	virtual void renderMenuseparator(Menuseparator const* menusep);
	virtual void renderMenuitem(Menuitem const* menuitem, UnicodeString const& label, bool mouse_over);
	virtual uint32_t getMenubarHeight(void) const;
	virtual uint32_t getMenuLabelWidth(UnicodeString const& label) const;
	virtual uint32_t getMenuseparatorMinWidth(void) const;
	virtual uint32_t getMenuseparatorHeight(void) const;
	virtual uint32_t getMenuitemWidth(UnicodeString const& label) const;
	virtual uint32_t getMenuitemHeight(void) const;

};

}

}

#endif


