#ifndef HPP_LOCALE_H
#define HPP_LOCALE_H

#include <iconv.h>
#include <string>
#include <errno.h>
#ifdef WIN32
#include <windows.h>
#endif

namespace Hpp
{

inline std::string getSystemCharset(void);
inline std::string convertToUtf8(std::string const& charset);
inline std::string convertFromSystemCharsetToUtf8(std::string const& str);

inline std::string getSystemCharset(void)
{
// TODO: Code this properly!
	#ifndef WIN32
return "UTF-8";
	#else
return "CP1252";
	#endif
}

inline std::string convertTo(std::string const& str, std::string const& from, std::string const& to)
{
	if (from == to) {
		return str;
	}

	std::string result;
	size_t const BUF_SIZE = 1024;
	char buf[BUF_SIZE];

	iconv_t cd = iconv_open(to.c_str(), from.c_str());
	if (cd == (iconv_t)(-1)) {
		throw Exception("Unable to initialize conversion from \"" + from + "\" to \"" + to + "\"!");
	}

	char const* str_c = &str[0];
	size_t str_c_left = str.size();

	// Insert whole string
	size_t subresult_size;
	while (str_c_left > 0) {
		char* subresult = buf;
		size_t subresult_left = BUF_SIZE;
		#ifndef WIN32
		size_t chars_converted = iconv(cd, (char**)&str_c, &str_c_left, &subresult, &subresult_left);
		#else
		size_t chars_converted = iconv(cd, &str_c, &str_c_left, &subresult, &subresult_left);
		#endif
		if (chars_converted == (size_t)(-1)) {
			if (errno == E2BIG) {
				// No more room in subresult. This is normal,
				// there will be more at next round.
			} else {
				iconv_close(cd);
				throw Exception("Unable to do conversion from \"" + from + "\" to \"" + to + "\"!");
			}
		}
		subresult_size = subresult - buf;
		result += std::string(subresult - subresult_size, subresult_size);
	}
	// Convert remaining bytes
	do {
		char* subresult = buf;
		size_t subresult_left = BUF_SIZE;
		size_t chars_converted = iconv(cd, NULL, NULL, &subresult, &subresult_left);
		if (chars_converted == (size_t)(-1)) {
			if (errno == E2BIG) {
				// No more room in subresult. This is normal,
				// there will be more at next round.
			} else {
				iconv_close(cd);
				throw Exception("Unable to do conversion from \"" + from + "\" to \"" + to + "\"!");
			}
		}
		subresult_size = subresult - buf;
		result += std::string(subresult - subresult_size, subresult_size);
	} while (subresult_size > 0);

	if (iconv_close(cd) != 0) {
		throw Exception("Unable to deinitialize conversion from \"" + from + "\" to \"" + to + "\"!");
	}

	return result;
}

inline std::string convertToUtf8(std::string const& str, std::string const& charset)
{
	return convertTo(str, charset, "UTF-8");
}

inline std::string convertFromUtf8(std::string const& str, std::string const& charset)
{
	return convertTo(str, "UTF-8", charset);
}

inline std::string convertFromSystemCharsetToUtf8(std::string const& str)
{
	std::string charset = getSystemCharset();
	return convertToUtf8(str, charset);
}

inline std::string convertFromUtf8ToSystemCharset(std::string const& str)
{
	std::string charset = getSystemCharset();
	return convertFromUtf8(str, charset);
}

}

#endif
