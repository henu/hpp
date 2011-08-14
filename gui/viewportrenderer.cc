#include "viewportrenderer.h"

#include "menuitem.h"
#include "menuseparator.h"
#include "menu.h"
#include "menubar.h"
#include "window.h"
#include "label.h"
#include "textinput.h"
#include "button.h"

#include "../viewport.h"
#include "../assert.h"
#include <cmath>

namespace Hpp
{

namespace Gui
{

ViewportRenderer::ViewportRenderer(Viewport const* viewport) :
viewport(viewport),
spr_x_origin(0.0),
spr_y_origin(0.0),
text_color(0, 0, 0),
text_align(CENTER),
text_valign(CENTER)
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
	font_titlebar_size = font_size * 0.86;
	font_label_size = font_size * 0.86;
	font_input_size = font_size * 0.86;
	font_button_size = font_size;
	// Tune some padding values
	padding_menu_h = font_menu_size * 0.5;
	padding_menuitem_h = font_menuitem_size * 0.5;
	padding_menuitem_v = font_menuitem_size * 0.125;
	// Tune other values
	textinput_min_size = font_input_size * 5;
}

void ViewportRenderer::loadTextureMenubarBg(Path const& path)
{
	tex_menubar_bg.loadFromFile(path, DEFAULT);
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

void ViewportRenderer::loadTextureWindowBg(Path const& path)
{
	tex_window_bg.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureWindowEdgeTop(Path const& path)
{
	tex_window_edge_top.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureWindowEdgeTopLeft(Path const& path)
{
	tex_window_edge_topleft.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureWindowEdgeTopRight(Path const& path)
{
	tex_window_edge_topright.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureWindowEdgeLeft(Path const& path)
{
	tex_window_edge_left.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureWindowEdgeRight(Path const& path)
{
	tex_window_edge_right.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureWindowEdgeBottom(Path const& path)
{
	tex_window_edge_bottom.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureWindowEdgeBottomLeft(Path const& path)
{
	tex_window_edge_bottomleft.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureWindowEdgeBottomRight(Path const& path)
{
	tex_window_edge_bottomright.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldBg(Path const& path)
{
	tex_field_bg.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldEdgeTop(Path const& path)
{
	tex_field_edge_top.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldEdgeTopLeft(Path const& path)
{
	tex_field_edge_topleft.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldEdgeTopRight(Path const& path)
{
	tex_field_edge_topright.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldEdgeLeft(Path const& path)
{
	tex_field_edge_left.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldEdgeRight(Path const& path)
{
	tex_field_edge_right.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldEdgeBottom(Path const& path)
{
	tex_field_edge_bottom.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldEdgeBottomLeft(Path const& path)
{
	tex_field_edge_bottomleft.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureFieldEdgeBottomRight(Path const& path)
{
	tex_field_edge_bottomright.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureButtonLeft(Path const& path)
{
	tex_button_left.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureButton(Path const& path)
{
	tex_button.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureButtonRight(Path const& path)
{
	tex_button_right.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureButtonPressedLeft(Path const& path)
{
	tex_button_pressed_left.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureButtonPressed(Path const& path)
{
	tex_button_pressed.loadFromFile(path, DEFAULT);
}

void ViewportRenderer::loadTextureButtonPressedRight(Path const& path)
{
	tex_button_pressed_right.loadFromFile(path, DEFAULT);
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

void ViewportRenderer::renderMenubarBackground(int32_t x_origin, int32_t y_origin, Menubar const* menubar)
{
	uint32_t width = menubar->getWidth();

	uint32_t tex_menubar_width = tex_menubar_bg.getWidth();
	uint32_t tex_menubar_height = tex_menubar_bg.getHeight();

	viewport->renderSprite(tex_menubar_bg,
	                       Vector2(x_origin, viewport->getHeight() - tex_menubar_height - y_origin),
	                       Vector2(width, tex_menubar_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(width / (Real)tex_menubar_width, 1.0));
}

void ViewportRenderer::renderMenuLabel(int32_t x_origin, int32_t y_origin, Menu const* menu, UnicodeString const& label, bool mouse_over)
{
	Real width = menu->getWidth();
	Real height = menu->getHeight();

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
	                  Vector2(x_origin + (width - label_width) / 2.0, viewport->getHeight() - y_origin - height + (height - font_menu_size) / 2.0));
}

void ViewportRenderer::renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep)
{
	Real width = menusep->getWidth();

	Real tex_menuseparator_width = tex_menuseparator.getWidth();
	Real tex_menuseparator_height = tex_menuseparator.getHeight();
	Real tex_menuseparator_leftend_width = tex_menuseparator_leftend.getWidth();
	Real tex_menuseparator_rightend_width = tex_menuseparator_rightend.getWidth();

	viewport->renderSprite(tex_menuseparator_leftend,
	                       Vector2(x_origin, viewport->getHeight() - tex_menuseparator_height - y_origin),
	                       Vector2(tex_menuseparator_leftend_width, tex_menuseparator_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, 1.0));
	viewport->renderSprite(tex_menuseparator,
	                       Vector2(x_origin + tex_menuseparator_leftend_width, viewport->getHeight() - tex_menuseparator_height - y_origin),
	                       Vector2(width - tex_menuseparator_leftend_width - tex_menuseparator_rightend_width, tex_menuseparator_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(width / (Real)tex_menuseparator_width, 1.0));
	viewport->renderSprite(tex_menuseparator_rightend,
	                       Vector2(x_origin + width - tex_menuseparator_rightend_width, viewport->getHeight() - tex_menuseparator_height - y_origin),
	                       Vector2(tex_menuseparator_rightend_width, tex_menuseparator_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, 1.0));
}

void ViewportRenderer::renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over)
{
	Real width = menuitem->getWidth();
	Real height = getMenuitemHeight();

	Real tex_menuitem_width = tex_menuitem_bg.getWidth();
	Real tex_menuitem_height = tex_menuitem_bg.getHeight();

	// Render background
	viewport->renderSprite(tex_menuitem_bg,
	                       Vector2(x_origin, viewport->getHeight() - height - y_origin),
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
	                  Vector2(x_origin + padding_menuitem_h, viewport->getHeight() - y_origin - height + (height - font_menuitem_size) / 2.0));

}

void ViewportRenderer::renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title)
{

	Real window_width = window->getWidth();
	Real window_height = window->getHeight();

	Real titlebar_height = tex_window_edge_top.getHeight();
	Real titlebar_leftcorner_width = tex_window_edge_topleft.getWidth();
	Real titlebar_rightcorner_width = tex_window_edge_topright.getWidth();

	Real edge_left_width = tex_window_edge_left.getWidth();
	Real edge_right_width = tex_window_edge_right.getWidth();
	Real edge_bottom_height = tex_window_edge_bottom.getHeight();

	// Titlebar
	viewport->renderSprite(tex_window_edge_topleft,
	                       Vector2(x_origin, viewport->getHeight() - y_origin - titlebar_height),
	                       Vector2(titlebar_leftcorner_width, titlebar_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, 1.0));
	viewport->renderSprite(tex_window_edge_top,
	                       Vector2(x_origin + titlebar_leftcorner_width, viewport->getHeight() - y_origin - titlebar_height),
	                       Vector2(window_width - titlebar_leftcorner_width - titlebar_rightcorner_width, titlebar_height),
	                       Vector2(0.0, 0.0),
	                       Vector2((window_width - titlebar_leftcorner_width - titlebar_rightcorner_width) / tex_window_edge_top.getWidth(), 1.0));
	viewport->renderSprite(tex_window_edge_topright,
	                       Vector2(x_origin + window_width - titlebar_rightcorner_width, viewport->getHeight() - y_origin - titlebar_height),
	                       Vector2(titlebar_rightcorner_width, titlebar_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, 1.0));

	// Left and right edges
	viewport->renderSprite(tex_window_edge_left,
	                       Vector2(x_origin, viewport->getHeight() - y_origin - window_height + edge_bottom_height),
	                       Vector2(edge_left_width, window_height - titlebar_height - edge_bottom_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, (window_height - titlebar_height - edge_bottom_height) / tex_window_edge_left.getHeight()));
	viewport->renderSprite(tex_window_edge_right,
	                       Vector2(x_origin + window_width - edge_right_width, viewport->getHeight() - y_origin - window_height + edge_bottom_height),
	                       Vector2(edge_right_width, window_height - titlebar_height - edge_bottom_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, (window_height - titlebar_height - edge_bottom_height) / tex_window_edge_left.getHeight()));

	// Bottom edge
	viewport->renderSprite(tex_window_edge_bottomleft,
	                       Vector2(x_origin, viewport->getHeight() - y_origin - window_height),
	                       Vector2(edge_left_width, edge_bottom_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, 1.0));
	viewport->renderSprite(tex_window_edge_bottom,
	                       Vector2(x_origin + edge_left_width, viewport->getHeight() - y_origin - window_height),
	                       Vector2(window_width - edge_left_width - edge_right_width, edge_bottom_height),
	                       Vector2(0.0, 0.0),
	                       Vector2((window_width - edge_left_width - edge_right_width) / tex_window_edge_bottom.getWidth(), 1.0));
	viewport->renderSprite(tex_window_edge_bottomright,
	                       Vector2(x_origin + window_width - edge_right_width, viewport->getHeight() - y_origin - window_height),
	                       Vector2(edge_right_width, edge_bottom_height),
	                       Vector2(0.0, 0.0),
	                       Vector2(1.0, 1.0));

	// Render background
	viewport->renderSprite(tex_window_bg,
	                       Vector2(x_origin + edge_left_width, viewport->getHeight() - y_origin - window_height + edge_bottom_height),
	                       Vector2(window_width - edge_left_width - edge_right_width, window_height - titlebar_height - edge_bottom_height),
	                       Vector2(0.0, 0.0),
	                       Vector2((window_width - edge_left_width - edge_right_width) / tex_window_bg.getWidth(), (window_height - titlebar_height - edge_bottom_height) / tex_window_bg.getWidth()));

	// Title
	Color color(0.25, 0.25, 0.25);
	font.renderString(title, font_titlebar_size, color, viewport,
	                  Vector2(x_origin + (window_width - font.getStringWidth(title, font_titlebar_size)) / 2.0, viewport->getHeight() - y_origin - titlebar_height + (titlebar_height - font_titlebar_size) / 2.0));

}

void ViewportRenderer::renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str)
{
	Real width = label->getWidth();
	Real height = label->getHeight();

	Color color(0, 0, 0);

	// Render label
	Real label_width = font.getStringWidth(label_str, font_label_size);

	font.renderString(label_str,
	                  font_label_size,
	                  color,
	                  viewport,
	                  Vector2(x_origin + (width - label_width) / 2.0, viewport->getHeight() - y_origin - height + (height - font_label_size) / 2.0));
}

void ViewportRenderer::renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput, UnicodeString const& value)
{
	prepareSprites(x_origin, y_origin);
	Real textinput_width = textinput->getWidth();

	Real edge_top_height = tex_field_edge_top.getHeight();
	Real edge_left_width = tex_field_edge_left.getWidth();
	Real edge_right_width = tex_field_edge_right.getWidth();
	Real edge_bottom_height = tex_field_edge_bottom.getHeight();

	Real content_height = font_input_size;

	// Top edge
	renderSprite(tex_field_edge_topleft,
	             Vector2(0, 0),
	             Vector2(edge_left_width, edge_top_height));
	renderSprite(tex_field_edge_top,
	             Vector2(edge_left_width, 0),
	             Vector2(textinput_width - edge_left_width - edge_right_width, edge_top_height));
	renderSprite(tex_field_edge_topright,
	             Vector2(textinput_width - edge_right_width, 0),
	             Vector2(edge_right_width, edge_top_height));
	// Middle edges and content
	renderSprite(tex_field_edge_left,
	             Vector2(0, edge_top_height),
	             Vector2(edge_left_width, content_height));
	renderSprite(tex_field_bg,
	             Vector2(edge_left_width, edge_top_height),
	             Vector2(textinput_width - edge_left_width - edge_right_width, content_height));
	renderSprite(tex_field_edge_right,
	             Vector2(textinput_width - edge_right_width, edge_top_height),
	             Vector2(edge_right_width, content_height));
	// Top edge
	renderSprite(tex_field_edge_bottomleft,
	             Vector2(0, edge_top_height + content_height),
	             Vector2(edge_left_width, edge_bottom_height));
	renderSprite(tex_field_edge_bottom,
	             Vector2(edge_left_width, edge_top_height + content_height),
	             Vector2(textinput_width - edge_left_width - edge_right_width, edge_bottom_height));
	renderSprite(tex_field_edge_bottomright,
	             Vector2(textinput_width - edge_right_width, edge_top_height + content_height),
	             Vector2(edge_right_width, edge_bottom_height));

	// Value
	textSetColor(Color(0, 0, 0));
	textSetHorizontalAlign(LEFT);
	textSetVerticalAlign(CENTER);
	renderString(value, font_input_size,
	             Vector2(edge_left_width, edge_top_height),
	             Vector2(textinput_width - edge_left_width - edge_right_width, content_height));
}

void ViewportRenderer::renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed)
{
	prepareSprites(x_origin, y_origin);
	Real button_width = button->getWidth();
	Real button_height = button->getHeight();

	Real button_left_width = tex_button_left.getWidth();
	Real button_right_width = tex_button_right.getWidth();

	if (!pressed) {
		renderSprite(tex_button_left,
		             Vector2(0, 0));
		renderSprite(tex_button,
		             Vector2(button_left_width, 0),
		             Vector2(button_width - button_left_width - button_right_width, 0));
		renderSprite(tex_button_right,
		             Vector2(button_width - button_right_width, 0));
		textSetColor(Color(0.15, 0.15, 0.15));
	} else {
		renderSprite(tex_button_pressed_left,
		             Vector2(0, 0));
		renderSprite(tex_button_pressed,
		             Vector2(button_left_width, 0),
		             Vector2(button_width - button_left_width - button_right_width, 0));
		renderSprite(tex_button_pressed_right,
		             Vector2(button_width - button_right_width, 0));
		textSetColor(Color(0.05, 0.05, 0.05));
	}
	textSetHorizontalAlign(CENTER);
	textSetVerticalAlign(CENTER);
	renderString(label, font_button_size,
	             Vector2(0, 0),
	             Vector2(button_width, button_height));
}

uint32_t ViewportRenderer::getMenubarHeight(void) const
{
	return tex_menubar_bg.getHeight();
}

uint32_t ViewportRenderer::getMenuLabelWidth(UnicodeString const& label) const
{
	return font.getStringWidth(label, font_menu_size) + padding_menu_h*2;
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

uint32_t ViewportRenderer::getWindowTitlebarHeight(void) const
{
	return tex_window_edge_top.getHeight();
}

uint32_t ViewportRenderer::getWindowEdgeLeftWidth(void) const
{
	return tex_window_edge_left.getWidth();
}

uint32_t ViewportRenderer::getWindowEdgeRightWidth(void) const
{
	return tex_window_edge_right.getWidth();
}

uint32_t ViewportRenderer::getWindowEdgeBottomHeight(void) const
{
	return tex_window_edge_bottom.getHeight();
}

uint32_t ViewportRenderer::getLabelWidth(UnicodeString const& label) const
{
	return font.getStringWidth(label, font_label_size);
}

uint32_t ViewportRenderer::getLabelHeight(void) const
{
	return font_label_size;
}

uint32_t ViewportRenderer::getMinimumTextinputWidth(void) const
{
	return tex_field_edge_left.getWidth() + tex_field_edge_right.getWidth() + textinput_min_size;
}

uint32_t ViewportRenderer::getTextinputHeight(void) const
{
	return tex_field_edge_top.getHeight() + tex_field_edge_bottom.getHeight() + font_input_size;
}

uint32_t ViewportRenderer::getButtonWidth(UnicodeString const& label) const
{
	return tex_button_left.getWidth() + font.getStringWidth(label, font_button_size) + tex_button_right.getWidth();
}

uint32_t ViewportRenderer::getButtonHeight(void) const
{
	return tex_button.getHeight();
}

void ViewportRenderer::prepareSprites(Real x_origin, Real y_origin)
{
	spr_x_origin = x_origin;
	spr_y_origin = y_origin;
}

void ViewportRenderer::renderSprite(Texture& tex, Vector2 const& pos, Vector2 const& size)
{
	Vector2 size2;
	if (size.x == 0) {
	 	size2.x = tex.getWidth();
	} else {
		size2.x = size.x;
	}
	if (size.y == 0) {
	 	size2.y = tex.getHeight();
	} else {
		size2.y = size.y;
	}
	viewport->renderSprite(tex,
	                       Vector2(spr_x_origin + pos.x, viewport->getHeight() - spr_y_origin - size2.y - pos.y),
	                       size2,
	                       Vector2(0.0, 0.0),
	                       Vector2(size2.x / tex.getWidth(), size2.y / tex.getHeight()));
}

void ViewportRenderer::renderString(UnicodeString const& str, Real fontsize, Vector2 const& pos, Vector2 const& size)
{
	Real str_width = font.getStringWidth(str, fontsize);
	Vector2 pos_rel;
	if (text_align == CENTER) {
		pos_rel.x = (size.x - str_width) / 2.0;
	} else if (text_align == RIGHT) {
		pos_rel.x = size.x - str_width;
	} else {
		pos_rel.x = 0;
	}
	if (text_valign == CENTER) {
		pos_rel.y = (size.y - fontsize) / 2.0;
	} else if (text_valign == BOTTOM) {
		pos_rel.y = size.y - fontsize;
	} else {
		pos_rel.y = 0;
	}

	font.renderString(str, fontsize, text_color,
	                  viewport,
	                  Vector2(spr_x_origin + pos.x + pos_rel.x, viewport->getHeight() - spr_y_origin - fontsize - pos.y - pos_rel.y));
}

}

}
