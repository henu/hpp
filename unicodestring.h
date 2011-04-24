#ifndef HPP_UNICODESTRING_H
#define HPP_UNICODESTRING_H

#include "assert.h"
#include "unicode.h"

#include <string>
#include <ostream>
#include <string.h>

namespace Hpp
{

class UnicodeString
{

public:

	typedef std::string::size_type size_type;
	static const size_type npos = std::string::npos;

	class const_iterator
	{
		friend class UnicodeString;
	public:
		inline ~const_iterator(void);
		// Comparison operators
		inline bool operator==(const_iterator const& it) const;
		inline bool operator!=(const_iterator const& it) const;
		inline bool operator<(const_iterator const& it) const;
		inline bool operator>(const_iterator const& it) const;
		inline bool operator<=(const_iterator const& it) const;
		inline bool operator>=(const_iterator const& it) const;
		// Iterating operators
		inline const_iterator operator++(void);
		inline const_iterator operator--(void);
		inline const_iterator operator++(int);
		inline const_iterator operator--(int);
		inline const_iterator operator+=(ssize_t amount);
		inline const_iterator operator-=(ssize_t amount);
		// Dereferencing operators
		inline UChr operator*(void) const;
	private:
		inline const_iterator(UChr const* loc);
		UChr const* loc;
	};

	inline UnicodeString(void);
	inline UnicodeString(std::string const& str);
	inline UnicodeString(char const* c_str);
	inline UnicodeString(UnicodeString const& ustr);
	inline UnicodeString(size_t len, UChr c);
	inline UnicodeString(UChr const* ustr, size_t len);
	inline ~UnicodeString(void);

	inline UnicodeString operator=(UnicodeString const& ustr);

	inline UnicodeString operator+(UnicodeString const& ustr) const;

	inline UChr& operator[](size_t idx);
	inline UChr operator[](size_t idx) const;

	// Iterator functions
	inline const_iterator begin(void) const;
	inline const_iterator end(void) const;

	inline size_t size(void) const;

	inline bool empty(void) const;

	inline UnicodeString substr(size_type offset, size_type size = npos);

private:

	size_t reserve;
	size_t len;
	UChr* buf;

};

inline std::ostream& operator<<(std::ostream& strm, UnicodeString const& ustr);



// ----------------------------------------
// ----------------------------------------
//
// Implementation of inline members of class UnicodeString
//
// ----------------------------------------
// ----------------------------------------

inline UnicodeString::UnicodeString(void) :
reserve(0),
len(0),
buf(NULL)
{
}

inline UnicodeString::UnicodeString(std::string const& str)
{
	reserve = sizeUTF8(str.begin(), str.end());
	len = reserve;
	if (reserve > 0) {
		buf = new UChr[reserve];
		std::string::const_iterator str_it;
		for (size_t offset = 0; offset < len; offset ++) {
			buf[offset] = extractUTF8(str_it, str.end());
		}
		HppAssert(str_it == str.end(), "All UTF-8 characters not extracted!");
	} else {
		buf = NULL;
	}
}

inline UnicodeString::UnicodeString(char const* c_str)
{
	char const* c_str_end = c_str + strlen(c_str);
	reserve = sizeUTF8(c_str, c_str_end);
	len = reserve;
	if (reserve > 0) {
		buf = new UChr[reserve];
		std::string::const_iterator str_it;
		for (size_t offset = 0; offset < len; offset ++) {
			buf[offset] = extractUTF8(c_str, c_str_end);
		}
		HppAssert(c_str == c_str_end, "All UTF-8 characters not extracted!");
	} else {
		buf = NULL;
	}
}

inline UnicodeString::UnicodeString(UnicodeString const& ustr) :
reserve(ustr.len),
len(ustr.len)
{
	if (reserve > 0) {
		buf = new UChr[reserve];
		memcpy(buf, ustr.buf, sizeof(UChr)*len);
	} else {
		buf = NULL;
	}
}

inline UnicodeString::UnicodeString(size_t len, UChr c)
{
	reserve = len;
	this->len = len;
	if (reserve > 0) {
		buf = new UChr[reserve];
		UChr* buf_end = buf + len;
		for (UChr* buf_it = buf; buf_it != buf_end; buf_it ++) {
			*buf_it = c;
		}
	} else {
		buf = NULL;
	}
}

inline UnicodeString::UnicodeString(UChr const* ustr, size_t len)
{
	reserve = len;
	this->len = len;
	if (reserve > 0) {
		buf = new UChr[reserve];
		memcpy(buf, ustr, sizeof(UChr)*len);
	} else {
		buf = NULL;
	}
}

inline UnicodeString::~UnicodeString(void)
{
	delete buf;
}

inline UnicodeString UnicodeString::operator=(UnicodeString const& ustr)
{
	if (reserve > 0) {
		delete[] buf;
	}
	reserve = ustr.len;
	len = ustr.len;
	if (reserve > 0) {
		buf = new UChr[reserve];
		memcpy(buf, ustr.buf, sizeof(UChr)*len);
	} else {
		buf = NULL;
	}
	return *this;
}

inline UnicodeString UnicodeString::operator+(UnicodeString const& ustr) const
{
	UnicodeString result;
	if (len + ustr.len > 0) {
		result.reserve = len + ustr.len;
		result.len = result.reserve;
		result.buf = new UChr[result.reserve];
		memcpy(result.buf, buf, sizeof(UChr)*len);
		memcpy(result.buf + len, ustr.buf, sizeof(UChr)*ustr.len);
	}
	return result;
}

inline UChr& UnicodeString::operator[](size_t idx)
{
	HppAssert(idx < len, "Overflow!");
	return buf[idx];
}

inline UChr UnicodeString::operator[](size_t idx) const
{
	HppAssert(idx < len, "Overflow!");
	return buf[idx];
}

inline UnicodeString::const_iterator UnicodeString::begin(void) const
{
	return const_iterator(buf);
}

inline UnicodeString::const_iterator UnicodeString::end(void) const
{
	return const_iterator(buf + len);
}

inline size_t UnicodeString::size(void) const
{
	return len;
}

inline bool UnicodeString::empty(void) const
{
	return len == 0;
}


inline UnicodeString UnicodeString::substr(size_type offset, size_type size)
{
	if (offset >= len) {
		return UnicodeString();
	}
	if (size == npos || size >= len - offset) {
		return UnicodeString(buf + offset, len - offset);
	}
	return UnicodeString(buf + offset, size);
}


// ----------------------------------------
// ----------------------------------------
//
// Implementation of inline members of class UnicodeString::const_iterator
//
// ----------------------------------------
// ----------------------------------------

inline UnicodeString::const_iterator::~const_iterator(void)
{
}

inline bool UnicodeString::const_iterator::operator==(const_iterator const& it) const
{
	return loc == it.loc;
}

inline bool UnicodeString::const_iterator::operator!=(const_iterator const& it) const
{
	return loc != it.loc;
}

inline bool UnicodeString::const_iterator::operator<(const_iterator const& it) const
{
	return loc < it.loc;
}

inline bool UnicodeString::const_iterator::operator>(const_iterator const& it) const
{
	return loc > it.loc;
}

inline bool UnicodeString::const_iterator::operator<=(const_iterator const& it) const
{
	return loc <= it.loc;
}

inline bool UnicodeString::const_iterator::operator>=(const_iterator const& it) const
{
	return loc >= it.loc;
}

inline UnicodeString::const_iterator UnicodeString::const_iterator::operator++(void)
{
	loc ++;
	return *this;
}

inline UnicodeString::const_iterator UnicodeString::const_iterator::operator--(void)
{
	loc --;
	return *this;
}

inline UnicodeString::const_iterator UnicodeString::const_iterator::operator++(int)
{
	const_iterator result = *this;
	loc ++;
	return result;
}

inline UnicodeString::const_iterator UnicodeString::const_iterator::operator--(int)
{
	const_iterator result = *this;
	loc --;
	return result;
}

inline UnicodeString::const_iterator UnicodeString::const_iterator::operator+=(ssize_t amount)
{
	loc += amount;
	return *this;
}

inline UnicodeString::const_iterator UnicodeString::const_iterator::operator-=(ssize_t amount)
{
	loc -= amount;
	return *this;
}

inline UChr UnicodeString::const_iterator::operator*(void) const
{
	return *loc;
}

inline UnicodeString::const_iterator::const_iterator(UChr const* loc) :
loc(loc)
{
}

inline std::ostream& operator<<(std::ostream& strm, UnicodeString const& ustr)
{
	std::string str;
	str.reserve(ustr.size() * 2);
	for (UnicodeString::const_iterator ustr_it = ustr.begin();
	     ustr_it != ustr.end();
	     ustr_it ++) {
	     	UChr uchr = *ustr_it;
		str += uChrToUTF8(uchr);
	}
	return strm << str;
}

}

#endif
