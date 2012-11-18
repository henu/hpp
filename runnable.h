#ifndef HPP_RUNNABLE_H
#define HPP_RUNNABLE_H

namespace Hpp
{

class Mutex;

class Runnable
{

	friend void runRunnable(void* runnable_raw);

protected:

	Runnable(void);
	~Runnable(void);

	// Helper functions to control thread.
	// It is not mandatory to use these.
	void stopRunnable(void);
	bool isRunnableStopRequested(void);

private:

	bool stop_requested;
	Mutex* stop_requested_mutex;

	// Called by friend
	virtual void run(void) = 0;

};

}

#endif
