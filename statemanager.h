#ifndef HPP_STATEMANAGER_H
#define HPP_STATEMANAGER_H

#include "eventmanager.h"
#include "sdlhandler.h"
#include "thread.h"
#include "time.h"

#include <SDL/SDL.h>
#include <list>
#include <map>
#include <set>
#include <vector>


namespace Hpp
{

// Classes that doesn't need their headers included for now
class Joystick;
class State;

class Statemanager
{

	// State needs to have access to some special operations
	friend class State;

	// Eventmanager needs to update states of input devices
	friend class Eventmanager;

	// Joystick needs to register and unregister itself
	friend class Joystick;

public:

	// Pushes new states to manager
	static void pushState(State* state);

	// Pops and returns the topstate from stack.
	static State* popState(void);

	// Pops and later deletes topstate from stack.
	static void popAndDestroyState(void);

	// Starts the manager (and states)
	static void start(void);

	// Sets/gets desired fps. 0 means there is no desired fps.
	inline static void setDesiredFps(size_t desired_fps) { instance.desired_fps = desired_fps; }
	inline static size_t getDesiredFps(void) { return instance.desired_fps; }

	// Sets/gets maximum FPS. 0 for no limit
	inline static void setMaxFps(unsigned short max_fps) { instance.max_fps = max_fps; }
	inline static unsigned short getMaxFps(void) { return instance.max_fps; }

	static float getFps(void);

	// Sets/gets max frame dropping. 0 for no limit.
	inline static void setMaxFramedrop(unsigned short max_framedrop) { instance.max_framedrop = max_framedrop; }
	inline static unsigned short getMaxFramedrop(void) { return instance.max_framedrop; }

	// Get's topstate of the stack of states
	inline static State* getTopstate(void) { return instance.states.empty() ? NULL : instance.states.back(); };

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	// Stops and quits. This is called trough State::stop() by it's
	// subclass.
	inline static void stop(void) { instance.stopped = true; }

	// Registers and unregisters Joysticks. These are called by constructor
	// and destructor of Joysticks.
	static SDL_Joystick* registerJoystick(Joystick* joy, unsigned int id);
	static void unregisterJoystick(Joystick* joy, unsigned int id);

	// Sets state of specific joystick. These are called by event handling
	// function of Eventmanager.
	void static setJoystickAxis(unsigned char joystick_id,
	                            unsigned char axis_id,
	                            short value);
	void static setJoystickButton(unsigned char joystick_id,
	                              unsigned char button_id,
	                              bool pressed);

private:

	// Vector of states
	typedef std::vector< State* > Statevector;

	// Container for input devices
	typedef std::set< Joystick* > Joysticks;
	typedef std::map< unsigned int, Joysticks > JoysticksById;

	// Container of SDL Joysticks by ID
	typedef std::map< unsigned int, SDL_Joystick* > SDLJoysticks;

	// The thread that statemanager is ran. This is for debugging purposes
	// only.
	Thread::Id thread_id;

	// The only instance of this class
	static Statemanager instance;

	// The Eventmanager to handle events
	Eventmanager eventmanager;

	// Stakes in manager
	Statevector states;
	Statevector removable_states;

	// Registered inputdevices
	JoysticksById joys;
	SDLJoysticks sdljoys;

	// Desired FPS
	size_t desired_fps;

	// Maximum FPS and framedrop
	unsigned short max_fps;
	unsigned short max_framedrop;

	// Boolean that indicates whenever the mainloop is wanted to stop
	bool stopped;

	// Counter for rendered frames and last moment when FPS was asked.
	size_t fps_framecount;
	Time fps_last_querytime;

	Statemanager(void);
	~Statemanager(void);

};

}

#endif
