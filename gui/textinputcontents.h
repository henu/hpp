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

	Path path;
	FolderChildren items;

	UnicodeString value;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);

	inline void reloadFolderContents(void);

};

inline TextinputContents::TextinputContents(void)
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
	return rend->renderTextinputContents(x_origin, y_origin, this);
}

}

}

#endif

