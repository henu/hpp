#ifndef HPP_SHAREDLOCK_H
#define HPP_SHAREDLOCK_H

#include "sharedmutex.h"
#include "noncopyable.h"
#include "lock.h"

namespace Hpp
{

class SharedLock : public NonCopyable
{

public:

	inline SharedLock(SharedMutex* mutex);
	inline SharedLock(SharedMutex& mutex);

	inline void unlock(void);
	inline void relock(void);

private:

	Lock lock;

};

inline SharedLock::SharedLock(SharedMutex* mutex) :
lock(mutex->ro->mutex)
{
}

inline SharedLock::SharedLock(SharedMutex& mutex) :
lock(mutex.ro->mutex)
{
}

inline void SharedLock::unlock(void)
{
	lock.unlock();
}

inline void SharedLock::relock(void)
{
	lock.relock();
}

}

#endif
