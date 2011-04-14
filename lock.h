#ifndef HPP_LOCK_H
#define HPP_LOCK_H

#include "mutex.h"

#include "assert.h"



namespace Hpp
{

class Lock
{

public:

	// Constuctor and destructor
	inline Lock(Mutex& mutex);
	inline ~Lock(void);

	// Copy constructor and assignment operators. When lock is copied, the
	// source lock will become useless
	inline Lock(Lock const& lock);
	inline Lock const& operator=(Lock const& lock);

	inline void unlock(void);
	inline void relock(void);

private:

	mutable Mutex* mutex;
	mutable bool locked;

};

inline Lock::Lock(Mutex& mutex) :
mutex(&mutex)
{
	mutex.lock();
	locked = true;
}

inline Lock::~Lock(void)
{
	if (locked && mutex) {
		mutex->unlock();
	}
}

inline Lock::Lock(Lock const& lock) :
mutex(lock.mutex),
locked(lock.locked)
{
	HppAssert(lock.mutex, "Unable to copy lock, because it has become useless!");
	lock.mutex = NULL;
}

inline Lock const& Lock::operator=(Lock const& lock)
{
	HppAssert(lock.mutex, "Unable to copy lock, because it has become useless!");
	mutex = lock.mutex;
	locked = lock.locked;
	lock.mutex = NULL;
	return *this;
}

inline void Lock::unlock(void)
{
	HppAssert(mutex, "Lock has become useless!");
	HppAssert(locked, "Lock is not locked!");
	mutex->unlock();
	locked = false;
}

inline void Lock::relock(void)
{
	HppAssert(mutex, "Lock has become useless!");
	HppAssert(!locked, "Lock is already locked!");
	mutex->lock();
	locked = true;
}

}

#endif
