#ifndef HPP_GUI_WIDGET_H
#define HPP_GUI_WIDGET_H

#include <cstdlib>
#include <stdint.h>

namespace Hpp
{

namespace Gui
{

class Engine;

class Widget
{

	friend class Engine;

public:

	inline Widget(void) : engine(NULL), parent(NULL), x(0), y(0), width(0), height(0) { }
	inline virtual ~Widget(void) { }

	inline int32_t getPositionX(void) const { return x; }
	inline int32_t getPositionY(void) const { return y; }
	inline int32_t getWidth(void) const { return width; }
	inline int32_t getHeight(void) const { return height; }

protected:

	inline void setPosition(int32_t x, int32_t y) { this->x = x; this->y = y; }
	inline void setSize(uint32_t width, uint32_t height) { this->width = width; this->height = height; }

private:

	// Called by engine
	inline void setEngine(Engine* engine) { this->engine = engine; }
	inline void setParent(Widget* parent) { this->parent = parent; }

private:

	Engine* engine;
	Widget* parent;

	// Position and size
	int32_t x, y;
	uint32_t width, height;

};

}

}

#endif
