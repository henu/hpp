#include "ncursesrenderer.h"

#include "scrollbar.h"

#include "../ncios.h"

namespace Hpp
{

namespace Gui
{

NCursesRenderer::NCursesRenderer(void) :
cursor_col(0), cursor_row(0),
limit(false)
{
}

NCursesRenderer::~NCursesRenderer(void)
{
}

void NCursesRenderer::setCursor(int32_t col, int32_t row)
{
	cursor_col = col;
	cursor_row = row;
	positionCursorIfAtRenderarea();
}

void NCursesRenderer::setColors(NC::Color text, NC::Color bg)
{
	ncout.setColors(text, bg);
}

void NCursesRenderer::printChar(char c)
{
	if (c < 0x20) {
		throw Exception("Invalid character!");
	}
	size_t visible_left = getVisibleLeft();
	if (visible_left > 0) {
		hideTextCursor(1);
		ncout << c << std::flush;
	}
	advanceCursor(1);
}

void NCursesRenderer::printString(UnicodeString const& str)
{
	// Ensure there are no invalid characters in string
	for (UnicodeString::const_iterator str_it = str.begin();
	     str_it != str.end();
	     ++ str_it) {
		UChr c = *str_it;
		if (c < 0x20) {
			throw Exception("String contains invalid characters!");
		}
	}

	// Do printing
	size_t ofs = 0;
	while (ofs < str.size()) {
		size_t visible_left = getVisibleLeft();
		if (visible_left > 0) {
			size_t print_amount = std::min(visible_left, str.size() - ofs);
			hideTextCursor(print_amount);
			ncout << str.substr(ofs, print_amount) << std::flush;
			ofs += print_amount;
			advanceCursor(print_amount);
		} else {
			size_t hidden_left = getHiddenLeft();
			size_t print_amount = std::min(hidden_left, str.size() - ofs);
			ofs += print_amount;
			advanceCursor(print_amount);
		}
	}
}

void NCursesRenderer::positionCursorIfAtRenderarea(void)
{
	if (!isAtRenderarea(cursor_col, cursor_row)) {
		return;
	}
	ncout.setLoc(cursor_col, cursor_row);
}

void NCursesRenderer::advanceCursor(size_t amount)
{
	cursor_col += amount;
	positionCursorIfAtRenderarea();
}

size_t NCursesRenderer::getVisibleLeft(void) const
{
	if (!limit) {
		if (cursor_row < 0) return 0;
		if (cursor_row > (int32_t)ncout.getScreenHeight()) return 0;
		if (cursor_col < 0) return 0;
		int32_t scr_w = ncout.getScreenWidth();
		if (cursor_col < scr_w) return scr_w - cursor_col;
		return 0;
	}
	if (cursor_row >= (int32_t)(limit_y + limit_h)) return 0;
	if (cursor_row >= (int32_t)limit_y) {
		if (cursor_col < (int32_t)(limit_x)) return 0;
		if (cursor_col < (int32_t)(limit_x + limit_w)) return limit_x + limit_w - cursor_col;
		return 0;
	}
	return 0;
}

size_t NCursesRenderer::getHiddenLeft(void) const
{
	if (!limit) {
		if (cursor_row < 0) return (size_t)-1;
		if (cursor_row > (int32_t)ncout.getScreenHeight()) return (size_t)-1;
		if (cursor_col < 0) return -cursor_col;
		int32_t scr_w = ncout.getScreenWidth();
		if (cursor_col < scr_w) return 0;
		return (size_t)-1;
	}
	if (!limit) return 0;
	if (cursor_row >= (int32_t)(limit_y + limit_h)) return (size_t)-1;
	if (cursor_row >= (int32_t)limit_y) {
		if (cursor_col < (int32_t)limit_x) return limit_x - cursor_col;
		if (cursor_col < (int32_t)(limit_x + limit_w)) return 0;
		return (size_t)-1;
	}
	return (size_t)-1;
}

bool NCursesRenderer::isAtRenderarea(int32_t col, int32_t row) const
{
	if (limit &&
	    (col < (int32_t)limit_x || col >= (int32_t)(limit_x + limit_w) ||
	     row < (int32_t)limit_y || row >= (int32_t)(limit_y + limit_h))) {
		return false;
	}
	if (col < 0 || col >= (int32_t)ncout.getScreenWidth() ||
	    row < 0 || row >= (int32_t)ncout.getScreenHeight()) {
		return false;
	}
	return true;
}

void NCursesRenderer::hideTextCursor(size_t chars)
{
	if (!textcursor_visible) return;
	if (cursor_row != textcursor_row) return;
	if (cursor_col > textcursor_col) return;
	if ((ssize_t)cursor_col + chars <= (ssize_t)textcursor_col) return;
	textcursor_visible = false;
}

uint32_t NCursesRenderer::getWidth(void) const
{
	return ncout.getScreenWidth();
}

uint32_t NCursesRenderer::getHeight(void) const
{
	return ncout.getScreenHeight();
}

void NCursesRenderer::initRendering(void)
{
	textcursor_visible = false;
}

void NCursesRenderer::deinitRendering(void)
{
	// If textcursor was left visible, then position it
	if (textcursor_visible) {
		ncout.setCursorVisible(true);
		ncout.setLoc(textcursor_col, textcursor_row);
	} else {
		ncout.setCursorVisible(false);
	}

	ncout.refresh();
}

void NCursesRenderer::renderMenubarBackground(int32_t x_origin, int32_t y_origin, uint32_t width, AreaWithMenubar const* areawithmenubar)
{
	(void)areawithmenubar;
	setCursor(x_origin, y_origin);
	setColors(NC::BLACK, NC::GRAY);
	printString(std::string(width, ' '));
}

void NCursesRenderer::renderMenuLabel(int32_t x_origin, int32_t y_origin, Menu const* menu, UnicodeString const& label, bool mouse_over)
{
	(void)menu;
	(void)mouse_over;
	setCursor(x_origin, y_origin);
	setColors(NC::BLACK, NC::GRAY);
	printChar(' ');
	printString(label);
	printChar(' ');
}

void NCursesRenderer::renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep)
{
(void)x_origin;
(void)y_origin;
(void)menusep;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over)
{
(void)x_origin;
(void)y_origin;
(void)menuitem;
(void)label;
(void)mouse_over;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title)
{
(void)x_origin;
(void)y_origin;
(void)window;
(void)title;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str)
{
(void)x_origin;
(void)y_origin;
(void)label;
(void)label_str;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput)
{
(void)x_origin;
(void)y_origin;
(void)textinput;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderTextinputContents(int32_t x_origin, int32_t y_origin, TextinputContents const* textinputcontents, ssize_t cursor)
{
(void)x_origin;
(void)y_origin;
(void)textinputcontents;
(void)cursor;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed)
{
(void)x_origin;
(void)y_origin;
(void)button;
(void)label;
(void)pressed;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderFolderview(int32_t x_origin, int32_t y_origin, Folderview const* folderview)
{
(void)x_origin;
(void)y_origin;
(void)folderview;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, Path::DirChildren const& items)
{
(void)x_origin;
(void)y_origin;
(void)folderviewcontents;
(void)items;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderScrollbar(int32_t x_origin, int32_t y_origin, Scrollbar const* scrollbar, bool horizontal, bool up_or_left_key_pressed, bool down_or_right_key_pressed, bool slider_pressed)
{
	(void)down_or_right_key_pressed;
	(void)up_or_left_key_pressed;
	(void)slider_pressed;
	Real slider_size_rel = scrollbar->getSliderSize();
	Real value = scrollbar->getValue();

	if (horizontal) {
		// Calculate absolute position and size of slider
		uint32_t slider_size = (getWidth() - 2) * slider_size_rel + 0.5;
		uint32_t slider_pos = (getWidth() - 2 - slider_size) * value + 0.5;

		// Render
		setCursor(x_origin, y_origin);
		setColors(NC::BLACK, NC::GRAY);
		printChar('<');

		setColors(NC::DARK_GRAY, NC::BLACK);
		printString(std::string(slider_pos, '-'));
		setColors(NC::BLACK, NC::GRAY);
		printString(std::string(slider_size, ' '));
		setColors(NC::DARK_GRAY, NC::BLACK);
		HppAssert(scrollbar->getWidth() >= 2 + slider_pos + slider_size, "Fail!");
		printString(std::string(scrollbar->getWidth() - 2 - slider_pos - slider_size, '-'));

		setColors(NC::BLACK, NC::GRAY);
		printChar('>');
	} else {
		// Calculate absolute position and size of slider
		uint32_t slider_size = (getHeight() - 2) * slider_size_rel + 0.5;
		uint32_t slider_pos = (getHeight() - 2 - slider_size) * value + 0.5;

		// Render
		setCursor(x_origin, y_origin);
		setColors(NC::BLACK, NC::GRAY);
		printChar('^');

		for (uint32_t row = 1; row < scrollbar->getHeight() - 1; row ++) {
			setCursor(x_origin, y_origin + row);
			if (row - 1 >= slider_pos && row - 1 < slider_pos + slider_size) {
				setColors(NC::BLACK, NC::GRAY);
				printChar(' ');
			} else {
				setColors(NC::DARK_GRAY, NC::BLACK);
				printChar('|');
			}
		}

		setCursor(x_origin, y_origin + scrollbar->getHeight() - 1);
		setColors(NC::BLACK, NC::GRAY);
		printChar('v');
	}
}

void NCursesRenderer::renderSlider(int32_t x_origin, int32_t y_origin, Slider const* slider, bool horizontal, bool slider_pressed)
{
(void)x_origin;
(void)y_origin;
(void)slider;
(void)horizontal;
(void)slider_pressed;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderTabs(int32_t x_origin, int32_t y_origin, Tabs const* tabs)
{
(void)x_origin;
(void)y_origin;
(void)tabs;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderBackground(int32_t x_origin, int32_t y_origin, int32_t width, int32_t height, Texture* tex, Color const& color)
{
(void)x_origin;
(void)y_origin;
(void)width;
(void)height;
(void)tex;
(void)color;
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderTextCursor(int32_t x_origin, int32_t y_origin)
{
	if (isAtRenderarea(x_origin, y_origin)) {
		textcursor_visible = true;
		textcursor_col = x_origin;
		textcursor_row = y_origin;
	}
}

void NCursesRenderer::setRenderareaLimit(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	limit_x = x;
	limit_y = y;
	limit_w = width;
	limit_h = height;
	limit = true;
	positionCursorIfAtRenderarea();
}

void NCursesRenderer::removeRenderareaLimit(void)
{
	limit = false;
}

}

}
