#ifndef HPP_THREAD_H
#define HPP_THREAD_H

#include "exception.h"

#include "assert.h"
#ifdef HPP_USE_SDL_MUTEX
#include <SDL/SDL_thread.h>
#else
#ifndef WIN32
#include <pthread.h>
#endif
#endif
#ifdef WIN32
#include <windows.h>
#endif
#include <stdexcept>
#include <stdint.h>
#include <iostream>


namespace Hpp
{

class Thread
{

public:

	// Type for thread runner function
	typedef void (*Func)(void*);

	// Type for thread ID
	#ifndef WIN32
	typedef pthread_t Id;
	#else
	typedef DWORD Id;
	#endif

	// Constructors. Note, that destroying thread does not stop it!
	inline Thread(void);
	inline Thread(Func func, void* data);

	// Copy constructor and assignment operator. Notice, that thread
	// objects may be copied but only one of them can call wait!
	inline Thread(Thread const& t);
	inline Thread operator=(Thread const& t);

	// Checks if thread is running
	inline bool isRunning(void) const;

	// Waits until thread has finished
	inline void wait(void);

	inline Id getId(void);

private:

	// Type for information block that is given to real threadrunner
	struct Runinfo
	{
		void* data;
		Func func;
		std::string* error;
		#ifndef HPP_USE_SDL_MUTEX
		int* result;
		#endif
	};

	#ifdef HPP_USE_SDL_MUTEX
	SDL_Thread* thrd;
	#else
	#ifndef WIN32
	pthread_t thrd;
	#else
	HANDLE thrd;
	DWORD thrd_id;
	#endif
	int* thrd_result;
	#endif

	// Pointer to error string. This also tells if thread is being ran or
	// not.
	std::string* error;

	// The real function that runs thread
	#ifdef HPP_USE_SDL_MUTEX
	inline static int threadRunner(void* runinfo_raw);
	#else
	#ifndef WIN32
	inline static void* threadRunner(void* runinfo_raw);
	#else
	inline static DWORD WINAPI threadRunner(LPVOID tfp_raw);
	#endif
	#endif

};

// Gets ID of this thread
inline Thread::Id getThisThreadID(void);


// ----------------------------------------
// Implementation of inline functions
// ----------------------------------------

inline Thread::Thread(void) :
#ifndef HPP_USE_SDL_MUTEX
thrd_result(NULL),
#endif
error(NULL)
{
}

inline Thread::Thread(Func func, void* data)
{
	error = new std::string;
	#ifndef HPP_USE_SDL_MUTEX
	thrd_result = new int;
	#endif

	Runinfo* runinfo = new Runinfo;
	runinfo->data = data;
	runinfo->func = func;
	runinfo->error = error;
	#ifndef HPP_USE_SDL_MUTEX
	runinfo->result = thrd_result;
	#endif
	#ifdef HPP_USE_SDL_MUTEX
	thrd = SDL_CreateThread(threadRunner, runinfo);
	if (!thrd) {
		delete error;
		throw Exception("Thread creation has failed!");
	}
	#else
	#ifndef WIN32
	if (pthread_create(&thrd, NULL, threadRunner, runinfo) != 0) {
		delete error;
		delete thrd_result;
		throw Exception("Thread creation has failed!");
	}
	#else
	thrd = CreateThread(NULL, 0, threadRunner, runinfo, 0, &thrd_id);
	#endif
	#endif
}

inline Thread::Thread(Thread const& t) :
thrd(t.thrd),
#ifndef HPP_USE_SDL_MUTEX
thrd_result(t.thrd_result),
#endif
error(t.error)
{
}

inline Thread Thread::operator=(Thread const& t)
{
	thrd = t.thrd;
	#ifndef HPP_USE_SDL_MUTEX
	thrd_result = t.thrd_result;
	#endif
	error = t.error;
	return *this;
}

inline bool Thread::isRunning(void) const
{
	return error;
}

inline void Thread::wait(void)
{
	HppAssert(error, "The thread is not being ran!");
	int status;
	#ifdef HPP_USE_SDL_MUTEX
	SDL_WaitThread(thrd, &status);
	#else
	#ifndef WIN32
	if (pthread_join(thrd, NULL) != 0) {
		throw Exception("Unable to wait for thread to stop!");
	}
	#else
	WaitForSingleObject(thrd, INFINITE);
	#endif
	status = *thrd_result;
	delete thrd_result;
	#endif
	if (status) {
		std::string error2 = *error;
		delete error;
		error = NULL;
		throw Exception(error2);
	}
	delete error;
	error = NULL;
}

inline Thread::Id Thread::getId(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	return SDL_GetThreadID(thrd);
	#else
	#ifndef WIN32
	return thrd;
	#else
	return thrd_id;
	#endif
	#endif
}

#ifdef HPP_USE_SDL_MUTEX
inline int Thread::threadRunner(void* runinfo_raw)
#else
#ifndef WIN32
inline void* Thread::threadRunner(void* runinfo_raw)
#else
inline DWORD WINAPI Thread::threadRunner(LPVOID runinfo_raw)
#endif
#endif
{
	int result = 0;
	// Read parameters
	Runinfo* runinfo = reinterpret_cast< Runinfo* >(runinfo_raw);
	void* data = runinfo->data;
	Func func = runinfo->func;
	std::string* error = runinfo->error;
	#ifndef HPP_USE_SDL_MUTEX
	int* result_p = runinfo->result;
	#endif
	delete runinfo;
	// Run thread
	try {
		func(data);
	}
	catch (Exception const& e) {
		*error = e.what();
		result = 1;
	}
	catch (std::runtime_error const& e) {
		*error = e.what();
		result = 2;
	}
	catch (std::bad_alloc const&) {
		*error = "Not enough memory!";
		result = 3;
	}
	catch ( ... ) {
		*error = "Unknown error!";
		result = 4;
	}

	if (!error->empty()) {
		// If there is no ERROR callback, then print error.
// TODO: Implement error callback!
		if (true) {
			std::cerr << "ERROR: " << *error << std::endl;
		}
	}

	#ifdef HPP_USE_SDL_MUTEX
	return result;
	#else
	*result_p = result;
	#ifndef WIN32
	return NULL;
	#else
	return 0;
	#endif
	#endif
}


inline Thread::Id getThisThreadID(void)
{
	#ifdef HPP_USE_SDL_MUTEX
	return SDL_ThreadID();
	#else
	#ifndef WIN32
	return pthread_self();
	#else
	return GetCurrentThreadId();
	#endif
	#endif
}

}

#endif
