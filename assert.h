#ifndef HPP_ASSERT_H
#define HPP_ASSERT_H

#include <string>

#ifndef NDEBUG
#define HppAssert(check, error) Hpp::doAssertion(__FILE__, __LINE__, __FUNCTION__, (check), #check, error);
#else
#define HppAssert(check, error)
#endif

// This assertion will be disabled/enabled at compile time
#define HppAssertCC(check, error) Hpp::doAssertionCC(__FILE__, __LINE__, __FUNCTION__, (check), #check, error);

namespace Hpp
{

void doAssertion(std::string const& file, size_t line, std::string const& func, bool check, std::string const& check_str, std::string const& msg);

void doAssertionCC(std::string const& file, size_t line, std::string const& func, bool check, std::string const& check_str, std::string const& msg);

}

#endif

