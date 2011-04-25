#include "concurrencywatcher.h"

#include "lock.h"
#include "exception.h"

#include <cstdlib>
#include <iostream>

namespace Hpp
{

Mutex Concurrencywatcher::mutex;
Concurrencywatcher::Usages Concurrencywatcher::usages;

Concurrencywatcher::Concurrencywatcher(void const* ptr, std::string const& var, std::string const& file, size_t line) :
ptr(ptr)
{
	Lock lock(mutex);
	Usages::const_iterator usages_find = usages.find(ptr);
	if (usages_find != usages.end()) {
		Usage const& usage = usages_find->second;
		std::cerr << "ERROR: Variable used simultaneously from two threads!" << std::endl;
		std::cerr << "Variable : " << var << std::endl;
		std::cerr << "Thread #1: " << usage.file << ':' << usage.line << std::endl;
		std::cerr << "Thread #2: " << file << ':' << line << std::endl;
		#ifdef HPP_ASSERT_ABORTS
		abort();
		#else
		throw Exception("Variable used simultaneously from two threads!");
		#endif
	}
	Usage new_usage;
	new_usage.file = file;
	new_usage.line = line;
	usages[ptr] = new_usage;
}

Concurrencywatcher::~Concurrencywatcher(void)
{
	Lock lock(mutex);
	usages.erase(ptr);
}

}
