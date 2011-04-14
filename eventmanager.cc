#include "eventmanager.h"

#include "statemanager.h"

#include "cast.h"
#include "assert.h"
#include "exception.h"

namespace Hpp
{

// The SDL-handler of Eventmanager
Sdlhandler Eventmanager::sdlhandler(Sdlhandler::JOYSTICK);

Eventmanager::Eventmanager(void)
{
	// Initialize no_event
	no_event.type = Event::NOTHING;
}

Eventmanager::~Eventmanager(void)
{
}

Event Eventmanager::getEvent(void)
{

	if (SDL_PollEvent(&sdl_event)) {

		switch (sdl_event.type) {

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if (sdl_event.key.type == SDL_KEYDOWN) {
				event.type = Event::KEY_DOWN;
			} else {
				HppAssert(sdl_event.key.type == SDL_KEYUP, "");
				event.type = Event::KEY_UP;
			}
			event.key = static_cast< Key::Keycode >(sdl_event.key.keysym.sym);
			event.unicode = sdl_event.key.keysym.unicode;
			return event;
			break;

		case SDL_MOUSEMOTION:
			event.type = Event::MOUSE_MOVE;
			event.x = sdl_event.motion.x;
			event.y = sdl_event.motion.y;
			event.x_rel = sdl_event.motion.xrel;
			event.y_rel = sdl_event.motion.yrel;
			return event;
			break;

		case SDL_MOUSEBUTTONDOWN:
			event.type = Event::MOUSE_KEY_DOWN;
			event.mousekey = static_cast< Mousekey::Keycode >(sdl_event.button.button);
			event.x = sdl_event.button.x;
			event.y = sdl_event.button.y;
			return event;
			break;

		case SDL_MOUSEBUTTONUP:
			event.type = Event::MOUSE_KEY_UP;
			event.mousekey = static_cast< Mousekey::Keycode >(sdl_event.button.button);
			event.x = sdl_event.button.x;
			event.y = sdl_event.button.y;
			return event;
			break;

		case SDL_JOYAXISMOTION:
			Statemanager::setJoystickAxis(sdl_event.jaxis.which,
			                              sdl_event.jaxis.axis,
			                              sdl_event.jaxis.value);
			break;

		case SDL_JOYBUTTONDOWN:
			Statemanager::setJoystickButton(sdl_event.jbutton.which,
			                                sdl_event.jbutton.button,
			                                sdl_event.jbutton.state == SDL_PRESSED);
			break;

		case SDL_QUIT:
			event.type = Event::QUIT;
			return event;
			break;

		default:
			break;

		}

	}

	return no_event;

}

}
