#ifndef HPP_MISC_H
#define HPP_MISC_H

#include "bytev.h"
#include "angle.h"
#include "trigon.h"

#include <algorithm>
#include <string>
#include <strings.h>

#define HppFuncStr() (std::string(__FUNCTION__) + ": ")

namespace Hpp
{

template< typename Type >
inline void toZero(Type* buf, size_t size)
{
	#ifndef WIN32
	bzero(buf, size);
	#else
	char* buf_end = (char*)buf + size;
	for (char* buf_it = (char*)buf;
	     buf_it != buf_end;
	     buf_it ++) {
	     	*buf_it = 0;
	}
	#endif
}

template< typename Type >
inline Type clamp(Type min, Type max, Type val);

// Calculates median. Range must be sorted
template< typename Type, typename Iter1, typename Iter2 >
inline void median(Type& result, Iter1 const& begin, Iter2 const& end);

// Compares two ranges. Returns greater than, equal, or less than zero
// if range #1 is greater, equal, or less than range #2, respectively.
template< typename Iter1, typename Iter2, typename Iter3 >
inline int compare(Iter1 range1_begin, Iter2 range1_end, Iter3 range2_begin);

// Divides ByteV/string to parts using delimiter character or characters.
// Last functions do this in parts to save memory. They return true
// whenever subsearch was found.
inline std::vector< ByteV > splitString(ByteV const& v, char delimiter, bool skip_empty_strings = false);
inline std::vector< std::string > splitString(std::string const& str, char delimiter, bool skip_empty_strings = false);
inline std::vector< ByteV > splitString(ByteV const& v, std::vector< char > delimiters, bool skip_empty_strings = false);
inline std::vector< std::string > splitString(std::string const& str, std::vector< char > delimiters, bool skip_empty_strings = false);
template < class Type, class IterIt, class IterEnd, class Char >
inline bool splitStringInParts(Type& result, IterIt& it, IterEnd const& end, Char delimiter);

// Removes whitespace from beginning and end of string
inline std::string trim(std::string const& str, std::string ws = " \t\n");

// Power function for integers
inline ssize_t ipow(ssize_t a, size_t n);

// Division functions for integers that always round to down/up
inline ssize_t iDivFloor(ssize_t dividend, ssize_t divisor);
inline ssize_t iDivCeil(ssize_t dividend, ssize_t divisor);

// Modulo function that does not produce negative results. Always returns value
// that is in range [0 - divisor). Note, that divisor must be positive!
inline size_t iMod(ssize_t dividend, size_t divisor);

// Converts tab, enter, \ and given characters to slashes
inline std::string slashEncode(std::string str, std::string const& chars = "\"\'");

inline std::string wrapWords(std::string const& text, std::string const& prefix = "", size_t terminal_width = 80);


// ----------------------------------------
// Implementation of functions
// ----------------------------------------

template< typename Type >
inline Type clamp(Type min, Type max, Type val)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

template< typename Type, typename Iter1, typename Iter2 >
inline void median(Type& result, Iter1 const& begin, Iter2 const& end)
{
	size_t items = end - begin;
	if (items % 2 == 1) {
		size_t offset = items/2;
		result = *(begin + offset);
	} else {
		size_t offset = items/2;
		result = *(begin + offset - 1);
		result += *(begin + offset);
		result /= 2.0;
	}
}

template< typename Iter1, typename Iter2, typename Iter3 >
inline int compare(Iter1 range1_begin, Iter2 range1_end, Iter3 range2_begin)
{
	while (range1_begin != range1_end) {
		if (*range1_begin < *range2_begin) {
			return -1;
		} else if (*range1_begin > *range2_begin) {
			return 1;
		}
		++ range1_begin;
		++ range2_begin;
	}
	return 0;
}

inline std::vector< ByteV > splitString(ByteV const& v, char delimiter, bool skip_empty_strings)
{
	return splitString(v, std::vector< char >(1, delimiter), skip_empty_strings);
}

inline std::vector< std::string > splitString(std::string const& str, char delimiter, bool skip_empty_strings)
{
	return splitString(str, std::vector< char >(1, delimiter), skip_empty_strings);
}

inline std::vector< ByteV > splitString(ByteV const& v, std::vector< char > delimiters, bool skip_empty_strings)
{
	std::vector< ByteV > result;
	ByteV::const_iterator part_begin = v.begin();
	for (ByteV::const_iterator v_it = v.begin();
	     v_it != v.end();
	     ++ v_it) {
		char c = *v_it;
		if (std::find(delimiters.begin(), delimiters.end(), c) != delimiters.end()) {
			if (part_begin != v_it || !skip_empty_strings) {
				ByteV part(part_begin, v_it);
				result.push_back(part);
			}
			part_begin = v_it;
			++ part_begin;
		}
	}
	if (part_begin != v.end()) {
		result.push_back(ByteV(part_begin, v.end()));
	}
	return result;
}

inline std::vector< std::string > splitString(std::string const& str, std::vector< char > delimiters, bool skip_empty_strings)
{
	std::vector< std::string > result;
	std::string::const_iterator part_begin = str.begin();
	for (std::string::const_iterator str_it = str.begin();
	     str_it != str.end();
	     ++ str_it) {
		char c = *str_it;
		if (std::find(delimiters.begin(), delimiters.end(), c) != delimiters.end()) {
			if (part_begin != str_it || !skip_empty_strings) {
				std::string part(part_begin, str_it);
				result.push_back(part);
			}
			part_begin = str_it;
			++ part_begin;
		}
	}
	if (part_begin != str.end()) {
		result.push_back(std::string(part_begin, str.end()));
	}
	return result;
}

template < class Type, class IterIt, class IterEnd, class Char >
inline bool splitStringInParts(Type& result, IterIt& it, IterEnd const& end, Char delimiter)
{
	result.clear();

	if (it == end) {
		return false;
	}

	do {
		Char c = *it;
		it ++;

		if (c == delimiter) {
			break;
		}

		result += c;

	} while (it != end);

	return true;
}

inline std::string trim(std::string const& str, std::string ws)
{
	std::string::size_type non_ws_begin = str.find_first_not_of(ws);
	std::string::size_type non_ws_end = str.find_last_not_of(ws);
	if (non_ws_begin == std::string::npos) {
		return "";
	}
	HppAssert(non_ws_end != std::string::npos, "Fail!");
	return str.substr(non_ws_begin, non_ws_end + 1 - non_ws_begin);
}

inline ssize_t ipow(ssize_t a, size_t n)
{
	ssize_t result = 1;
	while (n > 0) {
		// If n is odd, then multiply and reduce n
		if (n % 2 == 1) {
			result *= a;
			n --;
		}
		// n is even, so it can be halved
		// if a is multiplied with itself.
		else {
			n /= 2;
			a *= a;
		}
	}
	return result;
}

inline ssize_t iDivFloor(ssize_t dividend, ssize_t divisor)
{
	HppAssert(divisor != 0, "Division by zero!");
	if (dividend >= 0 && divisor > 0) {
		return dividend / divisor;
	}
	if (dividend < 0 && divisor < 0) {
		return dividend / divisor;
	}
	if (dividend < 0) {
		HppAssert(divisor > 0, "Fail!");
		return (dividend - (divisor - 1)) / divisor;
	}
	return (dividend - (divisor + 1)) / divisor;
}

inline ssize_t iDivCeil(ssize_t dividend, ssize_t divisor)
{
	HppAssert(divisor != 0, "Division by zero!");
	if (dividend >= 0 && divisor > 0) {
		return (dividend + (divisor - 1)) / divisor;
	}
	if (dividend < 0 && divisor < 0) {
		return (dividend + (divisor + 1)) / divisor;
	}
	return dividend / divisor;
}

inline size_t iMod(ssize_t dividend, size_t divisor)
{
	HppAssert(divisor != 0, "Division by zero!");
	if (dividend >= 0) {
		return dividend % divisor;
	}
	return (dividend % (ssize_t)divisor) + divisor;
}

inline std::string slashEncode(std::string str, std::string const& chars)
{
	std::string result;
	result.reserve(str.size());
	for (std::string::const_iterator it = str.begin();
	     it != str.end();
	     ++ it) {
		char c = *it;
		if (c == '\t') {
			result += "\\t";
		} else if (c == '\n') {
			result += "\\n";
		} else if (c == '\\') {
			result += "\\\\";
		} else if (chars.find(c) != std::string::npos) {
			result += "\\";
			result += c;
		} else {
			result += c;
		}
	}
	return result;
}

inline std::string wrapWords(std::string const& text, std::string const& prefix, size_t terminal_width)
{
	// Calculate how much there is terminal width left after prefix
	size_t termline_left = terminal_width;
	for (std::string::const_iterator prefix_it = prefix.begin();
	     prefix_it != prefix.end();
	     ++ prefix_it) {
		char c = *prefix_it;
		if (c == '\t') {
			termline_left -= 8;
		} else {
			-- termline_left;
		}
	}

	// Split line to words
	std::vector< std::string > words = splitString(text, ' ');

	// Form nice lines
	std::string result;
	size_t line_len = 0;
	for (std::vector< std::string >::const_iterator words_it = words.begin();
	     words_it != words.end();
	     ++ words_it) {
		std::string const& word = *words_it;
		// When word is the first in line
		if (line_len == 0) {
			// Word fits
			if (word.size() <= termline_left) {
				result += prefix + word;
				line_len = word.size();
			}
			// Word does not fit
			else {
				std::string word_left = word;
				while (true) {
					result += prefix + word_left.substr(0, termline_left);
					line_len = word_left.size();
					if (word_left.size() <= termline_left) {
						break;
					} else {
						word_left = word_left.substr(termline_left);
						result += '\n';
					}
				}
			}
		}
		// When word is not the first in line
		else {
			// Word fits
			if (line_len + 1 + word.size() <= termline_left) {
				result += ' ' + word;
				line_len += 1 + word.size();
			}
			// Word does not fit to this line,
			// but it fits to the next line
			else if (word.size() <= termline_left) {
				result += '\n' + prefix + word;
				line_len = word.size();
			}
			// Word does not fit to this line
			// and not to the next line
			else {
				// Fill rest of current line with the beginning of the word
				std::string word_left;
				size_t line_left = termline_left - line_len;
				if (line_left < 2) {
					word_left = word;
					result += '\n';
				} else {
					result += ' ' + word.substr(0, line_left - 1) + '\n';
					word_left = word.substr(line_left - 1);
				}

				while (true) {
					result += prefix + word_left.substr(0, termline_left);
					line_len = word_left.size();
					if (word_left.size() <= termline_left) {
						break;
					} else {
						word_left = word_left.substr(termline_left);
						result += '\n';
					}
				}

			}
		}
	}

	return result;
}

// Goes through a range of elements and checks which of them should be moved to
// the back of range. Check function must return true to those that are needed
// to be moved. Returns iterator to the first element in the range of moved
// elements. Does not keep range in order!
template< typename FIter, typename CompFunc >
inline FIter moveToBack(FIter begin, FIter end, CompFunc cfunc)
{
	FIter new_end = end;
	while (begin != new_end) {
		if (cfunc(*begin)) {
			std::swap(*begin, *(new_end-1));
			new_end --;
		} else {
			begin ++;
		}
	}
	return new_end;
}

template< typename T >
class VectorMaker
{

public:

	VectorMaker &operator()(T value) { vec.push_back(value); return *this; }

	operator const std::vector< T > &() const { return vec; }

private:

	std::vector< T > vec;

};

template< typename T >
VectorMaker< T > makeVector(T value)
{
	VectorMaker< T > maker;
	return maker(value);
}

}

#endif
