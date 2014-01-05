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

}

#endif
