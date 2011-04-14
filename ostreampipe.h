#ifndef HPP_OSTREAMPIPE_H
#define HPP_OSTREAMPIPE_H

#include "streambuf.h"

#include <ostream>

namespace Hpp
{

class OStreampipe : public std::basic_streambuf< char, std::char_traits< char > >
{

public:

	inline OStreampipe(Streambuf* sbuf);

protected:

	// This is called when buffer becomes full. If
	// buffer is not used, then this is called every
	// time when characters are put to stream.
	inline virtual int overflow(int c = Traits::eof());

	// This function is called when stream is flushed,
	// for example when std::endl is put to stream.
	inline virtual int sync(void);

private:

	// For EOF detection
	typedef std::char_traits< char > Traits;

	// Work in buffer mode. It is also possible to work without buffer.
	static size_t const BUF_SIZE = 64;
	char buf[BUF_SIZE];

	Streambuf* sbuf;

	// In this function, the characters are parsed.
	inline void putChars(char const* begin, char const* end);

};

inline OStreampipe::OStreampipe(Streambuf* sbuf) :
sbuf(sbuf)
{
	setp(buf, buf + BUF_SIZE);
}

inline int OStreampipe::overflow(int c)
{
	// Handle output
	putChars(pbase(), pptr());
	if (c != Traits::eof()) {
		char c2 = c;
		// Handle the one character that didn't fit to buffer
		putChars(&c2, &c2 + 1);
	}
	// This tells that buffer is empty again
	setp(buf, buf + BUF_SIZE);
	// I'm not sure about this return value!
	return 0;
}

inline int OStreampipe::sync(void)
{
	// Handle output
	putChars(pbase(), pptr());
	// This tells that buffer is empty again
	setp(buf, buf + BUF_SIZE);
	return 0;
}

inline void OStreampipe::putChars(char const* begin, char const* end)
{
	sbuf->pushData((uint8_t const*)begin, (uint8_t const*)end);
}

}

#endif
