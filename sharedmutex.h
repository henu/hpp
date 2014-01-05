#ifndef HPP_SHAREDMUTEX_H
#define HPP_SHAREDMUTEX_H

#include "mutex.h"
#include "lock.h"
#include "noncopyable.h"

namespace Hpp
{

class SharedMutex : public NonCopyable
{

	friend class SharedLock;

public:

	inline SharedMutex(void);
	inline ~SharedMutex(void);

	inline SharedMutex* makeCopy(void);

	inline size_t getInstances(void);

private:

	struct RealObj
	{
		size_t instances;
		Mutex mutex;
		inline RealObj(void) : instances(1) { }
	};

	RealObj* ro;

	inline SharedMutex(RealObj* ro);

};

inline SharedMutex::SharedMutex(void) :
ro(new RealObj)
{
}

inline SharedMutex::~SharedMutex(void)
{
	Lock lock(ro->mutex);
	-- ro->instances;
	bool is_last_instance = (ro->instances == 0);
	lock.unlock();
	if (is_last_instance) {
		delete ro;
	}
}

inline SharedMutex* SharedMutex::makeCopy(void)
{
	Hpp::Lock(ro->mutex);
	++ ro->instances;
	return new SharedMutex(ro);
}

inline size_t SharedMutex::getInstances(void)
{
	Lock lock(ro->mutex);
	volatile size_t result = ro->instances;
	lock.unlock();
	return result;
}

inline SharedMutex::SharedMutex(RealObj* ro) :
ro(ro)
{
}

}

#endif
