#include "viewportrenderer.h"

#include "menuitem.h"
#include "menuseparator.h"
#include "menu.h"
#include "areawithmenubar.h"
#include "window.h"
#include "label.h"
#include "textinput.h"
#include "button.h"
#include "folderview.h"
#include "folderviewcontents.h"
#include "scrollbar.h"
#include "slider.h"
#include "tabs.h"

#include "../renderqueue2d.h"
#include "../display.h"
#include "../time.h"
#include "../assert.h"
#include <cmath>

namespace Hpp
{

namespace Gui
{

ViewportRenderer::ViewportRenderer(Renderqueue2d* rqueue) :
rqueue(rqueue),
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
	font_tablabel_size = font_size;
	// Tune some padding values
	padding_menu_h = font_menu_size * 0.5;
	padding_menuitem_h = font_menuitem_size * 0.5;
	padding_menuitem_v = font_menuitem_size * 0.125;
	// Tune other values
	folderview_min_width = font_input_size * 20;
	folderview_min_rows = 6;
	window_dragcorner_size = 16;
}

void ViewportRenderer::loadFont(Path const& path)
{
	font.loadMore(path);
}

void ViewportRenderer::renderSprite(Texture* tex, Vector2 const& pos, Vector2 const& size, Color const& color)
{
	Vector2 size2;
	if (size.x < 0) {
	 	size2.x = tex->getWidth();
	} else {
		size2.x = size.x;
	}
	if (size.y < 0) {
	 	size2.y = tex->getHeight();
	} else {
		size2.y = size.y;
	}
	rqueue->renderSprite(tex,
	                     Vector2(spr_x_origin + pos.x, rqueue->getHeight() - spr_y_origin - size2.y - pos.y),
	                     size2,
	                     Vector2(0.0, 0.0),
	                     Vector2(size2.x / tex->getWidth(), size2.y / tex->getHeight()),
	                     color);
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
	                  rqueue,
	                  Vector2(spr_x_origin + pos.x + pos_rel.x, rqueue->getHeight() - spr_y_origin - fontsize - pos.y - pos_rel.y));
}

void ViewportRenderer::renderTextCursor(int32_t x_origin, int32_t y_origin)
{
	(void)x_origin;
	(void)y_origin;
}

void ViewportRenderer::loadTexFromFile(TexturePtr& texptr, Path const& path, bool clamp_to_edge_horizontally, bool clamp_to_edge_vertically)
{
	if (texptr.clean) {
		delete texptr.tex;
		texptr.tex = NULL;
	}
	Texture::Flags flags = 0;
	if (clamp_to_edge_horizontally) flags |= Texture::CLAMP_TO_EDGE_HORIZONTALLY;
	if (clamp_to_edge_vertically) flags |= Texture::CLAMP_TO_EDGE_VERTICALLY;
	texptr.tex = new Texture(path, DEFAULT, flags);
	texptr.clean = true;
}

void ViewportRenderer::setTexFromTex(TexturePtr& texptr, Texture* tex, bool clamp_to_edge_horizontally, bool clamp_to_edge_vertically, bool clean)
{
	if (texptr.clean) {
		delete texptr.tex;
		texptr.tex = NULL;
	}
	texptr.tex = tex;
	tex->setClampToEdgeHorizontally(clamp_to_edge_horizontally);
	tex->setClampToEdgeVertically(clamp_to_edge_vertically);
	texptr.clean = clean;
}

uint32_t ViewportRenderer::getWidth(void) const
{
	HppAssert(rqueue, "Viewport not set!");
	return rqueue->getWidth();
}

uint32_t ViewportRenderer::getHeight(void) const
{
	HppAssert(rqueue, "Viewport not set!");
	return rqueue->getHeight();
}

void ViewportRenderer::initRendering(void)
{
}

void ViewportRenderer::deinitRendering(void)
{
}

void ViewportRenderer::renderMenubarBackground(int32_t x_origin, int32_t y_origin, uint32_t width, AreaWithMenubar const* areawithmenubar)
{
	(void)areawithmenubar;

	uint32_t tex_menubar_width = tex_menubar_bg.tex->getWidth();
	uint32_t tex_menubar_height = tex_menubar_bg.tex->getHeight();

	rqueue->renderSprite(tex_menubar_bg.tex,
	                     Vector2(x_origin, rqueue->getHeight() - tex_menubar_height - y_origin),
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
	                  rqueue,
	                  Vector2(x_origin + (width - label_width) / 2.0, rqueue->getHeight() - y_origin - height + (height - font_menu_size) / 2.0));
}

void ViewportRenderer::renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep)
{
	Real width = menusep->getWidth();

	Real tex_menuseparator_width = tex_menuseparator.tex->getWidth();
	Real tex_menuseparator_height = tex_menuseparator.tex->getHeight();
	Real tex_menuseparator_leftend_width = tex_menuseparator_leftend.tex->getWidth();
	Real tex_menuseparator_rightend_width = tex_menuseparator_rightend.tex->getWidth();

	rqueue->renderSprite(tex_menuseparator_leftend.tex,
	                     Vector2(x_origin, rqueue->getHeight() - tex_menuseparator_height - y_origin),
	                     Vector2(tex_menuseparator_leftend_width, tex_menuseparator_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(1.0, 1.0));
	rqueue->renderSprite(tex_menuseparator.tex,
	                     Vector2(x_origin + tex_menuseparator_leftend_width, rqueue->getHeight() - tex_menuseparator_height - y_origin),
	                     Vector2(width - tex_menuseparator_leftend_width - tex_menuseparator_rightend_width, tex_menuseparator_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(width / (Real)tex_menuseparator_width, 1.0));
	rqueue->renderSprite(tex_menuseparator_rightend.tex,
	                     Vector2(x_origin + width - tex_menuseparator_rightend_width, rqueue->getHeight() - tex_menuseparator_height - y_origin),
	                     Vector2(tex_menuseparator_rightend_width, tex_menuseparator_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(1.0, 1.0));
}

void ViewportRenderer::renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over)
{
	Real width = menuitem->getWidth();
	Real height = getMenuitemHeight();

	Real tex_menuitem_width = tex_menuitem_bg.tex->getWidth();
	Real tex_menuitem_height = tex_menuitem_bg.tex->getHeight();

	// Render background
	rqueue->renderSprite(tex_menuitem_bg.tex,
	                     Vector2(x_origin, rqueue->getHeight() - height - y_origin),
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
	font.renderString(label, font_menuitem_size, color, rqueue,
	                  Vector2(x_origin + padding_menuitem_h, rqueue->getHeight() - y_origin - height + (height - font_menuitem_size) / 2.0));

}

void ViewportRenderer::renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title)
{

	Real window_width = window->getWidth();
	Real window_height = window->getHeight();

	Real titlebar_height = tex_window_edge_top.tex->getHeight();
	Real titlebar_leftcorner_width = tex_window_edge_topleft.tex->getWidth();
	Real titlebar_rightcorner_width = tex_window_edge_topright.tex->getWidth();

	Real edge_left_width = tex_window_edge_left.tex->getWidth();
	Real edge_right_width = tex_window_edge_right.tex->getWidth();
	Real edge_bottom_height = tex_window_edge_bottom.tex->getHeight();

	// Titlebar
	rqueue->renderSprite(tex_window_edge_topleft.tex,
	                     Vector2(x_origin, rqueue->getHeight() - y_origin - titlebar_height),
	                     Vector2(titlebar_leftcorner_width, titlebar_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(1.0, 1.0));
	rqueue->renderSprite(tex_window_edge_top.tex,
	                     Vector2(x_origin + titlebar_leftcorner_width, rqueue->getHeight() - y_origin - titlebar_height),
	                     Vector2(window_width - titlebar_leftcorner_width - titlebar_rightcorner_width, titlebar_height),
	                     Vector2(0.0, 0.0),
	                     Vector2((window_width - titlebar_leftcorner_width - titlebar_rightcorner_width) / tex_window_edge_top.tex->getWidth(), 1.0));
	rqueue->renderSprite(tex_window_edge_topright.tex,
	                     Vector2(x_origin + window_width - titlebar_rightcorner_width, rqueue->getHeight() - y_origin - titlebar_height),
	                     Vector2(titlebar_rightcorner_width, titlebar_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(1.0, 1.0));

	// Left and right edges
	rqueue->renderSprite(tex_window_edge_left.tex,
	                     Vector2(x_origin, rqueue->getHeight() - y_origin - window_height + edge_bottom_height),
	                     Vector2(edge_left_width, window_height - titlebar_height - edge_bottom_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(1.0, (window_height - titlebar_height - edge_bottom_height) / tex_window_edge_left.tex->getHeight()));
	rqueue->renderSprite(tex_window_edge_right.tex,
	                     Vector2(x_origin + window_width - edge_right_width, rqueue->getHeight() - y_origin - window_height + edge_bottom_height),
	                     Vector2(edge_right_width, window_height - titlebar_height - edge_bottom_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(1.0, (window_height - titlebar_height - edge_bottom_height) / tex_window_edge_left.tex->getHeight()));

	// Bottom edge
	rqueue->renderSprite(tex_window_edge_bottomleft.tex,
	                     Vector2(x_origin, rqueue->getHeight() - y_origin - window_height),
	                     Vector2(edge_left_width, edge_bottom_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(1.0, 1.0));
	rqueue->renderSprite(tex_window_edge_bottom.tex,
	                     Vector2(x_origin + edge_left_width, rqueue->getHeight() - y_origin - window_height),
	                     Vector2(window_width - edge_left_width - edge_right_width, edge_bottom_height),
	                     Vector2(0.0, 0.0),
	                     Vector2((window_width - edge_left_width - edge_right_width) / tex_window_edge_bottom.tex->getWidth(), 1.0));
	rqueue->renderSprite(tex_window_edge_bottomright.tex,
	                     Vector2(x_origin + window_width - edge_right_width, rqueue->getHeight() - y_origin - window_height),
	                     Vector2(edge_right_width, edge_bottom_height),
	                     Vector2(0.0, 0.0),
	                     Vector2(1.0, 1.0));

	// Render background
	rqueue->renderSprite(tex_window_bg.tex,
	                     Vector2(x_origin + edge_left_width, rqueue->getHeight() - y_origin - window_height + edge_bottom_height),
	                     Vector2(window_width - edge_left_width - edge_right_width, window_height - titlebar_height - edge_bottom_height),
	                     Vector2(0.0, 0.0),
	                     Vector2((window_width - edge_left_width - edge_right_width) / tex_window_bg.tex->getWidth(), (window_height - titlebar_height - edge_bottom_height) / tex_window_bg.tex->getWidth()));

	// Title
	Color color(0.25, 0.25, 0.25);
	font.renderString(title, font_titlebar_size, color, rqueue,
	                  Vector2(x_origin + (window_width - font.getStringWidth(title, font_titlebar_size)) / 2.0, rqueue->getHeight() - y_origin - titlebar_height + (titlebar_height - font_titlebar_size) / 2.0));

}

void ViewportRenderer::renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str)
{
	Real height = label->getHeight();

	Color color = label->getColor();

	// Render label
	font.renderString(label_str,
	                  font_label_size,
	                  color,
	                  rqueue,
	                  Vector2(x_origin, rqueue->getHeight() - y_origin - height));
}

void ViewportRenderer::renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput)
{
	prepareSprites(x_origin, y_origin);
	Real textinput_width = textinput->getWidth();

	Real edge_top_height = tex_field_edge_top.tex->getHeight();
	Real edge_left_width = tex_field_edge_left.tex->getWidth();
	Real edge_right_width = tex_field_edge_right.tex->getWidth();
	Real edge_bottom_height = tex_field_edge_bottom.tex->getHeight();

	Real content_height = font_input_size;

	// Top edge
	renderSprite(tex_field_edge_topleft.tex,
	             Vector2(0, 0),
	             Vector2(edge_left_width, edge_top_height));
	renderSprite(tex_field_edge_top.tex,
	             Vector2(edge_left_width, 0),
	             Vector2(textinput_width - edge_left_width - edge_right_width, edge_top_height));
	renderSprite(tex_field_edge_topright.tex,
	             Vector2(textinput_width - edge_right_width, 0),
	             Vector2(edge_right_width, edge_top_height));
	// Middle edges
	renderSprite(tex_field_edge_left.tex,
	             Vector2(0, edge_top_height),
	             Vector2(edge_left_width, content_height));
	renderSprite(tex_field_edge_right.tex,
	             Vector2(textinput_width - edge_right_width, edge_top_height),
	             Vector2(edge_right_width, content_height));
	// Top edge
	renderSprite(tex_field_edge_bottomleft.tex,
	             Vector2(0, edge_top_height + content_height),
	             Vector2(edge_left_width, edge_bottom_height));
	renderSprite(tex_field_edge_bottom.tex,
	             Vector2(edge_left_width, edge_top_height + content_height),
	             Vector2(textinput_width - edge_left_width - edge_right_width, edge_bottom_height));
	renderSprite(tex_field_edge_bottomright.tex,
	             Vector2(textinput_width - edge_right_width, edge_top_height + content_height),
	             Vector2(edge_right_width, edge_bottom_height));
}

void ViewportRenderer::renderTextinputContents(int32_t x_origin, int32_t y_origin, TextinputContents const* textinputcontents, ssize_t cursor)
{
	prepareSprites(x_origin, y_origin);

	Real textinputcontents_width = textinputcontents->getWidth();

	Real content_height = font_input_size;

	// Background
	renderSprite(tex_field_bg.tex,
	             Vector2(0, 0),
	             Vector2(textinputcontents_width, content_height));

	// Value
	textSetColor(Color(0, 0, 0));
	textSetHorizontalAlign(LEFT);
	textSetVerticalAlign(CENTER);
	renderString(textinputcontents->getValue(), font_input_size,
	             Vector2(0, 0),
	             Vector2(textinputcontents_width, content_height));

	// Cursor
	if (cursor >= 0 && now().getNanoseconds() < 500000000) {
		Real cursor_pos = font.getStringWidth(textinputcontents->getValue().substr(0, cursor), font_input_size);
		renderString("|", font_input_size,
			     Vector2(cursor_pos, 0),
			     Vector2(textinputcontents_width - cursor_pos, content_height));
	}
}

void ViewportRenderer::renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed)
{
	prepareSprites(x_origin, y_origin);
	Real button_width = button->getWidth();
	Real button_height = button->getHeight();

	Real button_left_width = tex_button_left.tex->getWidth();
	Real button_right_width = tex_button_right.tex->getWidth();

	if (!button->isEnabled()) {
		renderSprite(tex_button_left.tex,
		             Vector2(0, 0),
		             Vector2(-1, -1),
		             button->getColor().getGrey());
		renderSprite(tex_button.tex,
		             Vector2(button_left_width, 0),
		             Vector2(button_width - button_left_width - button_right_width, -1),
		             button->getColor().getGrey());
		renderSprite(tex_button_right.tex,
		             Vector2(button_width - button_right_width, 0),
		             Vector2(-1, -1),
		             button->getColor().getGrey());
		textSetColor(button->getLabelColor().getGrey());
	} else if (!pressed) {
		renderSprite(tex_button_left.tex,
		             Vector2(0, 0),
		             Vector2(-1, -1),
		             button->getColor());
		renderSprite(tex_button.tex,
		             Vector2(button_left_width, 0),
		             Vector2(button_width - button_left_width - button_right_width, -1),
		             button->getColor());
		renderSprite(tex_button_right.tex,
		             Vector2(button_width - button_right_width, 0),
		             Vector2(-1, -1),
		             button->getColor());
		textSetColor(button->getLabelColor());
	} else {
		renderSprite(tex_button_pressed_left.tex,
		             Vector2(0, 0),
		             Vector2(-1, -1),
		             button->getColor());
		renderSprite(tex_button_pressed.tex,
		             Vector2(button_left_width, 0),
		             Vector2(button_width - button_left_width - button_right_width, -1),
		             button->getColor());
		renderSprite(tex_button_pressed_right.tex,
		             Vector2(button_width - button_right_width, 0),
		             Vector2(-1, -1),
		             button->getColor());
		textSetColor(button->getLabelColor());
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

	Real edge_top_height = tex_field_edge_top.tex->getHeight();
	Real edge_left_width = tex_field_edge_left.tex->getWidth();
	Real edge_right_width = tex_field_edge_right.tex->getWidth();
	Real edge_bottom_height = tex_field_edge_bottom.tex->getHeight();

	// Top edge
	renderSprite(tex_field_edge_topleft.tex,
	             Vector2(0, 0));
	renderSprite(tex_field_edge_top.tex,
	             Vector2(edge_left_width, 0),
	             Vector2(folderview_width - edge_left_width - edge_right_width, edge_top_height));
	renderSprite(tex_field_edge_topright.tex,
	             Vector2(folderview_width - edge_right_width, 0));
	// Middle edges and content
	renderSprite(tex_field_edge_left.tex,
	             Vector2(0, edge_top_height),
	             Vector2(edge_left_width, folderview_height - edge_top_height - edge_bottom_height));
	renderSprite(tex_field_bg.tex,
	             Vector2(edge_left_width, edge_top_height),
	             Vector2(folderview_width - edge_left_width - edge_right_width, folderview_height - edge_top_height - edge_bottom_height));
	renderSprite(tex_field_edge_right.tex,
	             Vector2(folderview_width - edge_right_width, edge_top_height),
	             Vector2(edge_right_width, folderview_height - edge_top_height - edge_bottom_height));
	// Top edge
	renderSprite(tex_field_edge_bottomleft.tex,
	             Vector2(0, folderview_height - edge_bottom_height));
	renderSprite(tex_field_edge_bottom.tex,
	             Vector2(edge_left_width, folderview_height - edge_bottom_height),
	             Vector2(folderview_width - edge_left_width - edge_right_width, edge_bottom_height));
	renderSprite(tex_field_edge_bottomright.tex,
	             Vector2(folderview_width - edge_right_width, folderview_height - edge_bottom_height));

}

void ViewportRenderer::renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, Path::DirChildren const& items)
{
	prepareSprites(x_origin, y_origin);

	Real folderviewcontents_width = folderviewcontents->getWidth();

	// Render selected items
	FolderviewContents::SelectedItems items_sel = folderviewcontents->getSelectedItems();
	for (FolderviewContents::SelectedItems::const_iterator items_sel_it = items_sel.begin();
	     items_sel_it != items_sel.end();
	     items_sel_it ++) {
		size_t item_id = *items_sel_it;
		renderSprite(tex_field_selection_bg.tex, Vector2(0, item_id * tex_folder.tex->getHeight()), Vector2(getWidth(), tex_folder.tex->getHeight()));
	}

	// Render files
	Real pos_y = 0;
	textSetColor(Color(0, 0, 0));
	textSetHorizontalAlign(LEFT);
	textSetVerticalAlign(CENTER);
	for (Path::DirChildren::const_iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		Path::DirChild const& item = *items_it;
		if (item.type == Path::DIRECTORY) {
			renderSprite(tex_folder.tex, Vector2(0, pos_y));
		} else {
			renderSprite(tex_file.tex, Vector2(0, pos_y));
		}
		renderString(item.name, font_input_size, Vector2(tex_folder.tex->getWidth(), pos_y), Vector2(folderviewcontents_width, tex_folder.tex->getHeight()));
		pos_y += tex_folder.tex->getHeight();
	}
}

void ViewportRenderer::renderScrollbar(int32_t x_origin, int32_t y_origin, Scrollbar const* scrollbar, bool horizontal, bool up_or_left_key_pressed, bool down_or_right_key_pressed, bool slider_pressed)
{
	prepareSprites(x_origin, y_origin);

	uint32_t scrollbar_width = scrollbar->getWidth();
	uint32_t scrollbar_height = scrollbar->getHeight();

	if (horizontal) {
		uint32_t scrollbar_leftend_width = tex_scrollbar_bg_horiz_left.tex->getWidth();
		uint32_t scrollbar_rightend_width = tex_scrollbar_bg_horiz_right.tex->getWidth();
		uint32_t scrollbar_button_left_width = tex_scrollbar_button_left.tex->getWidth();
		uint32_t scrollbar_button_right_width = tex_scrollbar_button_right.tex->getWidth();
		// Buttons
		if (up_or_left_key_pressed) {
			renderSprite(tex_scrollbar_button_pressed_left.tex, Vector2(0, 0));
		} else {
			renderSprite(tex_scrollbar_button_left.tex, Vector2(0, 0));
		}
		if (down_or_right_key_pressed) {
			renderSprite(tex_scrollbar_button_pressed_right.tex, Vector2(scrollbar_width - scrollbar_button_right_width, 0));
		} else {
			renderSprite(tex_scrollbar_button_right.tex, Vector2(scrollbar_width - scrollbar_button_right_width, 0));
		}
		// Background
		renderSprite(tex_scrollbar_bg_horiz_left.tex, Vector2(scrollbar_button_left_width, 0));
		renderSprite(tex_scrollbar_bg_horiz.tex, Vector2(scrollbar_button_left_width + scrollbar_leftend_width, 0), Vector2(scrollbar_width - scrollbar_leftend_width - scrollbar_rightend_width - scrollbar_button_left_width - scrollbar_button_right_width, scrollbar_height));
		renderSprite(tex_scrollbar_bg_horiz_right.tex, Vector2(scrollbar_width - scrollbar_rightend_width - scrollbar_button_right_width, 0));
		// Slider
		Texture* tex_left;
		Texture* tex;
		Texture* tex_right;
		if (slider_pressed) {
			tex_left = tex_scrollbar_pressed_horiz_left.tex;
			tex = tex_scrollbar_pressed_horiz.tex;
			tex_right = tex_scrollbar_pressed_horiz_right.tex;
		} else {
			tex_left = tex_scrollbar_horiz_left.tex;
			tex = tex_scrollbar_horiz.tex;
			tex_right = tex_scrollbar_horiz_right.tex;
		}
		uint32_t slider_leftend_width = tex_left->getWidth();
		uint32_t slider_rightend_width = tex_right->getWidth();
		uint32_t slider_area = scrollbar_width - scrollbar_button_left_width - scrollbar_button_right_width;
		Real slider_size = slider_area * scrollbar->getSliderSize();
		Real slider_pos = (slider_area - slider_size) * scrollbar->getValue();
// TODO: What it slider is too small?
		renderSprite(tex_left, Vector2(scrollbar_button_left_width + slider_pos, 0));
		renderSprite(tex, Vector2(scrollbar_button_left_width + slider_pos + slider_leftend_width, 0), Vector2(slider_size - slider_leftend_width - slider_rightend_width, -1));
		renderSprite(tex_right, Vector2(scrollbar_button_left_width + slider_pos + slider_size - slider_rightend_width, 0));
	} else {
		uint32_t scrollbar_topend_height = tex_scrollbar_bg_vert_top.tex->getHeight();
		uint32_t scrollbar_bottomend_height = tex_scrollbar_bg_vert_bottom.tex->getHeight();
		uint32_t scrollbar_button_up_height = tex_scrollbar_button_up.tex->getHeight();
		uint32_t scrollbar_button_down_height = tex_scrollbar_button_down.tex->getHeight();
		// Buttons
		if (up_or_left_key_pressed) {
			renderSprite(tex_scrollbar_button_pressed_up.tex, Vector2(0, 0));
		} else {
			renderSprite(tex_scrollbar_button_up.tex, Vector2(0, 0));
		}
		if (down_or_right_key_pressed) {
			renderSprite(tex_scrollbar_button_pressed_down.tex, Vector2(0, scrollbar_height - scrollbar_button_down_height));
		} else {
			renderSprite(tex_scrollbar_button_down.tex, Vector2(0, scrollbar_height - scrollbar_button_down_height));
		}
		// Background
		renderSprite(tex_scrollbar_bg_vert_top.tex, Vector2(0, scrollbar_button_up_height));
		renderSprite(tex_scrollbar_bg_vert.tex, Vector2(0, scrollbar_button_up_height + scrollbar_topend_height), Vector2(scrollbar_width, scrollbar_height - scrollbar_topend_height - scrollbar_bottomend_height - scrollbar_button_up_height - scrollbar_button_down_height));
		renderSprite(tex_scrollbar_bg_vert_bottom.tex, Vector2(0, scrollbar_height - scrollbar_bottomend_height - scrollbar_button_down_height));
		// Slider
		Texture* tex_top;
		Texture* tex;
		Texture* tex_bottom;
		if (slider_pressed) {
			tex_top = tex_scrollbar_pressed_vert_top.tex;
			tex = tex_scrollbar_pressed_vert.tex;
			tex_bottom = tex_scrollbar_pressed_vert_bottom.tex;
		} else {
			tex_top = tex_scrollbar_vert_top.tex;
			tex = tex_scrollbar_vert.tex;
			tex_bottom = tex_scrollbar_vert_bottom.tex;
		}
		uint32_t slider_topend_height = tex_top->getHeight();
		uint32_t slider_bottomend_height = tex_bottom->getHeight();
		uint32_t slider_area = scrollbar_height - scrollbar_button_up_height - scrollbar_button_down_height;
		Real slider_size = slider_area * scrollbar->getSliderSize();
		Real slider_pos = (slider_area - slider_size) * scrollbar->getValue();
// TODO: What it slider is too small?
		renderSprite(tex_top, Vector2(0, scrollbar_button_up_height + slider_pos));
		renderSprite(tex, Vector2(0, scrollbar_button_up_height + slider_pos + slider_topend_height), Vector2(-1, slider_size - slider_topend_height - slider_bottomend_height));
		renderSprite(tex_bottom, Vector2(0, scrollbar_button_up_height + slider_pos + slider_size - slider_bottomend_height));
	}

}

void ViewportRenderer::renderSlider(int32_t x_origin, int32_t y_origin, Slider const* slider, bool horizontal, bool slider_pressed)
{
	prepareSprites(x_origin, y_origin);

	if (horizontal) {
		uint32_t slider_width = slider->getWidth();
		Real bg_extra = (getHorizSliderHeight() - tex_slider_bg_horiz.tex->getHeight()) / 2.0;
		Real slider_extra = (getHorizSliderHeight() - tex_slider_horiz.tex->getHeight()) / 2.0;
		// First end of background
		renderSprite(tex_slider_bg_horiz_left.tex, Vector2(0, bg_extra));
		// Center of background
		renderSprite(tex_slider_bg_horiz.tex, Vector2(tex_slider_bg_horiz_left.tex->getWidth(), bg_extra), Vector2(slider_width - tex_slider_bg_horiz_left.tex->getWidth() - tex_slider_bg_horiz_right.tex->getWidth(), -1));
		// Second end of background
		renderSprite(tex_slider_bg_horiz_right.tex, Vector2(slider_width - tex_slider_bg_horiz_right.tex->getWidth(), bg_extra));
		// Slider
		Real no_slider_size = slider_width - tex_slider_horiz.tex->getWidth();
		Texture* tex_slider;
		if (!slider_pressed) {
			tex_slider = tex_slider_horiz.tex;
		} else {
			tex_slider = tex_slider_pressed_horiz.tex;
		}
		renderSprite(tex_slider, Vector2(no_slider_size * slider->getValue(), slider_extra));
	} else {
		uint32_t slider_height = slider->getHeight();
		Real bg_extra = (getVertSliderWidth() - tex_slider_bg_vert.tex->getWidth()) / 2.0;
		Real slider_extra = (getVertSliderWidth() - tex_slider_vert.tex->getWidth()) / 2.0;
		// First end of background
		renderSprite(tex_slider_bg_vert_top.tex, Vector2(bg_extra, 0));
		// Center of background
		renderSprite(tex_slider_bg_vert.tex, Vector2(bg_extra, tex_slider_bg_vert_top.tex->getHeight()), Vector2(-1, slider_height - tex_slider_bg_vert_top.tex->getHeight() - tex_slider_bg_vert_bottom.tex->getHeight()));
		// Second end of background
		renderSprite(tex_slider_bg_vert_bottom.tex, Vector2(bg_extra, slider_height - tex_slider_bg_vert_top.tex->getHeight()));
		// Slider
		Real no_slider_size = slider_height - tex_slider_vert.tex->getHeight();
		Texture* tex_slider;
		if (!slider_pressed) {
			tex_slider = tex_slider_vert.tex;
		} else {
			tex_slider = tex_slider_pressed_vert.tex;
		}
		renderSprite(tex_slider, Vector2(slider_extra, no_slider_size * slider->getValue()));
	}
}

void ViewportRenderer::renderTabs(int32_t x_origin, int32_t y_origin, Tabs const* tabs)
{
	prepareSprites(x_origin, y_origin);

	uint32_t width = tabs->getWidth();
	uint32_t height = tabs->getHeight();
	size_t selected = tabs->getSelected();
	size_t num_of_tabs = tabs->getNumOfTabs();

	// Render tab bar
	uint32_t selected_tab_begin = 0;
	uint32_t selected_tab_end = 0;
	uint32_t x_offset = getTabsLeftEdgeWidth();
	for (size_t tab_id = 0; tab_id < num_of_tabs; tab_id ++) {
		if (tab_id == selected) {
			selected_tab_begin = x_offset;
		}
		UnicodeString label = tabs->getTabLabel(tab_id);
		uint32_t label_width = font.getStringWidth(label, font_tablabel_size);

		renderSprite(tex_tab_left.tex, Vector2(x_offset, 0));
		x_offset += tex_tab_left.tex->getWidth();
		size_t label_pos_x = x_offset;
		renderSprite(tex_tab.tex, Vector2(x_offset, 0), Vector2(label_width, -1));
		x_offset += label_width;
		renderSprite(tex_tab_right.tex, Vector2(x_offset, 0));
		x_offset += tex_tab_right.tex->getWidth();

		textSetHorizontalAlign(CENTER);
		textSetVerticalAlign(CENTER);
		renderString(label, font_tablabel_size, Vector2(label_pos_x, 0), Vector2(label_width, tex_tab.tex->getHeight()));

		if (tab_id == selected) {
			selected_tab_end = x_offset;
		}
	}

	uint32_t tabbar_height = tex_tab.tex->getHeight() - tex_field_edge_top.tex->getHeight();

	// Render top edge
	renderSprite(tex_field_edge_bottomright_concave.tex, Vector2(0, tabbar_height));
	if (selected_tab_begin > tex_field_edge_right.tex->getWidth()) {
		renderSprite(tex_field_edge_bottom.tex, Vector2(tex_field_edge_right.tex->getWidth(), tabbar_height), Vector2(selected_tab_begin - tex_field_edge_right.tex->getWidth(), -1));
	}
	if (width > selected_tab_end + tex_field_edge_left.tex->getWidth()) {
		renderSprite(tex_field_edge_bottom.tex, Vector2(selected_tab_end, tabbar_height), Vector2(width - selected_tab_end - tex_field_edge_left.tex->getWidth(), -1));
	}
	renderSprite(tex_field_edge_bottomleft_concave.tex, Vector2(width - tex_field_edge_left.tex->getWidth(), tabbar_height));

	// Render side edges
	renderSprite(tex_field_edge_right.tex, Vector2(0, tex_tab.tex->getHeight()), Vector2(-1, height - tex_tab.tex->getHeight() - tex_field_edge_top.tex->getHeight()));
	renderSprite(tex_field_edge_left.tex, Vector2(width - tex_field_edge_left.tex->getWidth(), tex_tab.tex->getHeight()), Vector2(-1, height - tex_tab.tex->getHeight() - tex_field_edge_top.tex->getHeight()));

	// Render bottom edge
	renderSprite(tex_field_edge_topright_concave.tex, Vector2(0, height - tex_field_edge_top.tex->getHeight()));
	renderSprite(tex_field_edge_top.tex, Vector2(tex_field_edge_right.tex->getWidth(), height - tex_field_edge_top.tex->getHeight()), Vector2(width - tex_field_edge_right.tex->getWidth() - tex_field_edge_left.tex->getWidth(), -1));
	renderSprite(tex_field_edge_topleft_concave.tex, Vector2(width - tex_field_edge_left.tex->getWidth(), height - tex_field_edge_top.tex->getHeight()));
}

uint32_t ViewportRenderer::getMenubarHeight(void) const
{
	return tex_menubar_bg.tex->getHeight();
}

uint32_t ViewportRenderer::getMenuLabelWidth(UnicodeString const& label) const
{
	return font.getStringWidth(label, font_menu_size) + padding_menu_h*2;
}

uint32_t ViewportRenderer::getMenuseparatorMinWidth(void) const
{
	return tex_menuseparator_leftend.tex->getWidth() + tex_menuseparator_rightend.tex->getWidth();
}

uint32_t ViewportRenderer::getMenuseparatorHeight(void) const
{
	return tex_menuseparator.tex->getHeight();
}

uint32_t ViewportRenderer::getMenuitemWidth(UnicodeString const& label) const
{
	return round(font.getStringWidth(label, font_menuitem_size) + padding_menuitem_h * 2.0);
}

uint32_t ViewportRenderer::getMenuitemHeight(void) const
{
	return round(font_menuitem_size + padding_menuitem_v * 2.0);
}

uint32_t ViewportRenderer::getLabelWidth(UnicodeString const& label) const
{
	return font.getStringWidth(label, font_label_size);
}

uint32_t ViewportRenderer::getLabelHeight(size_t lines) const
{
	return font_label_size * lines;
}

void ViewportRenderer::getTextinputContentsCursorProps(uint32_t& cursor_pos_x, uint32_t& cursor_pos_y, uint32_t& cursor_width, uint32_t& cursor_height, UnicodeString const& value, ssize_t cursor) const
{
	cursor_pos_x = font.getStringWidth(value.substr(0, cursor), font_input_size);
	cursor_pos_y = 0;
	cursor_width = font.getStringWidth("|", font_input_size);
	cursor_height = font_input_size;
}

void ViewportRenderer::getTextinputEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const
{
	edge_top = tex_field_edge_top.tex->getHeight();
	edge_left = tex_field_edge_left.tex->getWidth();
	edge_right = tex_field_edge_right.tex->getWidth();
	edge_bottom = tex_field_edge_bottom.tex->getHeight();
}

uint32_t ViewportRenderer::getButtonWidth(UnicodeString const& label) const
{
	return tex_button_left.tex->getWidth() + font.getStringWidth(label, font_button_size) + tex_button_right.tex->getWidth();
}

uint32_t ViewportRenderer::getButtonHeight(void) const
{
	return tex_button.tex->getHeight();
}

uint32_t ViewportRenderer::getMinimumFolderviewWidth(void) const
{
	return folderview_min_width;
}

uint32_t ViewportRenderer::getFolderviewHeight(void) const
{
	return tex_folder.tex->getHeight() * folderview_min_rows + tex_field_edge_top.tex->getHeight() + tex_field_edge_bottom.tex->getHeight();
}

uint32_t ViewportRenderer::getMinimumFolderviewContentsWidth(UnicodeString const& label) const
{
	return tex_folder.tex->getWidth() + font.getStringWidth(label, font_input_size);
}

uint32_t ViewportRenderer::getFolderviewContentsHeight(size_t items) const
{
	return tex_folder.tex->getHeight() * items;
}

void ViewportRenderer::getFolderviewEdgeSizes(uint32_t& edge_top, uint32_t& edge_left, uint32_t& edge_right, uint32_t& edge_bottom) const
{
	edge_top = tex_field_edge_top.tex->getHeight();
	edge_left = tex_field_edge_left.tex->getWidth();
	edge_right = tex_field_edge_right.tex->getWidth();
	edge_bottom = tex_field_edge_bottom.tex->getHeight();
}

uint32_t ViewportRenderer::getScrollbarWidth(void) const
{
	return tex_scrollbar_bg_vert.tex->getWidth();
}

uint32_t ViewportRenderer::getScrollbarHeight(void) const
{
	return tex_scrollbar_bg_horiz.tex->getHeight();
}

void ViewportRenderer::setRenderareaLimit(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	x += rqueue->getPositionX();
	y = rqueue->getPositionY() + rqueue->getHeight() - y - height;
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

}

}
