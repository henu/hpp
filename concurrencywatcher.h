#ifndef HPP_CONCURRENCYWATCHER_H
#define HPP_CONCURRENCYWATCHER_H

#include "mutex.h"

#include <map>

namespace Hpp
{

class Concurrencywatcher
{

public:

	Concurrencywatcher(void const* ptr, std::string const& var, std::string const& file = "", size_t line = 0);
	~Concurrencywatcher(void);

private:

	struct Usage
	{
		std::string file;
		size_t line;
	};
	typedef std::map< void const*, Usage > Usages;

	static Mutex mutex;
	static Usages usages;

	void const* ptr;

};

}

#endif
