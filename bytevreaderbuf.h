#ifndef HPP_BYTEVREADERBUF_H
#define HPP_BYTEVREADERBUF_H

#include "bytev.h"

#include <istream>
#include <cstring>

namespace Hpp
{

class ByteVReaderBufStreambuf : public std::basic_streambuf< char, std::char_traits< char > >
{

public:

	inline ByteVReaderBufStreambuf(void);

	inline void setIterators(Hpp::ByteV::const_iterator* it,
	                         Hpp::ByteV::const_iterator const& end);

protected:

	inline virtual std::streamsize showmanyc(void);
	inline virtual int underflow(void);
	inline virtual std::streamsize xsgetn(char_type* s, std::streamsize count);
	inline virtual int uflow(void);

private:

	Hpp::ByteV::const_iterator* it;
	Hpp::ByteV::const_iterator end;

	char buf;

};

inline ByteVReaderBufStreambuf::ByteVReaderBufStreambuf(void) :
it(NULL)
{
	// Mark input buffer completely read
	setg(&buf, &buf + 1, &buf + 1);
}

inline void ByteVReaderBufStreambuf::setIterators(Hpp::ByteV::const_iterator* it,
                                                  Hpp::ByteV::const_iterator const& end)
{
	this->it = it;
	this->end = end;
}

inline std::streamsize ByteVReaderBufStreambuf::showmanyc(void)
{
	return end - *it;
}

inline int ByteVReaderBufStreambuf::underflow(void)
{
	// If there is nothing left, then return EOF
	if (*it == end) {
		return traits_type::eof();
	}

	// Return result and do not update buffer
	// because there really isn't one.
	int result = **it;
	++ *it;
	return result;
}

inline std::streamsize ByteVReaderBufStreambuf::xsgetn(char_type* s, std::streamsize count)
{
	size_t amount = std::min< size_t >(count, end - *it);
	::memcpy(s, &**it, amount);
	*it += amount;
	return amount;
}

inline int ByteVReaderBufStreambuf::uflow(void)
{
	// If there is nothing left, then return EOF
	if (*it == end) {
		return traits_type::eof();
	}

	// Return result and do not update buffer
	// because there really isn't one.
	int result = **it;
	++ *it;
	return result;
}

class ByteVReaderBuf : public std::basic_istream< char, std::char_traits< char > >
{

public:

	inline ByteVReaderBuf(Hpp::ByteV const& bv);
	inline ByteVReaderBuf(Hpp::ByteV::const_iterator& it, Hpp::ByteV::const_iterator const& end);
	inline ~ByteVReaderBuf(void);

private:

	// If readerbuffer is not initialized with iterator,
	// then new iterator is spawned to this pointer.
	Hpp::ByteV::const_iterator* my_it;

	ByteVReaderBufStreambuf buf;

};

inline ByteVReaderBuf::ByteVReaderBuf(Hpp::ByteV const& bv) :
std::basic_istream< char, std::char_traits< char > >(&buf),
my_it(new Hpp::ByteV::const_iterator(bv.begin()))
{
	buf.setIterators(my_it, bv.end());
}

inline ByteVReaderBuf::ByteVReaderBuf(Hpp::ByteV::const_iterator& it, Hpp::ByteV::const_iterator const& end) :
std::basic_istream< char, std::char_traits< char > >(&buf),
my_it(NULL)
{
	buf.setIterators(&it, end);
}

inline ByteVReaderBuf::~ByteVReaderBuf(void)
{
	delete my_it;
}

}

#endif
