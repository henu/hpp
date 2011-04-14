#ifndef HPP_ISTREAMPIPE_H
#define HPP_ISTREAMPIPE_H

#include "streambuf.h"

#include <ostream>

namespace Hpp
{

class IStreampipeBuf : public std::streambuf
{

public:

	inline IStreampipeBuf(Streambuf* sbuf);

protected:

	// Functions required by class std::streambuf.
	inline virtual int_type underflow(void);

private:

	// Work in buffer mode.
	static size_t const BUF_SIZE = 64;
	char buf[BUF_SIZE];

	Streambuf* sbuf;

};

class IStreampipe : private IStreampipeBuf, public std::istream
{

public:

	inline IStreampipe(Streambuf* sbuf);

protected:

private:

	// For EOF detection
	typedef std::char_traits< char > Traits;

	Streambuf* sbuf;

	// In this function, the characters are parsed.
	inline void putChars(char const* begin, char const* end);

	// Callback function for streambuffer
	inline static void reader(uint8_t const* begin, uint8_t const* end, void* ispipe_raw);

	inline void pushBytes(uint8_t const* begin, uint8_t const* end);

};

inline IStreampipeBuf::IStreampipeBuf(Streambuf* sbuf) :
sbuf(sbuf)
{
	setg(NULL, NULL, NULL);
}

inline std::streambuf::int_type IStreampipeBuf::underflow(void)
{
	// Extract data to buffer
	size_t bytes_read = sbuf->readData((uint8_t*)buf, BUF_SIZE);
	HppAssert(bytes_read <= BUF_SIZE, "Overflow!");

	if (bytes_read == 0) {
		return traits_type::eof();
	}

	setg(buf, buf, buf + bytes_read);

	return buf[0];
}

inline IStreampipe::IStreampipe(Streambuf* sbuf) :
IStreampipeBuf(sbuf),
std::istream(this)
{
}

}

#endif
