#ifndef HPP_EVENTMANAGER_H
#define HPP_EVENTMANAGER_H

#include "sdlhandler.h"
#include "event.h"

namespace Hpp
{

class Eventmanager
{

public:

	// Constructor and destructor
	Eventmanager(void);
	~Eventmanager(void);

	// Reads events
	Event getEvent(void);

private:

	// The SDL-handler of Eventmanager
	static Sdlhandler sdlhandler;

	// SDL event that is used when polling new events
	SDL_Event sdl_event;

	// The event that will be returned when calling getEvent() if it's not
	// NOTHING of it's type.
	Event event;

	// Event with type NOTHING to be returned when there is no events
	Event no_event;

};

}

#endif
