#ifndef HPP_PROFILER_H
#define HPP_PROFILER_H

#include "profilermanager.h"

#include <string>

namespace Hpp
{

class Profiler
{

public:

	inline Profiler(std::string const& task);
	inline ~Profiler(void);

	inline void changeTask(std::string const& task);

private:

};

inline Profiler::Profiler(std::string const& task)
{
	Profilermanager::setTask(this, task);
}

inline Profiler::~Profiler(void)
{
	Profilermanager::clearTask(this);
}

inline void Profiler::changeTask(std::string const& task)
{
	Profilermanager::setTask(this, task);
}

}

#endif
