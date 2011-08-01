#ifndef HPP_MOUSEKEY_H
#define HPP_MOUSEKEY_H

#include "SDL.h"

namespace Hpp
{

namespace Mousekey
{

enum Keycode { LEFT = SDL_BUTTON_LEFT,
               MIDDLE = SDL_BUTTON_MIDDLE,
               RIGHT = SDL_BUTTON_RIGHT,
               WHEEL_UP = SDL_BUTTON_WHEELUP,
               WHEEL_DOWN = SDL_BUTTON_WHEELDOWN };

typedef uint8_t KeycodeFlags;
KeycodeFlags const FLAG_LEFT		= 0x01;
KeycodeFlags const FLAG_MIDDLE		= 0x02;
KeycodeFlags const FLAG_RIGHT		= 0x04;
KeycodeFlags const FLAG_WHEEL_UP	= 0x08;
KeycodeFlags const FLAG_WHEEL_DOWN	= 0x10;

}

}

#endif
