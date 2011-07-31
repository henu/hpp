#include "viewportrenderer.h"

#include "menuitem.h"
#include "menuseparator.h"
#include "menu.h"
#include "menubar.h"

#include "../viewport.h"
#include "../assert.h"
#include <cmath>

namespace Hpp
{

namespace Gui
{

ViewportRenderer::ViewportRenderer(Viewport const* viewport) :
viewport(viewport)
{
}

ViewportRenderer::~ViewportRenderer(void)
{
}

void ViewportRenderer::setFontSize(uint32_t font_size)
{
	font.setDefaultHeight(font_size);
	font_menu_size = font_size;
	font_menuitem_size = font_size;
	// Tune some padding values
	padding_menuitem_h = font_menuitem_size * 0.5;
	padding_menuitem_v = font_menuitem_size * 0.125;
}

void ViewportRenderer::loadTextureMenubarBg(Path const& path)
{
	tex_menubar_bg.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureMenuSelection(Path const& path)
{
	tex_menu_selection.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureMenuSelectionLeftend(Path const& path)
{
	tex_menu_selection_leftend.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureMenuSelectionRightend(Path const& path)
{
	tex_menu_selection_rightend.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureMenuseparator(Path const& path)
{
	tex_menuseparator.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureMenuseparatorLeftend(Path const& path)
{
	tex_menuseparator_leftend.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureMenuseparatorRightend(Path const& path)
{
	tex_menuseparator_rightend.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureMenuitemBg(Path const& path)
{
	tex_menuitem_bg.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadFont(Path const& path)
{
	font.loadMore(path);
}

uint32_t ViewportRenderer::getWidth(void) const
{
	HppAssert(viewport, "Viewport not set!");
	return viewport->getWidth();
}

uint32_t ViewportRenderer::getHeight(void) const
{
	HppAssert(viewport, "Viewport not set!");
	return viewport->getHeight();
}

void ViewportRenderer::initRendering(void)
{
	viewport->init2DRendering();
}

void ViewportRenderer::deinitRendering(void)
{
	viewport->deinit2DRendering();
}

void ViewportRenderer::renderMenubarBackground(Menubar const* menubar)
{
	int32_t x = menubar->getPositionX();
	int32_t y = menubar->getPositionY();
	uint32_t width = menubar->getWidth();

	uint32_t tex_menubar_width = tex_menubar_bg.getWidth();
	uint32_t tex_menubar_height = tex_menubar_bg.getHeight();

	viewport->renderSprite(tex_menubar_bg,
	                       Vector2(x, viewport->getHeight() - tex_menubar_height - y),
	                       Vector2(width, tex_menubar_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(width / (Real)tex_menubar_width, 1.0));
}

void ViewportRenderer::renderMenuLabel(Menu const* menu, UnicodeString const& label, bool mouse_over)
{
	Real x = menu->getPositionX();
	Real y = menu->getPositionY();
	Real width = menu->getWidth();

	Real tex_menulabel_height = tex_menu_selection.getHeight();

	Color color;
	if (mouse_over) {
		color = Color(1, 1, 1);
	} else {
		color = Color(0, 0, 0);
	}

	// Render label
	Real label_width = font.getStringWidth(label, font_menu_size);

	font.renderString(label,
	                  font_menu_size,
	                  color,
	                  viewport,
	                  Vector2(x + (width - label_width) / 2.0, viewport->getHeight() - y - tex_menulabel_height + (tex_menulabel_height - font_menu_size) / 2.0));
}

void ViewportRenderer::renderMenuseparator(Menuseparator const* menusep)
{
	Real x = menusep->getPositionX();
	Real y = menusep->getPositionY();
	Real width = menusep->getWidth();

	Real tex_menuseparator_width = tex_menuseparator.getWidth();
	Real tex_menuseparator_height = tex_menuseparator.getHeight();
	Real tex_menuseparator_leftend_width = tex_menuseparator_leftend.getWidth();
	Real tex_menuseparator_rightend_width = tex_menuseparator_rightend.getWidth();

	viewport->renderSprite(tex_menuseparator_leftend,
	                       Vector2(x, viewport->getHeight() - tex_menuseparator_height - y),
	                       Vector2(tex_menuseparator_leftend_width, tex_menuseparator_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, 1.0));
	viewport->renderSprite(tex_menuseparator,
	                       Vector2(x + tex_menuseparator_leftend_width, viewport->getHeight() - tex_menuseparator_height - y),
	                       Vector2(width - tex_menuseparator_leftend_width - tex_menuseparator_rightend_width, tex_menuseparator_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(width / (Real)tex_menuseparator_width, 1.0));
	viewport->renderSprite(tex_menuseparator_rightend,
	                       Vector2(x + width - tex_menuseparator_rightend_width, viewport->getHeight() - tex_menuseparator_height - y),
	                       Vector2(tex_menuseparator_rightend_width, tex_menuseparator_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, 1.0));
}

void ViewportRenderer::renderMenuitem(Menuitem const* menuitem, UnicodeString const& label, bool mouse_over)
{
	Real x = menuitem->getPositionX();
	Real y = menuitem->getPositionY();
	Real width = menuitem->getWidth();
	Real height = getMenuitemHeight();

	Real tex_menuitem_width = tex_menuitem_bg.getWidth();
	Real tex_menuitem_height = tex_menuitem_bg.getHeight();

	// Render background
	viewport->renderSprite(tex_menuitem_bg,
	                       Vector2(x, viewport->getHeight() - height - y),
	                       Vector2(width, height),
	                       Vector2(0.0, 0.0),
	                       Vector2(width / (Real)tex_menuitem_width, height / (Real)tex_menuitem_height));

	// Render label
	Color color;
	if (mouse_over) {
		color = Color(1, 1, 1);
	} else {
		color = Color(0, 0, 0);
	}
	font.renderString(label, font_menuitem_size, color, viewport,
	                  Vector2(x + padding_menuitem_h, viewport->getHeight() - y - height + (height - font_menu_size) / 2.0));

}

uint32_t ViewportRenderer::getMenubarHeight(void) const
{
	return tex_menubar_bg.getHeight();
}

uint32_t ViewportRenderer::getMenuLabelWidth(UnicodeString const& label) const
{
	return tex_menu_selection_leftend.getWidth() + tex_menu_selection_rightend.getWidth() + font.getStringWidth(label, font_menu_size);
}

uint32_t ViewportRenderer::getMenuseparatorMinWidth(void) const
{
	return tex_menuseparator_leftend.getWidth() + tex_menuseparator_rightend.getWidth();
}

uint32_t ViewportRenderer::getMenuseparatorHeight(void) const
{
	return tex_menuseparator.getHeight();
}

uint32_t ViewportRenderer::getMenuitemWidth(UnicodeString const& label) const
{
	return round(font.getStringWidth(label, font_menuitem_size) + padding_menuitem_h * 2.0);
}

uint32_t ViewportRenderer::getMenuitemHeight(void) const
{
	return round(font_menuitem_size + padding_menuitem_v * 2.0);
}

}

}
