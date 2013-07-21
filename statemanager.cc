#include "statemanager.h"

#include "joystick.h"
#include "state.h"
#include "cast.h"
#include "assert.h"
#include "exception.h"
#include "display.h"

#include <SDL/SDL.h>
#include <algorithm>

namespace Hpp
{

Statemanager Statemanager::instance;

void Statemanager::pushState(State* state)
{
	HppAssert(instance.thread_id == getThisThreadID(), "Invalid thread!");
	HppAssert(state, "");
	HppAssert(getTopstate() != state, "");
	HppAssert(std::find(instance.states.begin(), instance.states.end(), state) == instance.states.end(), "State is already in stack!");
	instance.states.push_back(state);
	state->setBackgroundState(false);
	state->enter();
}

State* Statemanager::popState(void)
{
	HppAssert(instance.thread_id == getThisThreadID(), "Invalid thread!");
	HppAssert(!instance.states.empty(), "No states to pop!");
	// Get and pop topstack
	State* state = instance.states.back();
	instance.states.pop_back();
	// Inform states about the changes
	state->exit();
	return state;
}

void Statemanager::popAndDestroyState(void)
{
	HppAssert(instance.thread_id == getThisThreadID(), "Invalid thread!");
	HppAssert(!instance.states.empty(), "No states to pop!");
	// Get and pop topstack
	State* state = instance.states.back();
	instance.states.pop_back();
	// Inform states about the changes
	state->exit();
	HppAssert(std::find(instance.removable_states.begin(), instance.removable_states.end(), state) == instance.removable_states.end(), "State already marked removable!");
	instance.removable_states.push_back(state);
}

void Statemanager::start(void)
{
	HppAssert(instance.thread_id == getThisThreadID(), "Invalid thread!");

	// Reset variables
	instance.stopped = false;
	instance.fps_framecount = 0;
	instance.fps_last_querytime = now();
	unsigned int dropped_frames = 0;

	// Reset lasttime
	Time lasttime = now();
// TODO: Rewrite this whole function!

	// Seconds multiplier, maximum value of it and the remainings of it
	// from the last loop
	Delay dtime;
	Delay dtime_max;
	Delay dtime_remains = Delay::secs(0);

	do {

		if (instance.desired_fps != 0) {
			dtime_max = Delay::secs(1) / instance.desired_fps;
		}
		#ifndef NDEBUG
		else dtime_max = Delay::days(999999);
		#endif
		Delay dtime_min;
		if (instance.max_fps > 0) {
			dtime_min = Delay::secs(1) / instance.max_fps;
			HppAssert(dtime_min < dtime_max, "");
		}
		#ifndef NDEBUG
		else dtime_min = Delay::secs(0.0);
		#endif

		// Get the passed time since last start of
		// loop and update the current last_time.
		Time time_now = now();
		Delay timediff = time_now - lasttime;
		lasttime = time_now;

		// Calculate seconds multiplier
		dtime = timediff;
		HppAssert(dtime >= Delay::secs(0), "Negative time!");

		// Add remainings of seconds multiplier from last loop to this
		// one
		dtime += dtime_remains;
		dtime_remains = Delay::secs(0);

		// Check if program is running too fast
		if (instance.max_fps > 0 && dtime < dtime_min) {
			(dtime_min - dtime).sleep();
			dtime = dtime_min;
		}
		// If seconds multiplier exceeds the maximum value then store
		// the remainings
		if (instance.desired_fps != 0 && dtime > dtime_max) {
			dtime_remains = dtime - dtime_max;
			dtime = dtime_max;
		}

		// Read events and deliver them to the state at the top of
		// stack.
		Event event;
		bool events_occured = false;
		while (!instance.states.empty() &&
		       (event = instance.eventmanager.getEvent()).type != Event::NOTHING) {
// TODO: Code some own event type! -- What? There is already own event type, right?
			instance.states.back()->pushEvent(event);
			events_occured = true;
		}
		// If events occured, then finally inform that
		// no more events are coming during this frame
		if (events_occured && !instance.states.empty()) {
			Event end_of_events;
			end_of_events.type = Event::END_OF_EVENTS;
			instance.states.back()->pushEvent(end_of_events);
		}

		// Before running states, go all of them through and check if
		// some has been put to background or resumed.
		for (size_t states_id = 0; states_id < instance.states.size(); states_id ++) {
			State* state = instance.states[states_id];
			if (states_id != instance.states.size() - 1) {
				if (!state->getBackgroundState()) {
					state->setBackgroundState(true);
					state->putBackground();
				}
			} else {
				if (state->getBackgroundState()) {
					state->setBackgroundState(false);
					state->resume();
				}
			}
		}

		// Go States through and run them
		size_t states_id = instance.states.size();
		while (states_id > 0) {
			do {
				states_id --;
			} while (states_id >= instance.states.size());
			State* state = instance.states[states_id];
			state->run(dtime);
			// Remove removable states
			if (!instance.removable_states.empty()) {
				for (Statevector::iterator removable_states_it = instance.removable_states.begin();
				     removable_states_it != instance.removable_states.end();
				     removable_states_it ++) {
					delete *removable_states_it;
				}
				instance.removable_states.clear();
			}
		}

		// If the seconds multiplier exceeded the maximum value then
		// drop this frame. Otherwise render it. Also ensure the
		// maximum framedrop isn't reached
		if (dtime_remains > Delay::secs(0) &&
		    (instance.max_framedrop != 0.0 && dropped_frames < instance.max_framedrop)) {

			// Increase the amount of dropped frames so the frame
			// drop can be controlled
			dropped_frames ++;

		} else {

			Display::beginRendering();

			// Go States trough and render them
			for (Statevector::iterator states_it = instance.states.begin();
			     states_it != instance.states.end();
			     states_it ++) {
				State* state = *states_it;
				state->render();
			}

			Display::endRendering();

			instance.fps_framecount ++;

			// Mark that no frames are dropped
			dropped_frames = 0;

		}

	} while (!instance.stopped && !instance.states.empty());

	// Remove possible states
	while (!instance.states.empty()) {
		instance.states.back()->exit();
		instance.states.pop_back();
	}

	// Destroy removable states
	for (Statevector::iterator removable_states_it = instance.removable_states.begin();
	     removable_states_it != instance.removable_states.end();
	     removable_states_it ++) {
		delete *removable_states_it;
	}
	instance.removable_states.clear();
}

float Statemanager::getFps(void)
{
	Delay time = now() - instance.fps_last_querytime;
	if (time == Delay::secs(0)) {
		return 0;
	}
	float result = (float)instance.fps_framecount / time.getSecondsAsDouble();
	instance.fps_framecount = 0;
	instance.fps_last_querytime = now();
	return result;
}

Statemanager::Statemanager(void) :
#ifndef NDEBUG
thread_id(getThisThreadID()),
#endif
desired_fps(0),
max_fps(0),
max_framedrop(0)
{
}

Statemanager::~Statemanager(void)
{
	HppAssert(thread_id == getThisThreadID(), "Invalid thread!");
	for (Statevector::iterator removable_states_it = removable_states.begin();
	     removable_states_it != removable_states.end();
	     removable_states_it ++) {
		delete *removable_states_it;
	}
	if (!joys.empty()) {
		throw Exception("Unreleased joysticks found!");
	}
}

SDL_Joystick* Statemanager::registerJoystick(Joystick* joy, unsigned int id)
{
	HppAssert(instance.joys.find(id) == instance.joys.end() || instance.joys[id].find(joy) == instance.joys[id].end(), "");
	// If this is the first joystick then open new SDL Joystick.
	if (instance.joys[id].size() == 0) {
		HppAssert(!SDL_JoystickOpened(id), "");
		HppAssert(instance.sdljoys.find(id) == instance.sdljoys.end(), "");
		SDL_Joystick* new_sdljoy = SDL_JoystickOpen(id);
		if (!new_sdljoy) {
			throw Exception("Unable to open joystick #" + sizeToStr(id) + "!");
		}
		instance.sdljoys[id] = new_sdljoy;
	}
	instance.joys[id].insert(joy);
	return instance.sdljoys[id];
}

void Statemanager::unregisterJoystick(Joystick* joy, unsigned int id)
{
	HppAssert(instance.joys.find(id) != instance.joys.end() && instance.joys[id].find(joy) != instance.joys[id].end(), "");
	instance.joys[id].erase(joy);
	// If this was last joystick of this ID, then close Joystick
	if (instance.joys[id].empty()) {
		HppAssert(SDL_JoystickOpened(id), "");
		SDL_JoystickClose(instance.sdljoys[id]);
		instance.joys.erase(id);
		instance.sdljoys.erase(id);
	}
}

void Statemanager::setJoystickAxis(unsigned char joystick_id,
                                   unsigned char axis_id,
                                   short value)
{
	if (instance.joys.find(joystick_id) != instance.joys.end()) {
		for (Joysticks::iterator joysticks_it = instance.joys[joystick_id].begin();
		     joysticks_it != instance.joys[joystick_id].end();
		     joysticks_it ++) {
			(*joysticks_it)->setAxis(axis_id, value);
		}
	}
}

void Statemanager::setJoystickButton(unsigned char joystick_id,
                                     unsigned char button_id,
                                     bool pressed)
{
	if (instance.joys.find(joystick_id) != instance.joys.end()) {
		for (Joysticks::iterator joysticks_it = instance.joys[joystick_id].begin();
		     joysticks_it != instance.joys[joystick_id].end();
		     joysticks_it ++) {
			(*joysticks_it)->setButton(button_id, pressed);
		}
	}
}

}
