#ifndef HPP_EVENT_H
#define HPP_EVENT_H

#include "key.h"
#include "mousekey.h"

#include "SDL.h"

namespace Hpp
{

// TODO: Use union here!
struct Event
{

	// Enumeration for the type of event
	enum Type { NOTHING = 0,
	            QUIT,
	            KEY_DOWN,
	            KEY_UP,
	            MOUSE_MOVE,
	            MOUSE_KEY_DOWN,
	            MOUSE_KEY_UP };

	// The type of event
	Type type;

	// For KEYDOWN and KEUYUP
	Key::Keycode key;
	uint16_t unicode;

	// For MOUSEMOVE, MOUSEKEYDOWN and MOUSEKEYUP
	uint16_t x;
	uint16_t y;
	int16_t x_rel;
	int16_t y_rel;

	// For MOUSEKEYDOWN and MOUSEKEYUP
	Mousekey::Keycode mousekey;

};

}

#endif
