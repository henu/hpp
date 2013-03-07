#ifndef HPP_GUI_GUIEVENT_H
#define HPP_GUI_GUIEVENT_H

namespace Hpp
{

namespace Gui
{

class Widget;

class GuiEvent
{

	friend class Widget;

public:

	enum Type { WIDGET, KEYPRESS };

	inline Type getType(void) const { return type; }

	// For Widget event
	inline Widget* getWidget(void) const { return (Widget*)obj0; }
	inline int getAction(void) const { return int0; }

	// For Keypress event
	inline int getKeycode(void) const { return int0; }

private:

	// Functions for friends
	inline static GuiEvent fromWidget(Widget* widget, int action);
	inline static GuiEvent fromKeypress(int keycode);

private:

	Type type;
	void* obj0;
	int int0;

};

inline GuiEvent GuiEvent::fromWidget(Widget* widget, int action)
{
	GuiEvent result;
	result.type = WIDGET;
	result.obj0 = widget;
	result.int0 = action;
	return result;
}
inline GuiEvent GuiEvent::fromKeypress(int keycode)
{
	GuiEvent result;
	result.type = KEYPRESS;
	result.int0 = keycode;
	return result;
}

}

}

#endif
