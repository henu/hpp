#ifndef HPP_STATE_H
#define HPP_STATE_H

#include "event.h"
#include "key.h"
#include "mousekey.h"
#include "statemanager.h"
#include "time.h"

namespace Hpp
{

class Statemanager;

class State
{

	// Friends
	friend class Statemanager;

public:

	// Constructor and destructor
	inline State(void) { }
	inline virtual ~State(void) { }

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	// Functions to run and render State. These are called by friend
	// Statemanager.
	virtual void run(Delay const& delta_time) = 0;
	virtual void render(void) = 0;

	// Functions to handle order changing in stack
	virtual void putBackground(void) = 0;
	virtual void resume(void) = 0;
	virtual void enter(void) = 0;
	virtual void exit(void) = 0;

	// Function to deliver Events to State. This is called by Statemanager.
	inline void pushEvent(Event const& event);

	// Sets/gets background state.
	inline void setBackgroundState(bool bg) { bg_state = bg; }
	inline bool getBackgroundState(void) const { return bg_state; }

protected:

	// Hides/reveals mouse cursor
	inline void hideMousecursor(void) { SDL_ShowCursor(SDL_DISABLE); }
	inline void revealMousecursor(void) { SDL_ShowCursor(SDL_ENABLE); }

	// Grabs/releases input
	inline void grabInput(void) { SDL_WM_GrabInput(SDL_GRAB_ON); }
	inline void releaseInput(void) { SDL_WM_GrabInput(SDL_GRAB_OFF); }

	// Returns information about specific key
	inline bool getKeyPressed(Key::Keycode key) const;
	inline bool getMousePressed(Mousekey::Keycode key) const;

	// Stops and quits application
	inline void stop(void);

	// Returns true if this is the topstate in the stack
	inline bool isTopstate(void) const { return Statemanager::getTopstate() == this; }

private:

	// Boolean if state is at background. This is used to define if state
	// is on the background or not. Using these, we can prevent useless
	// resume/putBackground calls.
	bool bg_state;

	// Function to handle events got from eventmanager
	virtual void handleEvent(Event const& event) = 0;

};

inline void State::pushEvent(Event const& event)
{
	// Deliver event to subclass
	handleEvent(event);
}

inline bool State::getKeyPressed(Key::Keycode key) const
{
	return SDL_GetKeyState(NULL)[key];
}

inline bool State::getMousePressed(Mousekey::Keycode key) const
{
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(key);
}

inline void State::stop(void)
{
	Statemanager::stop();
}

}

#endif
