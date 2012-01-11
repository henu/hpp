#ifndef HPP_MOUSEKEY_H
#define HPP_MOUSEKEY_H

#ifndef HPP_NO_SDL
#include <SDL/SDL.h>
#endif
#include <stdint.h>

namespace Hpp
{

namespace Mousekey
{

#ifndef HPP_NO_SDL
enum Keycode { LEFT = SDL_BUTTON_LEFT,
               MIDDLE = SDL_BUTTON_MIDDLE,
               RIGHT = SDL_BUTTON_RIGHT,
               WHEEL_UP = SDL_BUTTON_WHEELUP,
               WHEEL_DOWN = SDL_BUTTON_WHEELDOWN };
#else
enum Keycode { LEFT,
               MIDDLE,
               RIGHT,
               WHEEL_UP,
               WHEEL_DOWN };
#endif

typedef uint8_t KeycodeFlags;
KeycodeFlags const FLAG_LEFT		= 0x01;
KeycodeFlags const FLAG_MIDDLE		= 0x02;
KeycodeFlags const FLAG_RIGHT		= 0x04;
KeycodeFlags const FLAG_WHEEL_UP	= 0x08;
KeycodeFlags const FLAG_WHEEL_DOWN	= 0x10;

}

}

#endif
