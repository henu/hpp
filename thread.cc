#include "thread.h"

namespace Hpp
{

void runRunnable(void* runnable_raw);

void Thread::start(void)
{
	if (!runnable) {
		throw Exception("This is not a thread for Runnable-objects!");
	}

	startRunningThread(Hpp::runRunnable, runnable);
}

void runRunnable(void* runnable_raw)
{
	Runnable* runnable = reinterpret_cast< Runnable* >(runnable_raw);
	runnable->run();
}

}

