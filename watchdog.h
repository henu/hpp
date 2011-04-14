#ifndef HPP_WATCHDOG_H
#define HPP_WATCHDOG_H

#include "lock.h"
#include "mutex.h"
#include "thread.h"
#include "condition.h"
#include "time.h"

#include <stdint.h>
#include <cstdlib>
#include <iostream>

namespace Hpp
{

class Watchdog
{

public:

	inline Watchdog(void);
	inline Watchdog(std::string const& name);
	inline ~Watchdog(void);

	// Initializes/feeds watchdog
	inline void feed(Delay const& delay);

	inline void feedForever(void);

private:

	Watchdog(Watchdog const& wdog);
	Watchdog operator=(Watchdog const& wdog);

	Mutex mutex;
	Condition cond;

	std::string name;

	// Thread where watchdog is ran and boolean to tell if watchdog is
	// initialized.
	Thread watchdog_thread;
	bool watchdog_started;

	// Variable to request stop.
	bool stop_requested;

	// Time of death
	Time starving_time;
	bool do_not_starve;

	// Function for thread
	inline static void watchdog(void* dog_raw);

};


inline Watchdog::Watchdog(void) :
watchdog_started(false),
stop_requested(false)
{
}

inline Watchdog::Watchdog(std::string const& name) :
name(name),
watchdog_started(false),
stop_requested(false)
{
}

inline Watchdog::~Watchdog(void)
{
	Lock lock(mutex);
	stop_requested = true;
	if (watchdog_started) {
		lock.unlock();
		cond.signal();
		watchdog_thread.wait();
	}
}

inline void Watchdog::feed(Delay const& delay)
{
	Lock lock(mutex);
	starving_time = now() + delay;
	do_not_starve = false;
	if (watchdog_started) {
		lock.unlock();
		cond.signal();
	} else {
		lock.unlock();
		watchdog_thread = Thread(watchdog, this);
		watchdog_started = true;
	}
}

inline void Watchdog::feedForever(void)
{
	Lock lock(mutex);
	do_not_starve = true;
}

inline void Watchdog::watchdog(void* dog_raw)
{
	Watchdog* dog = reinterpret_cast< Watchdog* >(dog_raw);

	Lock lock(dog->mutex);
	while (!dog->stop_requested) {

		bool dog_starved;
		if (dog->do_not_starve) {
			dog->cond.wait(dog->mutex);
			dog_starved = false;
		} else {
			dog_starved = !dog->cond.wait(dog->mutex, dog->starving_time);
		}

		if (dog_starved) {
			lock.unlock();
			std::cerr << "ERROR: Watchdog \"" << dog->name << "\" has died!" << std::endl;
			exit(EXIT_FAILURE);
		}

	}
}

}

#endif
