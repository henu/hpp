#ifndef HPP_MEMWATCH_H
#define HPP_MEMWATCH_H

#include <string>
#include <stdint.h>


// ----------------------------------------
// ----------------------------------------
//
// Macros
//
// ----------------------------------------
// ----------------------------------------

// Assertion macro
#ifndef NDEBUG
	#define HppStartWatchingMem(ptr) Hpp::Debug::doAddAllocation(__FILE__, __LINE__, __FUNCTION__, (void*)(ptr), #ptr);
	#define HppStopWatchingMem(ptr) Hpp::Debug::doReleaseAllocation(__FILE__, __LINE__, __FUNCTION__, (void*)(ptr), #ptr);
	#define HppEnsureWatchingMem(ptr) Hpp::Debug::doEnsureAllocation(__FILE__, __LINE__, __FUNCTION__, (void*)(ptr), #ptr);
#else
	#ifdef HPP_MEMWATCH
		#define HppStartWatchingMem(ptr) Hpp::Debug::doAddAllocation(__FILE__, __LINE__, __FUNCTION__, (void*)(ptr), #ptr);
		#define HppStopWatchingMem(ptr) Hpp::Debug::doReleaseAllocation(__FILE__, __LINE__, __FUNCTION__, (void*)(ptr), #ptr);
		#define HppEnsureWatchingMem(ptr) Hpp::Debug::doEnsureAllocation(__FILE__, __LINE__, __FUNCTION__, (void*)(ptr), #ptr);
	#else
		#define HppStartWatchingMem(ptr)
		#define HppStopWatchingMem(ptr)
		#define HppEnsureWatchingMem(ptr)
	#endif
#endif

namespace Hpp
{

namespace Debug
{

void doAddAllocation(std::string const& file, size_t line, std::string const& func, void* ptr, std::string const& ptr_name);
void doReleaseAllocation(std::string const& file, size_t line, std::string const& func, void* ptr, std::string const& ptr_name);
void doEnsureAllocation(std::string const& file, size_t line, std::string const& func, void* ptr, std::string const& ptr_name);

}

}

#endif
