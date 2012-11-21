#ifndef HPP_EVENT_H
#define HPP_EVENT_H

#include "key.h"
#include "mousekey.h"

#ifndef HPP_NO_SDL
#include <SDL/SDL.h>
#endif

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
	            MOUSE_KEY_UP,
	            END_OF_EVENTS };

	// The type of event
	Type type;

	// For KEYDOWN and KEUYUP
	Key::Keycode key;
	uint16_t unicode;

	// For MOUSEMOVE, MOUSEKEYDOWN and MOUSEKEYUP
// TODO: Support mouse coordinates outside screen as well!
	uint16_t x;
	uint16_t y;
	int16_t x_rel;
	int16_t y_rel;

	// For MOUSEKEYDOWN and MOUSEKEYUP
	Mousekey::Keycode mousekey;

};

}

#endif
