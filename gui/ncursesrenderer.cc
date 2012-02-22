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
	size_t visible_left = getVisibleLeft();
	if (visible_left > 0) {
		ncout << c << std::flush;
	}
	advanceCursor(1);
}

void NCursesRenderer::printString(Hpp::UnicodeString const& str)
{
	size_t ofs = 0;
	while (ofs < str.size()) {
		size_t visible_left = getVisibleLeft();
		if (visible_left > 0) {
			size_t print_amount = std::min(visible_left, str.size() - ofs);
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
	if (limit &&
	    (cursor_col < (int32_t)limit_x || cursor_col >= (int32_t)(limit_x + limit_w) ||
	     cursor_row < (int32_t)limit_y || cursor_row >= (int32_t)(limit_y + limit_h))) {
		return;
	}
	if (cursor_col < 0 || cursor_col >= (int32_t)ncout.getScreenWidth() ||
	    cursor_row < 0 || cursor_row >= (int32_t)ncout.getScreenHeight()) {
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
}

void NCursesRenderer::deinitRendering(void)
{
	ncout.refresh();
}

void NCursesRenderer::renderMenubarBackground(int32_t x_origin, int32_t y_origin, uint32_t width, AreaWithMenubar const* areawithmenubar)
{
	setCursor(x_origin, y_origin);
	setColors(NC::BLACK, NC::GRAY);
	printString(std::string(width, ' '));
}

void NCursesRenderer::renderMenuLabel(int32_t x_origin, int32_t y_origin, Menu const* menu, UnicodeString const& label, bool mouse_over)
{
	setCursor(x_origin, y_origin);
	setColors(NC::BLACK, NC::GRAY);
	printChar(' ');
	printString(label);
	printChar(' ');
}

void NCursesRenderer::renderMenuseparator(int32_t x_origin, int32_t y_origin, Menuseparator const* menusep)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderMenuitem(int32_t x_origin, int32_t y_origin, Menuitem const* menuitem, UnicodeString const& label, bool mouse_over)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderWindow(int32_t x_origin, int32_t y_origin, Window const* window, UnicodeString const& title)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderLabel(int32_t x_origin, int32_t y_origin, Label const* label, UnicodeString const& label_str)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderTextinput(int32_t x_origin, int32_t y_origin, Textinput const* textinput)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderTextinputContents(int32_t x_origin, int32_t y_origin, TextinputContents const* textinputcontents, ssize_t cursor)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderButton(int32_t x_origin, int32_t y_origin, Button const* button, UnicodeString const& label, bool pressed)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderFolderview(int32_t x_origin, int32_t y_origin, Folderview const* folderview)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderFolderviewContents(int32_t x_origin, int32_t y_origin, FolderviewContents const* folderviewcontents, FolderChildren const& items)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderScrollbar(int32_t x_origin, int32_t y_origin, Scrollbar const* scrollbar, bool horizontal, bool up_or_left_key_pressed, bool down_or_right_key_pressed, bool slider_pressed)
{
	if (horizontal) {
		setCursor(x_origin, y_origin);
		setColors(NC::BLACK, NC::GRAY);
		printChar('<');

		setColors(NC::BLACK, NC::BLACK);
		printString(std::string(scrollbar->getWidth() - 2, '-'));

		setColors(NC::BLACK, NC::GRAY);
		printChar('>');
	} else {
		setCursor(x_origin, y_origin);
		setColors(NC::BLACK, NC::GRAY);
		printChar('^');

		setColors(NC::BLACK, NC::BLACK);
		for (uint32_t row = 1; row < scrollbar->getHeight() - 1; row ++) {
			setCursor(x_origin, y_origin + row);
			printChar('|');
		}

		setCursor(x_origin, y_origin + scrollbar->getHeight() - 1);
		setColors(NC::BLACK, NC::GRAY);
		printChar('v');
	}
}

void NCursesRenderer::renderSlider(int32_t x_origin, int32_t y_origin, Slider const* slider, bool horizontal, bool slider_pressed)
{
HppAssert(false, "Not implemented yet!");
}

void NCursesRenderer::renderTabs(int32_t x_origin, int32_t y_origin, Tabs const* tabs)
{
HppAssert(false, "Not implemented yet!");
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
