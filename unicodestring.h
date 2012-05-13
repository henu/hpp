#ifndef HPP_UNICODESTRING_H
#define HPP_UNICODESTRING_H

#include "assert.h"
#include "unicode.h"

#include <algorithm>
#include <string>
#include <vector>
#include <ostream>
#include <string.h>

namespace Hpp
{

class UnicodeString
{

public:

	typedef std::string::size_type size_type;
	static const size_type npos = std::string::npos;

	class const_iterator;
	class iterator
	{
		friend class UnicodeString;
	public:
		inline ~iterator(void);
		// Comparison operators
		inline bool operator==(iterator const& it) const;
		inline bool operator!=(iterator const& it) const;
		inline bool operator<(iterator const& it) const;
		inline bool operator>(iterator const& it) const;
		inline bool operator<=(iterator const& it) const;
		inline bool operator>=(iterator const& it) const;
		// Iterating operators
		inline iterator operator++(void);
		inline iterator operator--(void);
		inline iterator operator++(int);
		inline iterator operator--(int);
		inline iterator operator+=(ssize_t amount);
		inline iterator operator-=(ssize_t amount);
		// Arithmetic operations
		inline size_t operator-(iterator const& it) const;
		inline size_t operator-(const_iterator const& it) const;
		// Dereferencing operators
		inline UChr& operator*(void) const;
	private:
		inline iterator(UChr* loc);
		UChr* loc;
	};

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
		// Arithmetic operations
		inline size_t operator-(const_iterator const& it) const;
		inline size_t operator-(iterator const& it) const;
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

	// Modification operators
	inline UnicodeString operator=(UnicodeString const& ustr);
	inline UnicodeString operator+(UnicodeString const& ustr) const;
	inline UnicodeString operator+(UChr c) const;
	inline UnicodeString operator+=(UnicodeString const& ustr);
	inline UnicodeString operator+=(UChr c);

	// Comparison operators
	inline bool operator==(UnicodeString const& ustr) const;
	inline bool operator!=(UnicodeString const& ustr) const { return !(*this == ustr); }
	inline bool operator<(UnicodeString const& ustr) const;
	inline bool operator>(UnicodeString const& ustr) const { return ustr < *this; }

	inline UChr& operator[](size_t idx);
	inline UChr operator[](size_t idx) const;

	// Iterator functions
	inline iterator begin(void);
	inline iterator end(void);
	inline const_iterator begin(void) const;
	inline const_iterator end(void) const;

	inline size_t size(void) const;

	inline bool empty(void) const;

	inline void clear(void);

	inline void swap(UnicodeString& ustr);

	inline UnicodeString substr(size_type offset, size_type size = npos) const;

	// Find functions
	inline size_type find(UChr c, size_type index = 0) const;
	inline size_type find_first_of(UnicodeString const& chars, size_type index = 0) const;
	inline size_type find_first_not_of(UnicodeString const& chars, size_type index = 0) const;
	inline size_type find_last_not_of(UnicodeString const& chars, size_type index = npos) const;

	// Special functions
	// Returns string with whitespaces removed from begin and end
	inline UnicodeString strip(void) const;
	// Returns string with letters converted to lower/uppercase
	inline UnicodeString tolower(void) const;
	inline UnicodeString toupper(void) const;
	// Returns STL string
	inline std::string stl_string(void) const;
	// Splits string
	inline std::vector< UnicodeString > split(UChr separator) const;
	inline std::vector< UnicodeString > split(UnicodeString const& separator) const;
	inline std::vector< UnicodeString > split(std::vector< UnicodeString > const& separators) const;
	inline std::vector< UnicodeString > split(std::vector< UChr > const& separators) const;
	inline std::vector< UnicodeString > split(void) const;

	// "Constants"
	inline static UnicodeString getWhitespaceString(void);

private:

	size_t reserve;
	size_t len;
	UChr* buf;

};

inline std::ostream& operator<<(std::ostream& strm, UnicodeString const& ustr);

inline UnicodeString operator+(char const* c_str, UnicodeString const& ustr);



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
		std::string::const_iterator str_it = str.begin();
		try {
			for (size_t offset = 0; offset < len; offset ++) {
				try {
					buf[offset] = extractUTF8(str_it, str.end());
				}
				catch (Hpp::Exception const& e) {
					throw Hpp::Exception("Unable to construct UnicodeString from std::string \"" + str + "\"! Reason: " + e.what());
				}
			}
		}
		catch ( ... ) {
			delete[] buf;
			throw;
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
			try {
				buf[offset] = extractUTF8(c_str, c_str_end);
			}
			catch (Hpp::Exception const& e) {
				delete[] buf;
				throw Hpp::Exception(std::string("Unable to construct UnicodeString from C string \"") + c_str + "\"! Reason: " + e.what());
			}
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
	delete[] buf;
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

inline UnicodeString UnicodeString::operator+(UChr c) const
{
	UnicodeString result;
	result.reserve = len + 1;
	result.len = result.reserve;
	result.buf = new UChr[result.reserve];
	memcpy(result.buf, buf, sizeof(UChr)*len);
	result.buf[len] = c;
	return result;
}

inline UnicodeString UnicodeString::operator+=(UnicodeString const& ustr)
{
	if (len + ustr.len > 0) {
		if (reserve < len + ustr.len) {
			reserve = len + ustr.len;
			UChr* new_buf = new UChr[reserve];
			memcpy(new_buf, buf, sizeof(UChr)*len);
			delete[] buf;
			buf = new_buf;
		}
		memcpy(buf + len, ustr.buf, sizeof(UChr)*ustr.len);
		len += ustr.len;
	}
	return *this;
}

inline UnicodeString UnicodeString::operator+=(UChr c)
{
	if (reserve < len + 1) {
		reserve = 1 + reserve*2;
		UChr* new_buf = new UChr[reserve];
		memcpy(new_buf, buf, sizeof(UChr)*len);
		delete[] buf;
		buf = new_buf;
	}
	buf[len] = c;
	len ++;
	return *this;
}

inline bool UnicodeString::operator==(UnicodeString const& ustr) const
{
	if (size() != ustr.size()) return false;
	const_iterator it = begin();
	const_iterator ustr_it = ustr.begin();
	for (size_t ofs = 0; ofs < size(); ofs ++) {
		if (*it != *ustr_it) return false;
		it ++;
		ustr_it ++;
	}
	return true;
}

inline bool UnicodeString::operator<(UnicodeString const& ustr) const
{
	const_iterator it = begin();
	const_iterator ustr_it = ustr.begin();
	while (true) {
		if (ustr_it == ustr.end()) return false;
		if (it == end()) return true;
		if (*it < *ustr_it) return true;
		if (*it > *ustr_it) return false;
		it ++;
		ustr_it ++;
	}
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

inline UnicodeString::iterator UnicodeString::begin(void)
{
	return iterator(buf);
}

inline UnicodeString::iterator UnicodeString::end(void)
{
	return iterator(buf + len);
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

inline void UnicodeString::clear(void)
{
	len = 0;
	reserve = 0;
	delete[] buf;
	buf = NULL;
}

inline void UnicodeString::swap(UnicodeString& ustr)
{
	size_t swap_reserve = reserve;
	size_t swap_len = len;
	UChr* swap_buf = buf;
	reserve = ustr.reserve;
	len = ustr.len;
	buf = ustr.buf;
	ustr.reserve = swap_reserve;
	ustr.len = swap_len;
	ustr.buf = swap_buf;
}

inline UnicodeString UnicodeString::substr(size_type offset, size_type size) const
{
	if (offset >= len) {
		return UnicodeString();
	}
	if (size == npos || size >= len - offset) {
		return UnicodeString(buf + offset, len - offset);
	}
	return UnicodeString(buf + offset, size);
}

inline UnicodeString::size_type UnicodeString::find(UChr c, size_type index) const
{
	if (index == npos) return npos;
	while (index < size()) {
		UChr c2 = (*this)[index];
		if (c2 == c) {
			return index;
		}
		index ++;
	}
	return npos;
}

inline UnicodeString::size_type UnicodeString::find_first_of(UnicodeString const& chars, size_type index) const
{
	if (index == npos) return npos;
	while (index < size()) {
		UChr c = (*this)[index];
		if (chars.find(c) != npos) {
			return index;
		}
		index ++;
	}
	return npos;
}

inline UnicodeString::size_type UnicodeString::find_first_not_of(UnicodeString const& chars, size_type index) const
{
	if (index == npos) return npos;
	while (index < size()) {
		UChr c = (*this)[index];
		if (chars.find(c) == npos) {
			return index;
		}
		index ++;
	}
	return npos;
}

inline UnicodeString::size_type UnicodeString::find_last_not_of(UnicodeString const& chars, size_type index) const
{
	if (index == npos || index > size()) index = size();
	while (index > 0) {
		index --;
		UChr c = (*this)[index];
		if (chars.find(c) == npos) {
			return index;
		}
	}
	return npos;
}

inline UnicodeString UnicodeString::strip(void) const
{
	UnicodeString const WHITESPACE_CHARACTERS(" \t");
	size_type whitespace_end = find_first_not_of(WHITESPACE_CHARACTERS);
	if (whitespace_end == npos) return UnicodeString();
	size_type whitespace_begin = find_last_not_of(WHITESPACE_CHARACTERS) + 1;
	HppAssert(whitespace_begin != npos, "NPos should not occure!");
	return substr(whitespace_end, whitespace_begin - whitespace_end);
}

inline UnicodeString UnicodeString::tolower(void) const
{
	UnicodeString result(*this);
	for (UnicodeString::iterator result_it = result.begin();
	     result_it != result.end();
	     result_it ++) {
		*result_it = Hpp::tolower(*result_it);
	}
	return result;
}

inline UnicodeString UnicodeString::toupper(void) const
{
	UnicodeString result(*this);
	for (UnicodeString::iterator result_it = result.begin();
	     result_it != result.end();
	     result_it ++) {
		*result_it = Hpp::toupper(*result_it);
	}
	return result;
}

inline std::string UnicodeString::stl_string(void) const
{
	std::string result;
	for (const_iterator it = begin(); it != end(); it ++) {
		result += uChrToUTF8(*it);
	}
	return result;
}

inline std::vector< UnicodeString > UnicodeString::split(UChr separator) const
{
	std::vector< UnicodeString > result;
	UnicodeString subresult;
	for (const_iterator it = begin(); it != end(); it ++) {
		UChr c = *it;
		if (c == separator) {
			result.push_back(subresult);
			subresult.clear();
		} else {
			subresult += c;
		}
	}
	result.push_back(subresult);
	return result;
}

inline std::vector< UnicodeString > UnicodeString::split(UnicodeString const& separator) const
{
	std::vector< UnicodeString > result;
	UnicodeString subresult;
	for (size_type ofs = 0; ofs < size(); ofs ++) {
		if (substr(ofs, separator.size()) == separator) {
			result.push_back(subresult);
			subresult.clear();
			ofs += separator.size() - 1;
		} else {
			subresult += buf[ofs];
		}
	}
	result.push_back(subresult);
	return result;
}

inline std::vector< UnicodeString > UnicodeString::split(std::vector< UChr > const& separators) const
{
	std::vector< UnicodeString > result;
	UnicodeString subresult;
	for (const_iterator it = begin(); it != end(); it ++) {
		UChr c = *it;
		if (std::find(separators.begin(), separators.end(), c) != separators.end()) {
			result.push_back(subresult);
			subresult.clear();
		} else {
			subresult += c;
		}
	}
	result.push_back(subresult);
	return result;
}

inline std::vector< UnicodeString > UnicodeString::split(std::vector< UnicodeString > const& separators) const
{
	std::vector< UnicodeString > result;
	UnicodeString subresult;
	for (size_type ofs = 0; ofs < size(); ofs ++) {
		bool separator_found = false;
		for (std::vector< UnicodeString >::const_iterator separators_it = separators.begin();
		     separators_it != separators.end();
		     separators_it ++) {
			UnicodeString const& separator = *separators_it;
			if (substr(ofs, separator.size()) == separator) {
				result.push_back(subresult);
				subresult.clear();
				ofs += separator.size() - 1;
				separator_found = true;
				break;
			}
		}
		if (!separator_found) {
			subresult += buf[ofs];
		}
	}
	result.push_back(subresult);
	return result;
}

inline std::vector< UnicodeString > UnicodeString::split(void) const
{
	std::vector< UChr > ws_chars;
	ws_chars.push_back(' ');
	ws_chars.push_back('\t');
	return split(ws_chars);
}

inline UnicodeString UnicodeString::getWhitespaceString(void)
{
	return " \t";
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

inline UnicodeString operator+(char const* c_str, UnicodeString const& ustr)
{
// TODO: Make faster!
	return UnicodeString(c_str) + ustr;
}


// ----------------------------------------
// ----------------------------------------
//
// Implementation of inline members of class UnicodeString::iterator
//
// ----------------------------------------
// ----------------------------------------

inline UnicodeString::iterator::~iterator(void)
{
}

inline bool UnicodeString::iterator::operator==(iterator const& it) const
{
	return loc == it.loc;
}

inline bool UnicodeString::iterator::operator!=(iterator const& it) const
{
	return loc != it.loc;
}

inline bool UnicodeString::iterator::operator<(iterator const& it) const
{
	return loc < it.loc;
}

inline bool UnicodeString::iterator::operator>(iterator const& it) const
{
	return loc > it.loc;
}

inline bool UnicodeString::iterator::operator<=(iterator const& it) const
{
	return loc <= it.loc;
}

inline bool UnicodeString::iterator::operator>=(iterator const& it) const
{
	return loc >= it.loc;
}

inline UnicodeString::iterator UnicodeString::iterator::operator++(void)
{
	loc ++;
	return *this;
}

inline UnicodeString::iterator UnicodeString::iterator::operator--(void)
{
	loc --;
	return *this;
}

inline UnicodeString::iterator UnicodeString::iterator::operator++(int)
{
	iterator result = *this;
	loc ++;
	return result;
}

inline UnicodeString::iterator UnicodeString::iterator::operator--(int)
{
	iterator result = *this;
	loc --;
	return result;
}

inline UnicodeString::iterator UnicodeString::iterator::operator+=(ssize_t amount)
{
	loc += amount;
	return *this;
}

inline UnicodeString::iterator UnicodeString::iterator::operator-=(ssize_t amount)
{
	loc -= amount;
	return *this;
}

inline size_t UnicodeString::iterator::operator-(UnicodeString::iterator const& it) const
{
	return loc - it.loc;
}

inline size_t UnicodeString::iterator::operator-(UnicodeString::const_iterator const& it) const
{
	return loc - it.loc;
}

inline UChr& UnicodeString::iterator::operator*(void) const
{
	return *loc;
}

inline UnicodeString::iterator::iterator(UChr* loc) :
loc(loc)
{
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

inline size_t UnicodeString::const_iterator::operator-(UnicodeString::const_iterator const& it) const
{
	return loc - it.loc;
}

inline size_t UnicodeString::const_iterator::operator-(UnicodeString::iterator const& it) const
{
	return loc - it.loc;
}

inline UChr UnicodeString::const_iterator::operator*(void) const
{
	return *loc;
}

inline UnicodeString::const_iterator::const_iterator(UChr const* loc) :
loc(loc)
{
}

}

#endif
