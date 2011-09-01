#ifndef HPP_GUI_TEXTINPUTCONTENTS_H
#define HPP_GUI_TEXTINPUTCONTENTS_H

#include "widget.h"

#include "../unicodestring.h"

namespace Hpp
{

namespace Gui
{

class TextinputContents : public Widget
{

public:

	inline TextinputContents(void);
	inline virtual ~TextinputContents(void);

	inline void setValue(UnicodeString const& value);
	inline UnicodeString getValue(void) const { return value; }

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	UnicodeString value;
	ssize_t cursor;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	inline virtual void onKeyDown(Key::Keycode keycode, UChr uchr);
	inline virtual void onKeyboardListeningStop(void);

	inline void reloadFolderContents(void);

};

inline TextinputContents::TextinputContents(void) :
cursor(-1)
{
}

inline TextinputContents::~TextinputContents(void)
{
}

inline void TextinputContents::setValue(UnicodeString const& value)
{
	this->value = value;
	markSizeChanged();
}

inline uint32_t TextinputContents::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getMinimumTextinputContentsWidth(value);
}

inline uint32_t TextinputContents::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getTextinputContentsHeight();
}

inline void TextinputContents::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	return rend->renderTextinputContents(x_origin, y_origin, this, cursor);
}

inline bool TextinputContents::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	if (mouse_key == Mousekey::LEFT) {
		listenKeyboard();
		cursor = value.size();
		return true;
	}
	return false;
}

inline void TextinputContents::onKeyDown(Key::Keycode keycode, UChr uchr)
{
	(void)keycode;
	if (keycode == Key::LEFT) {
		cursor --;
		if (cursor < 0) cursor = 0;
	} else if (keycode == Key::RIGHT) {
		cursor ++;
		if (cursor > (ssize_t)value.size()) cursor = value.size();
	} else if (keycode == Key::HOME) {
		cursor = 0;
	} else if (keycode == Key::END) {
		cursor = value.size();
	} else if (keycode == Key::BACKSPACE) {
		if (cursor > 0) {
			value = value.substr(0, cursor - 1) + value.substr(cursor);
			cursor --;
		}
	} else if (keycode == Key::DELETE) {
		value = value.substr(0, cursor) + value.substr(cursor + 1);
	} else if (uchr >= 32 && cursor >= 0) {
		value = value.substr(0, cursor) + uchr + value.substr(cursor);
		cursor ++;
	}
}

inline void TextinputContents::onKeyboardListeningStop(void)
{
	cursor = -1;
}

}

}

#endif

