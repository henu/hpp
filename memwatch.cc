#include "memwatch.h"

#include "assert.h"
#include "mutex.h"
#include "lock.h"
#include "thread.h"
#include "exception.h"

#include <iostream>
#include <map>


namespace Hpp
{

namespace Debug
{

// This boolean tracks whenever allocations holder has been destroyed or not.
// It is not protected.
bool allocsholder_destroyed = false;

struct AllocInfo
{
    std::string file;
    size_t line;
    std::string func;
    std::string ptr_name;
    Thread::Id caller;
};

typedef std::map< void*, AllocInfo > Allocations;
struct AllocsHolder
{
	Mutex allocs_mutex;
	Allocations allocs;
	inline ~AllocsHolder(void) {
		allocsholder_destroyed = true;
	}
};

// The idea of allocsholder is to be able to detect when it has been destroyed
AllocsHolder aholder;

void doAddAllocation(std::string const& file, size_t line, std::string const& func, void* ptr, std::string const& ptr_name)
{
	HppAssert(ptr, "Cannot watch NULL!");
	if (allocsholder_destroyed) {
		std::cerr << "WARNING: Do NOT perform memory watching after main function has returned! You are doing it right now from " << file << " at line " << line << "!" << std::endl;
		return;
	}
	Lock lock(aholder.allocs_mutex);
	Allocations::const_iterator allocs_find = aholder.allocs.find(ptr);
	if (allocs_find != aholder.allocs.end()) {
		std::cerr << "ERROR: An allocation has failed! Reason: Pointer is already marked as an allocted memory!" << std::endl;
		std::cerr << "Pointer:  " << ptr_name << std::endl;
		std::cerr << "File:     " << file << std::endl;
		std::cerr << "Line:     " << line << std::endl;
		std::cerr << "Function: " << func << std::endl;
		std::cerr << "Thread:   " << getThisThreadID() << std::endl;
		std::cerr << "The pointer was allocated here:" << std::endl;
		std::cerr << "Pointer:  " << allocs_find->second.ptr_name << std::endl;
		std::cerr << "File:     " << allocs_find->second.file << std::endl;
		std::cerr << "Line:     " << allocs_find->second.line << std::endl;
		std::cerr << "Function: " << allocs_find->second.func << std::endl;
		std::cerr << "Thread:   " << allocs_find->second.caller << std::endl;
		throw Exception("Allocation has failed!");
	}
	AllocInfo ainfo;
	ainfo.file = file;
	ainfo.line = line;
	ainfo.func = func;
	ainfo.ptr_name = ptr_name;
	ainfo.caller = getThisThreadID();
	aholder.allocs[ptr] = ainfo;
}

void doReleaseAllocation(std::string const& file, size_t line, std::string const& func, void* ptr, std::string const& ptr_name)
{
	if (allocsholder_destroyed) {
		std::cerr << "WARNING: Do NOT perform memory watching after main function has returned! You are doing it right now from " << file << " at line " << line << "!" << std::endl;
		return;
	}
	Lock lock(aholder.allocs_mutex);
	if (aholder.allocs.find(ptr) == aholder.allocs.end()) {
		std::cerr << "ERROR: Releasing has failed! Reason: Pointer is not marked as an allocted memory!" << std::endl;
		std::cerr << "Pointer:  " << ptr_name << std::endl;
		std::cerr << "File:     " << file << std::endl;
		std::cerr << "Line:     " << line << std::endl;
		std::cerr << "Function: " << func << std::endl;
		throw Exception("Releasing has failed!");
	}
	aholder.allocs.erase(ptr);
}

void doEnsureAllocation(std::string const& file, size_t line, std::string const& func, void* ptr, std::string const& ptr_name)
{
	if (allocsholder_destroyed) {
		std::cerr << "WARNING: Do NOT perform memory watching after main function has returned! You are doing it right now from " << file << " at line " << line << "!" << std::endl;
		return;
	}
	Lock lock(aholder.allocs_mutex);
	if (aholder.allocs.find(ptr) == aholder.allocs.end()) {
		std::cerr << "ERROR: Allocation check has failed! Reason: Pointer is not marked as an allocted memory!" << std::endl;
		std::cerr << "Pointer:  " << ptr_name << std::endl;
		std::cerr << "File:     " << file << std::endl;
		std::cerr << "Line:     " << line << std::endl;
		std::cerr << "Function: " << func << std::endl;
		throw Exception("Allocation check has failed!");
	}
}

}

}
