#ifndef HPP_NONCOPYABLE_H
#define HPP_NONCOPYABLE_H

namespace Hpp
{

class NonCopyable
{

protected:

	inline NonCopyable(void) { }
	inline virtual ~NonCopyable(void) { }

private:

	NonCopyable(const NonCopyable&);
	NonCopyable& operator=(const NonCopyable&);

};

}

#endif

