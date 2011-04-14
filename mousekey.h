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
}

}

#endif
