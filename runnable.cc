#include "runnable.h"

#include "lock.h"
#include "mutex.h"

namespace Hpp
{

Runnable::Runnable(void) :
stop_requested(false)
{
	stop_requested_mutex = new Mutex;
}

Runnable::~Runnable(void)
{
	delete stop_requested_mutex;
}

void Runnable::stopRunnable(void)
{
	Lock stop_requested_lock(*stop_requested_mutex);
	stop_requested = true;
}

bool Runnable::isRunnableStopRequested(void)
{
	Lock stop_requested_lock(*stop_requested_mutex);
	return stop_requested;
}

}

