#ifndef HPP_DEBUG_H
#define HPP_DEBUG_H

#include "magic.h"

#ifndef NDEBUG
#include "watchdog.h"
#include "concurrencywatcher.h"
#endif

#ifndef NDEBUG
#define HPP_DEBUG_BEGIN_TIMELIMIT(name, delay) Hpp::Watchdog name("#name"); name.feed(delay)
#define HPP_DEBUG_END_TIMELIMIT(name) name.feedForever()
#else
#define HPP_DEBUG_BEGIN_TIMELIMIT(name, delay)
#define HPP_DEBUG_END_TIMELIMIT(name)
#endif

#ifndef NDEBUG
#define HPP_ENSURE_NO_CONCURRENCY(variable) Hpp::Concurrencywatcher HPP_UNIQUE_NAME ((void const*)&(variable), #variable, __FILE__, __LINE__)
#else
#define HPP_ENSURE_NO_CONCURRENCY(variable)
#endif

#endif

