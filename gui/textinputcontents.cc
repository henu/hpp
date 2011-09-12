#include "textinputcontents.h"

#include "textinput.h"

namespace Hpp
{

namespace Gui
{

void TextinputContents::onKeyDown(Key::Keycode keycode, UChr uchr)
{
	(void)keycode;
	if (keycode == Key::LEFT) {
		cursor --;
		if (cursor < 0) cursor = 0;
		updateScrolling();
	} else if (keycode == Key::RIGHT) {
		cursor ++;
		if (cursor > (ssize_t)value.size()) cursor = value.size();
		updateScrolling();
	} else if (keycode == Key::HOME) {
		cursor = 0;
		updateScrolling();
	} else if (keycode == Key::END) {
		cursor = value.size();
		updateScrolling();
	} else if (keycode == Key::BACKSPACE) {
		if (cursor > 0) {
			value = value.substr(0, cursor - 1) + value.substr(cursor);
			cursor --;
			markSizeChanged();
			updateScrolling();
		}
	} else if (keycode == Key::DEL) {
		value = value.substr(0, cursor) + value.substr(cursor + 1);
		markSizeChanged();
		updateScrolling();
	} else if (keycode == Key::RETURN) {
		cursor = -1;
		stopListeningKeyboard();
		textinput->submitted();
	} else if (uchr >= 32 && cursor >= 0) {
		value = value.substr(0, cursor) + uchr + value.substr(cursor);
		cursor ++;
		markSizeChanged();
		updateScrolling();
	}
}

void TextinputContents::updateScrolling(void)
{
	Renderer const* rend = getRenderer();
	if (!rend || cursor < 0) {
		return;
	}
	// Get cursor position and size
	uint32_t cursor_pos_x, cursor_pos_y;
	uint32_t cursor_width, cursor_height;
	rend->getTextinputContentsCursorProps(cursor_pos_x, cursor_pos_y, cursor_width, cursor_height, value, cursor);
	textinput->scrollToCursor(cursor_pos_x, cursor_pos_y, cursor_width, cursor_height);
}

}

}
