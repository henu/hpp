#ifndef WIDGETWITHBACKGROUND_H
#define WIDGETWITHBACKGROUND_H

#include "widget.h"
#include "../texture.h"
#include "../color.h"

namespace Hpp
{

namespace Gui
{

class WidgetWithBackground : public Widget
{

public:

	inline WidgetWithBackground(void);

	inline void setBackgroundTexture(Texture* tex, Color const& color = Color(1, 1, 1, 1)) { bg_tex = tex; bg_color = color; }
	inline void setColor(Color const& color) { bg_color = color; }

private:

	Texture* bg_tex;
	Color bg_color;

	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);

};

inline WidgetWithBackground::WidgetWithBackground(void) :
bg_tex(NULL),
bg_color(0, 0, 0, 0)
{
}

inline void WidgetWithBackground::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	return rend->renderBackground(x_origin, y_origin, getWidth(), getHeight(), bg_tex, bg_color);
}

}

}

#endif
