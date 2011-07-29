#include "viewportrenderer.h"

#include "menu.h"
#include "menubar.h"

#include "../viewport.h"
#include "../assert.h"

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
	                       Vector2(x, viewport->getHeight() - tex_menubar_height + y),
	                       Vector2(width, tex_menubar_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(width / (Real)tex_menubar_width, 1.0));
}

void ViewportRenderer::renderMenuLabel(Menu const* menu, UnicodeString const& label)
{
	Real x = menu->getPositionX();
	Real y = menu->getPositionY();
	Real width = menu->getWidth();

	Real tex_menulabel_height = tex_menu_selection.getHeight();

	// Render possible selection background
// TODO: If menu is selected, then render selection background for it!

	// Render label
	Real label_width = font.getStringWidth(label, font_menu_size);

	font.renderString(label,
	                  font_menu_size,
	                  Hpp::Color(0, 0, 0),
	                  viewport,
	                  Vector2(x + (width - label_width) / 2.0, viewport->getHeight() - y - tex_menulabel_height + (tex_menulabel_height - font_menu_size) / 2.0));
}

uint32_t ViewportRenderer::getMenubarHeight(void)
{
	return tex_menubar_bg.getHeight();
}

uint32_t ViewportRenderer::getMenuLabelWidth(UnicodeString const& label)
{
	return tex_menu_selection_leftend.getWidth() + tex_menu_selection_rightend.getWidth() + font.getStringWidth(label, font_menu_size);
}

}

}
