#include "profilermanager.h"

#include <algorithm>
#include <iostream>

namespace Hpp
{

// Static variables
Profilermanager Profilermanager::instance;

void Profilermanager::printReport(void)
{
	Lock lock(instance.mutex);
	ProfiledataV pdata_v;
	pdata_v.reserve(instance.pdata.size());
	Delay total_time = Delay::secs(0);
	for (Profiledata::const_iterator pdata_it = instance.pdata.begin();
	     pdata_it != instance.pdata.end();
	     pdata_it ++) {
		pdata_v.push_back(*pdata_it);
		total_time += pdata_it->second;
	}
	lock.unlock();
	// Sort so that the most time consuming task is at the beginning of
	// vector.
	std::sort(pdata_v.begin(), pdata_v.end(), profilePairComparer);
	for (ProfiledataV::const_iterator pdata_v_it = pdata_v.begin();
	     pdata_v_it != pdata_v.end();
	     pdata_v_it ++) {
	     	Delay const& d = pdata_v_it->second;
	     	std::cout << pdata_v_it->first << ": " << (d / total_time * 100.0) << "% (" << d << ")" << std::endl;
	}
}

void Profilermanager::reset(void)
{
	Lock lock(instance.mutex);
	instance.pdata.clear();
}

Profilermanager::Profilermanager(void)
{
}

Profilermanager::~Profilermanager(void)
{
	printReport();
}

void Profilermanager::setTask(Profiler const* profiler, std::string const& task)
{
	Time now1 = now();
	Lock lock(instance.mutex);

	// First find correct Taskstack
	Thread::Id this_thread_id = getThisThreadID();
	Taskstacks::iterator tstacks_find = instance.tstacks.find(this_thread_id);
	Taskstack* tstack;
	if (tstacks_find != instance.tstacks.end()) {
		tstack = &tstacks_find->second;
	} else {
		Taskstack new_tstack;
		instance.tstacks[this_thread_id] = new_tstack;
		tstack = &instance.tstacks[this_thread_id];
	}

	// If this stack is found from stack, ensure it is at the topmost one.
	#ifndef NDEBUG
	for (Taskstack::const_iterator tstack_it = tstack->begin();
	     tstack_it != tstack->end();
	     tstack_it ++) {
	     	HppAssert(tstack_it->profiler != profiler || tstack->back().profiler == profiler, "This profiler has already task assigned, but it is not the top most in this thread!");
	}
	#endif

	// There is some previous task being recorded currently
	if (!tstack->empty()) {
		Task& prevtask = tstack->back();
		// Record previous task
		Delay prevtask_time = now1 - prevtask.start;
		Profiledata::iterator pdata_find = instance.pdata.find(prevtask.name);
		if (pdata_find == instance.pdata.end()) {
			instance.pdata[prevtask.name] = prevtask_time;
		} else {
			pdata_find->second += prevtask_time;
		}
		// Change to this task.
		if (prevtask.profiler == profiler) {
			prevtask.name = task;
			prevtask.start = now();
		} else {
			Task new_task;
			new_task.name = task;
			new_task.profiler = profiler;
			new_task.start = now();
			tstack->push_back(new_task);
		}
	}
	// There are no previous tasks
	else {
		Task new_task;
		new_task.name = task;
		new_task.profiler = profiler;
		new_task.start = now();
		tstack->push_back(new_task);
	}
}

void Profilermanager::clearTask(Profiler const* profiler)
{
	Time now1 = now();
	Lock lock(instance.mutex);

	// First find correct Taskstack
	Thread::Id this_thread_id = getThisThreadID();
	Taskstacks::iterator tstacks_find = instance.tstacks.find(this_thread_id);
	Taskstack* tstack;
	if (tstacks_find != instance.tstacks.end()) {
		tstack = &tstacks_find->second;
	} else {
		Taskstack new_tstack;
		instance.tstacks[this_thread_id] = new_tstack;
		tstack = &instance.tstacks[this_thread_id];
	}

	// Ensure this profiler is at the top of stack
	HppAssert(!tstack->empty() && tstack->back().profiler == profiler, "Unable to clear task, because this profiler is not at the top of taskstack!");

	// Record previous task
	Task& currenttask = tstack->back();
	Delay currenttask_time = now1 - currenttask.start;
	Profiledata::iterator pdata_find = instance.pdata.find(currenttask.name);
	if (pdata_find == instance.pdata.end()) {
		instance.pdata[currenttask.name] = currenttask_time;
	} else {
		pdata_find->second += currenttask_time;
	}
	tstack->pop_back();

	// Check if there was another interrupted task from some other profiler
	if (!tstack->empty()) {
		Task& prevtask = tstack->back();
		prevtask.start = now();
	}
	// There was no other tasks, so remove this whole taskstack
	else {
		instance.tstacks.erase(this_thread_id);
	}

}

inline bool Profilermanager::profilePairComparer(Profiledatapair const& pair1, Profiledatapair const& pair2)
{
	if (pair1.second > pair2.second) return true;
	return false;
}

}
