#ifndef HPP_EXCEPTION_H
#define HPP_EXCEPTION_H

#include <exception>
#include <string>

namespace Hpp
{

class Exception : public std::exception
{

public:

	inline Exception(void);
	inline Exception(std::string const& error);
	inline virtual ~Exception(void) throw ();

	inline virtual const char* what(void) const throw ();

private:

	std::string error;

};


inline Exception::Exception(void)
{
}

inline Exception::Exception(std::string const& error) :
error(error)
{
}

inline Exception::~Exception(void) throw ()
{
}

inline const char* Exception::what(void) const throw ()
{
	return error.c_str();
}

}

#endif
