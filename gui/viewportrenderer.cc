#include "viewportrenderer.h"

#include "menuitem.h"
#include "menuseparator.h"
#include "menu.h"
#include "menubar.h"
#include "window.h"
#include "label.h"
#include "textinput.h"
#include "button.h"
#include "folderview.h"
#include "folderviewcontents.h"
#include "scrollbar.h"

#include "../display.h"
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
	font_input_size = font_size;
	font_button_size = font_size;
	// Tune some padding values
	padding_menu_h = font_menu_size * 0.5;
	padding_menuitem_h = font_menuitem_size * 0.5;
	padding_menuitem_v = font_menuitem_size * 0.125;
	// Tune other values
	textinput_min_size = font_input_size * 5;
	folderview_min_width = font_input_size * 20;
	folderview_min_rows = 6;
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

void ViewportRenderer::renderFolderview(int32_t x_origin, int32_t y_origin, Folderview const* folderview)
{
	prepareSprites(x_origin, y_origin);
	Real folderview_width = folderview->getWidth();
	Real folderview_height = folderview->getHeight();

	Real edge_top_height = tex_field_edge_top.getHeight();
	Real edge_left_width = tex_field_edge_left.getWidth();
	Real edge_right_width = tex_field_edge_right.getWidth();
	Real edge_bottom_height = tex_field_edge_bottom.getHeight();

	// Top edge
	renderSprite(tex_field_edge_topleft,
	             Vector2(0, 0));
	renderSprite(tex_field_edge_top,
	             Vector2(edge_left_width, 0),
	             Vector2(folderview_width - edge_left_width - edge_right_width, edge_top_height));
	renderSprite(tex_field_edge_topright,
	             Vector2(folderview_width - edge_right_width, 0));
	// Middle edges and content
	renderSprite(tex_field_edge_left,
	             Vector2(0, edge_top_height),
	             Vector2(edge_left_width, folderview_height - edge_top_height - edge_bottom_height));
	renderSprite(tex_field_bg,
	             Vector2(edge_left_width, edge_top_height),
	             Vector2(folderview_width - edge_left_width - edge_right_width, folderview_height - edge_top_height - edge_bottom_height));
	renderSprite(tex_field_edge_right,
	             Vector2(folderview_width - edge_right_width, edge_top_height),
	             Vector2(edge_right_width, folderview_height - edge_top_height - edge_bottom_height));
	// Top edge
	renderSprite(tex_field_edge_bottomleft,
	             Vector2(0, folderview_height - edge_bottom_height));
	renderSprite(tex_field_edge_bottom,
	             Vector2(edge_left_width, folderview_height - edge_bottom_height),
	             Vector2(folderview_width - edge_left_width - edge_right_width, edge_bottom_height));
	renderSprite(tex_field_edge_bottomright,
	             Vector2(folderview_width - edge_right_width, folderview_height - edge_bottom_height));

}

void ViewportRenderer::renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, FolderChildren const& items)
{
	prepareSprites(x_origin, y_origin);

	Real folderviewcontents_width = folderviewcontents->getWidth();

	Real pos_y = 0;
	setFontSize(font_input_size);
	textSetColor(Color(0, 0, 0));
	textSetHorizontalAlign(LEFT);
	textSetVerticalAlign(CENTER);
	for (FolderChildren::const_iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		FolderChild const& item = *items_it;
		if (item.type == FolderChild::FOLDER) {
			renderSprite(tex_folder, Vector2(0, pos_y));
		} else {
			renderSprite(tex_file, Vector2(0, pos_y));
		}
		renderString(item.name, font_input_size, Vector2(tex_folder.getWidth(), pos_y), Vector2(folderviewcontents_width, tex_folder.getHeight()));
		pos_y += tex_folder.getHeight();
	}
}

void ViewportRenderer::renderScrollbar(int32_t x_origin, int32_t y_origin, Scrollbar const* scrollbar, bool horizontal, bool up_or_left_key_pressed, bool down_or_right_key_pressed, bool slider_pressed)
{
	prepareSprites(x_origin, y_origin);

	uint32_t scrollbar_width = scrollbar->getWidth();
	uint32_t scrollbar_height = scrollbar->getHeight();

	if (horizontal) {
		uint32_t scrollbar_leftend_width = tex_scrollbar_bg_horiz_left.getWidth();
		uint32_t scrollbar_rightend_width = tex_scrollbar_bg_horiz_right.getWidth();
		uint32_t scrollbar_button_left_width = tex_scrollbar_button_left.getWidth();
		uint32_t scrollbar_button_right_width = tex_scrollbar_button_right.getWidth();
		// Buttons
		if (up_or_left_key_pressed) {
			renderSprite(tex_scrollbar_button_pressed_left, Vector2(0, 0));
		} else {
			renderSprite(tex_scrollbar_button_left, Vector2(0, 0));
		}
		if (down_or_right_key_pressed) {
			renderSprite(tex_scrollbar_button_pressed_right, Vector2(scrollbar_width - scrollbar_button_right_width, 0));
		} else {
			renderSprite(tex_scrollbar_button_right, Vector2(scrollbar_width - scrollbar_button_right_width, 0));
		}
		// Background
		renderSprite(tex_scrollbar_bg_horiz_left, Vector2(scrollbar_button_left_width, 0));
		renderSprite(tex_scrollbar_bg_horiz, Vector2(scrollbar_button_left_width + scrollbar_leftend_width, 0), Vector2(scrollbar_width - scrollbar_leftend_width - scrollbar_rightend_width - scrollbar_button_left_width - scrollbar_button_right_width, scrollbar_height));
		renderSprite(tex_scrollbar_bg_horiz_right, Vector2(scrollbar_width - scrollbar_rightend_width - scrollbar_button_right_width, 0));
		// Slider
		Texture* tex_left;
		Texture* tex;
		Texture* tex_right;
		if (slider_pressed) {
			tex_left = &tex_scrollbar_pressed_horiz_left;
			tex = &tex_scrollbar_pressed_horiz;
			tex_right = &tex_scrollbar_pressed_horiz_right;
		} else {
			tex_left = &tex_scrollbar_horiz_left;
			tex = &tex_scrollbar_horiz;
			tex_right = &tex_scrollbar_horiz_right;
		}
		uint32_t slider_leftend_width = tex_left->getWidth();
		uint32_t slider_rightend_width = tex_right->getWidth();
		uint32_t slider_area = scrollbar_width - scrollbar_button_left_width - scrollbar_button_right_width;
		Real slider_size = slider_area * scrollbar->getSliderSize();
		Real slider_pos = (slider_area - slider_size) * scrollbar->getValue();
// TODO: What it slider is too small?
		renderSprite(*tex_left, Vector2(scrollbar_button_left_width + slider_pos, 0));
		renderSprite(*tex, Vector2(scrollbar_button_left_width + slider_pos + slider_leftend_width, 0), Vector2(slider_size - slider_leftend_width - slider_rightend_width, 0));
		renderSprite(*tex_right, Vector2(scrollbar_button_left_width + slider_pos + slider_size - slider_rightend_width, 0));
	} else {
		uint32_t scrollbar_topend_height = tex_scrollbar_bg_vert_top.getHeight();
		uint32_t scrollbar_bottomend_height = tex_scrollbar_bg_vert_bottom.getHeight();
		uint32_t scrollbar_button_up_height = tex_scrollbar_button_up.getHeight();
		uint32_t scrollbar_button_down_height = tex_scrollbar_button_down.getHeight();
		// Buttons
		if (up_or_left_key_pressed) {
			renderSprite(tex_scrollbar_button_pressed_up, Vector2(0, 0));
		} else {
			renderSprite(tex_scrollbar_button_up, Vector2(0, 0));
		}
		if (down_or_right_key_pressed) {
			renderSprite(tex_scrollbar_button_pressed_down, Vector2(0, scrollbar_height - scrollbar_button_down_height));
		} else {
			renderSprite(tex_scrollbar_button_down, Vector2(0, scrollbar_height - scrollbar_button_down_height));
		}
		// Background
		renderSprite(tex_scrollbar_bg_vert_top, Vector2(0, scrollbar_button_up_height));
		renderSprite(tex_scrollbar_bg_vert, Vector2(0, scrollbar_button_up_height + scrollbar_topend_height), Vector2(scrollbar_width, scrollbar_height - scrollbar_topend_height - scrollbar_bottomend_height - scrollbar_button_up_height - scrollbar_button_down_height));
		renderSprite(tex_scrollbar_bg_vert_bottom, Vector2(0, scrollbar_height - scrollbar_bottomend_height - scrollbar_button_down_height));
		// Slider
		Texture* tex_top;
		Texture* tex;
		Texture* tex_bottom;
		if (slider_pressed) {
			tex_top = &tex_scrollbar_pressed_vert_top;
			tex = &tex_scrollbar_pressed_vert;
			tex_bottom = &tex_scrollbar_pressed_vert_bottom;
		} else {
			tex_top = &tex_scrollbar_vert_top;
			tex = &tex_scrollbar_vert;
			tex_bottom = &tex_scrollbar_vert_bottom;
		}
		uint32_t slider_topend_height = tex_top->getHeight();
		uint32_t slider_bottomend_height = tex_bottom->getHeight();
		uint32_t slider_area = scrollbar_height - scrollbar_button_up_height - scrollbar_button_down_height;
		Real slider_size = slider_area * scrollbar->getSliderSize();
		Real slider_pos = (slider_area - slider_size) * scrollbar->getValue();
// TODO: What it slider is too small?
		renderSprite(*tex_top, Vector2(0, scrollbar_button_up_height + slider_pos));
		renderSprite(*tex, Vector2(0, scrollbar_button_up_height + slider_pos + slider_topend_height), Vector2(0, slider_size - slider_topend_height - slider_bottomend_height));
		renderSprite(*tex_bottom, Vector2(0, scrollbar_button_up_height + slider_pos + slider_size - slider_bottomend_height));
	}

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

uint32_t ViewportRenderer::getMinimumFolderviewWidth(void) const
{
	return folderview_min_width;
}

uint32_t ViewportRenderer::getFolderviewHeight(void) const
{
	return tex_folder.getHeight() * folderview_min_rows + tex_field_edge_top.getHeight() + tex_field_edge_bottom.getHeight();
}

uint32_t ViewportRenderer::getMinimumFolderviewContentsWidth(UnicodeString const& label) const
{
	return tex_folder.getWidth() + font.getStringWidth(label, font_input_size);
}

uint32_t ViewportRenderer::getFolderviewContentsHeight(size_t items) const
{
	return tex_folder.getHeight() * items;
}

void ViewportRenderer::getFolderviewEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const
{
	edge_top = tex_field_edge_top.getHeight();
	edge_left = tex_field_edge_left.getWidth();
	edge_right = tex_field_edge_right.getWidth();
	edge_bottom = tex_field_edge_bottom.getHeight();
}

uint32_t ViewportRenderer::getScrollbarWidth(void) const
{
	return tex_scrollbar_bg_vert.getWidth();
}

uint32_t ViewportRenderer::getScrollbarHeight(void) const
{
	return tex_scrollbar_bg_horiz.getHeight();
}

void ViewportRenderer::setRenderareaLimit(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	x += viewport->getPositionX();
	y = viewport->getPositionY() + viewport->getHeight() - y - height;
	Display::pushScissor(x, y, width, height);
}

void ViewportRenderer::removeRenderareaLimit(void)
{
	Display::popScissor();
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
