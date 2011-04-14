#ifndef HPP_MUTEX_H
#define HPP_MUTEX_H

#include "exception.h"
#include "assert.h"
#ifndef NDEBUG
#include "thread.h"
#endif

#ifdef HPP_USE_SDL_MUTEX
#include "SDL_thread.h"
#else
#ifndef WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif
#endif

// Ensures mutex is locked for this thread. Throws error and prints
// errormessage if mutex is not locked. Note, that this function is effective
// only in debug mode!
#ifndef NDEBUG
#include <iostream>
#define HPP_MUTEX_ENSURE_LOCKED(mutex) (mutex).ensureLocked(__FILE__, __LINE__, __FUNCTION__);
#else
#define HPP_MUTEX_ENSURE_LOCKED(mutex)
#endif


namespace Hpp
{

class Mutex
{

	// Condition as a friend for condition waiting.
	friend class Condition;
	friend class Lock;

public:

	// Constuctor and destructor
	inline Mutex(void);
	inline ~Mutex(void);

	// Marks mutex as destroyable. This means, that mutex locking is no
	// more allowed. This can be done while mutex is locked in another
	// thread. This really does not destroy mutex and can be called
	// multiple times.
// TODO: In future, this function may be removed!
	inline void destroy(void);

	// This is called by a macro. Do not call it manually!
	inline void ensureLocked(std::string const& file, size_t line, std::string const& func) const;

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	// Locks/unlocks mutex. This is called by friend class Lock.
	inline void lock(void);
	inline void unlock(void);

private:

	// Copy constructor and assignment operator in private to prevent
	// copying.
	Mutex(Mutex const& mutex);
	Mutex operator=(Mutex const& mutex);

	#ifdef HPP_USE_SDL_MUTEX
	SDL_mutex* mutex;
	#else
	#ifndef WIN32
	pthread_mutex_t mutex;
	#else
	HANDLE mutex;
	#endif
	bool destroyed;
	#endif

	// Debug stuff
	#ifndef NDEBUG
	size_t locked;
	Thread::Id locked_thread;
	#endif

};

inline Mutex::Mutex(void)
#ifndef HPP_USE_SDL_MUTEX
 :
destroyed(false)
#ifndef NDEBUG
, locked(0),
locked_thread(0)
#endif
#else
#ifndef NDEBUG
: locked(0),
locked_thread(0)
#endif
#endif
{
	#ifdef HPP_USE_SDL_MUTEX
	mutex = SDL_CreateMutex();
	if (!mutex) {
		throw Exception("Unable to create new mutex!");
	}
	#else
	#ifndef WIN32
	pthread_mutexattr_t mutex_attr;
	if (pthread_mutexattr_init(&mutex_attr) != 0) {
		throw Exception("Unable to create new mutex!");
	}
	if (pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE) != 0 ||
	    pthread_mutex_init(&mutex, &mutex_attr) != 0) {
		pthread_mutexattr_destroy(&mutex_attr);
		throw Exception("Unable to create new mutex!");
	}
	pthread_mutexattr_destroy(&mutex_attr);
	#else
	mutex = CreateMutex(NULL, false, NULL);
	if (!mutex) {
		throw Exception("Unable to create new mutex!");
	}
	#endif
	#endif
}

inline Mutex::~Mutex(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	if (mutex) {
		SDL_DestroyMutex(mutex);
	}
	#else
	if (!destroyed) {
		#ifndef WIN32
		pthread_mutex_destroy(&mutex);
		#else
		CloseHandle(mutex);
		#endif
	}
	#endif
}

inline void Mutex::lock(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	HppAssert(mutex, "Mutex is destroyed and can not be locked any more!");
	if (SDL_mutexP(mutex) != 0) {
		throw Exception("Unable to lock mutex!");
	}
	HppAssert(mutex, "Mutex is destroyed and can not be locked any more!");
	#else
	HppAssert(!destroyed, "Mutex is destroyed and can not be locked any more!");
	#ifndef WIN32
	if (pthread_mutex_lock(&mutex) != 0) {
		throw Exception("Unable to lock mutex!");
	}
	#else
	if (WaitForSingleObject(mutex, INFINITE) != WAIT_OBJECT_0) {
		throw Exception("Unable to lock mutex!");
	}
	#endif
	#endif
	#ifndef NDEBUG
	HppAssert(locked_thread == getThisThreadID() || locked == 0, "Mutex has failed!");
	locked ++;
	locked_thread = getThisThreadID();
	#endif
}

inline void Mutex::unlock(void)
{
	#ifndef NDEBUG
	HppAssert(locked > 0, "Not locked when trying to unlock!");
	locked --;
	#endif
	#ifdef HPP_USE_SDL_MUTEX
	if (SDL_mutexV(mutex) != 0) {
		throw Exception("Unable to unlock mutex!");
	}
	#else
	#ifndef WIN32
	if (pthread_mutex_unlock(&mutex) != 0) {
		throw Exception("Unable to unlock mutex!");
	}
	#else
	if (!ReleaseMutex(mutex)) {
		throw Exception("Unable to unlock mutex!");
	}
	#endif
	#endif
}

inline void Mutex::ensureLocked(std::string const& file, size_t line, std::string const& func) const
{
	#ifndef NDEBUG
	if (locked == 0 || locked_thread != getThisThreadID()) {
		std::cerr << "ERROR: Mutex is not locked for the asking thread!" << std::endl;
		std::cerr << "File:     " << file << std::endl;
		std::cerr << "Line:     " << line << std::endl;
		std::cerr << "Function: " << func << std::endl;
		throw Exception("Mutex is not locked for the asking thread!");
	}
	HppAssert(locked > 0 && locked_thread == getThisThreadID(), "Mutex is not locked for the asking thread!");
	#else
	(void)file;
	(void)line;
	(void)func;
	#endif
}

inline void Mutex::destroy(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	if (SDL_mutexP(mutex) != 0) {
		throw Exception("Unable to lock mutex!");
	}
	SDL_mutex* destroyable_mutex = mutex;
	mutex = NULL;
	if (SDL_mutexV(destroyable_mutex) != 0) {
		throw Exception("Unable to unlock mutex!");
	}
	SDL_DestroyMutex(destroyable_mutex);
	#else
	#ifndef WIN32
	if (pthread_mutex_lock(&mutex) != 0) {
		throw Exception("Unable to lock mutex!");
	}
	#else
	if (WaitForSingleObject(mutex, INFINITE) != WAIT_OBJECT_0) {
		throw Exception("Unable to lock mutex!");
	}
	#endif
	bool destroy_now = !destroyed;
	destroyed = true;
	#ifndef WIN32
	if (pthread_mutex_unlock(&mutex) != 0) {
		throw Exception("Unable to unlock mutex!");
	}
	#else
	if (!ReleaseMutex(mutex)) {
		throw Exception("Unable to unlock mutex!");
	}
	#endif
	if (destroy_now) {
		#ifndef WIN32
		pthread_mutex_destroy(&mutex);
		#else
		CloseHandle(mutex);
		#endif
	}
	#endif
}

}

#endif
