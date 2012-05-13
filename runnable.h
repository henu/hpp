#ifndef HPP_RUNNABLE_H
#define HPP_RUNNABLE_H

namespace Hpp
{

class Runnable
{

	friend void Hpp::runRunnable(void* runnable_raw);

private:

	// Called by friend
	virtual void run(void) = 0;

};

}

#endif
