#ifndef HPP_CONDITION_H
#define HPP_CONDITION_H

#include "mutex.h"
#include "exception.h"
#include "time.h"

#ifdef HPP_USE_SDL_MUTEX
#include "SDL_thread.h"
#else
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#endif


namespace Hpp
{

class Condition
{

public:

	// Constuctor and destructor
	inline Condition(void);
	inline ~Condition(void);

	// Wakes up one thread that is waiting for this condition.
	inline void signal(void);

	// Wakes up all threads that are waiting for this condition.
	inline void broadcast(void);

	// Waits for this condition to be signalled.
	inline void wait(Mutex& mutex);

	// Waits specific amount of time for this condition to be signalled. If
	// time limit is exceeded, then false is returned. Otherwise true.
	inline bool wait(Mutex& mutex, Time const& deadline);
	inline bool wait(Mutex& mutex, Delay const& delay);

private:

	// Copy constructor and assignment operator in private to prevent
	// copying.
	Condition(Condition const& cond);
	Condition operator=(Condition const& cond);

	#ifdef HPP_USE_SDL_MUTEX
	SDL_cond* cond;
	#else
	pthread_cond_t cond;
	#endif

};



// ----------------------------------------
// ----------------------------------------
//
// Implementation of inline functions of class Condition
//
// ----------------------------------------
// ----------------------------------------

inline Condition::Condition(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	cond = SDL_CreateCond();
	if (!cond) {
		throw Exception("Unable to create new condition!");
	}
	#else
	if (pthread_cond_init(&cond, NULL) != 0) {
		throw Exception("Unable to create new condition!");
	}
	#endif
}

inline Condition::~Condition(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	SDL_DestroyCond(cond);
	#else
	pthread_cond_destroy(&cond);
	#endif
}

inline void Condition::signal(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	if (SDL_CondSignal(cond) != 0) {
		throw Exception("Unable to signal condition!");
	}
	#else
	if (pthread_cond_signal(&cond) != 0) {
		throw Exception("Unable to signal condition!");
	}
	#endif
}

inline void Condition::broadcast(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	if (SDL_CondBroadcast(cond) != 0) {
		throw Exception("Unable to broadcast condition!");
	}
	#else
	if (pthread_cond_broadcast(&cond) != 0) {
		throw Exception("Unable to broadcast condition!");
	}
	#endif
}

inline void Condition::wait(Mutex& mutex)
{
	HPP_MUTEX_ENSURE_LOCKED(mutex);
	#ifndef NDEBUG
	size_t old_lock_count = mutex.locked;
	mutex.locked = 0;
	#endif
	#ifdef HPP_USE_SDL_MUTEX
	if (SDL_CondWait(cond, mutex.mutex) != 0) {
		throw Exception("Unable to wait for condition!");
	}
	#else
	if (pthread_cond_wait(&cond, &mutex.mutex) != 0) {
		throw Exception("Unable to wait for condition!");
	}
	#endif
	#ifndef NDEBUG
	HppAssert(mutex.locked == 0, "Mutex is not free!");
	mutex.locked = old_lock_count;
	mutex.locked_thread = getThisThreadID();
	#endif
}

inline bool Condition::wait(Mutex& mutex, Time const& deadline)
{
	HPP_MUTEX_ENSURE_LOCKED(mutex);
	#ifndef NDEBUG
	size_t old_lock_count = mutex.locked;
	mutex.locked = 0;
	#endif
	#ifdef HPP_USE_SDL_MUTEX
	Delay delay = deadline - now();
	size_t msecs;
	if (delay.getSeconds() < 0) {
		msecs = 0;
	} else {
		msecs = delay.getSeconds() * 1000 + delay.getNanoseconds() / (1000 * 1000);
	}
	int result = SDL_CondWaitTimeout(cond, mutex.mutex, msecs);
	if (result == SDL_MUTEX_TIMEDOUT) {
	#else
	timespec deadline_ts;
	deadline_ts.tv_sec = deadline.getSeconds();
	deadline_ts.tv_nsec = deadline.getNanoseconds();
	int result = pthread_cond_timedwait(&cond, &mutex.mutex, &deadline_ts);
	if (result == ETIMEDOUT) {
	#endif
		#ifndef NDEBUG
		HppAssert(mutex.locked == 0, "Mutex is not free!");
		mutex.locked = old_lock_count;
		mutex.locked_thread = getThisThreadID();
		#endif
		return false;
	} else if (result != 0) {
		// I *think* that it is good idea to regain mutex debug stuff
		// if return value is INVAL
		#ifndef NDEBUG
		#ifndef HPP_USE_SDL_MUTEX
		if (result == EINVAL) {
			HppAssert(mutex.locked == 0, "Mutex is not free!");
			mutex.locked = old_lock_count;
			mutex.locked_thread = getThisThreadID();
		}
		#endif
		#endif
		throw Exception("Unable to timewait for condition!");
	}
	#ifndef NDEBUG
	HppAssert(mutex.locked == 0, "Mutex is not free!");
	mutex.locked = old_lock_count;
	mutex.locked_thread = getThisThreadID();
	#endif
	return true;
}

inline bool Condition::wait(Mutex& mutex, Delay const& delay)
{
	return wait(mutex, now() + delay);
}

}

#endif
