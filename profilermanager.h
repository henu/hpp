#ifndef HPP_PROFILERMANAGER_H
#define HPP_PROFILERMANAGER_H

#include "mutex.h"
#include "lock.h"
#include "thread.h"
#include "time.h"

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <map>

namespace Hpp
{

// Classes that does not need their headers included for now
class Profiler;

class Profilermanager
{

	friend class Profiler;

public:

	// Prints all tasks and time consuming of them
	static void printReport(void);

	// Resets tasks
	static void reset(void);

private:

	// Types for profilingdata
	typedef std::map< std::string, Delay > Profiledata;
	typedef std::pair< std::string, Delay > Profiledatapair;
	typedef std::vector< Profiledatapair > ProfiledataV;

	// Types for current taskstack
	struct Task
	{
		std::string name;
		Profiler const* profiler;
		Time start;
	};
	typedef std::list< Task > Taskstack;
	typedef std::map< Thread::Id, Taskstack > Taskstacks;

	// The only instance of this class
	static Profilermanager instance;

	// Thread protection
	Mutex mutex;

	// Profiling data
	Profiledata pdata;

	// Tasks
	Taskstacks tstacks;

	// Constructor and destructor in private to prevent other instances
	Profilermanager(void);
	~Profilermanager(void);

	// These functions are for friend
	static void setTask(Profiler const* profiler, std::string const& task);
	static void clearTask(Profiler const* profiler);

	static bool profilePairComparer(Profiledatapair const& pair1, Profiledatapair const& pair2);

};

}

#endif
