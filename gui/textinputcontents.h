#ifndef HPP_GUI_TEXTINPUTCONTENTS_H
#define HPP_GUI_TEXTINPUTCONTENTS_H

#include "renderer.h"
#include "widget.h"

#include "../unicodestring.h"

namespace Hpp
{

namespace Gui
{

class Textinput;

class TextinputContents : public Widget
{

public:

	inline TextinputContents(Textinput* textinput);
	inline virtual ~TextinputContents(void);

	inline void setValue(UnicodeString const& value);
	inline UnicodeString getValue(void) const { return value; }

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	Textinput* textinput;

	UnicodeString value;
	ssize_t cursor;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);
	virtual void onKeyDown(Key::Keycode keycode, UChr uchr);
	inline virtual void onKeyboardListeningStop(void);

	inline void reloadFolderContents(void);

	void updateScrolling(void);

};

inline TextinputContents::TextinputContents(Textinput* textinput) :
textinput(textinput),
cursor(-1)
{
}

inline TextinputContents::~TextinputContents(void)
{
}

inline void TextinputContents::setValue(UnicodeString const& value)
{
	this->value = value;
	markToNeedReposition();
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

inline void TextinputContents::onKeyboardListeningStop(void)
{
	cursor = -1;
}

}

}

#endif

