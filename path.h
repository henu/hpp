#ifndef HPP_PATH_H
#define HPP_PATH_H

#include "assert.h"
#include "exception.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#include <shlobj.h>
#endif

namespace Hpp
{

class Path
{

public:

	inline static Path getHome(void);
	inline static Path getConfig(void);
	inline static Path getRoot(void);
	inline static Path getUnknown(void);

	inline Path(void);
	inline Path(std::string const& p);

	inline Path(Path const& p);
	inline Path operator=(Path const& p);

	inline std::string toString(void) const;

	inline bool isUnknown(void) const { return type == UNKNOWN; }

	inline size_t partsSize(void);

	inline std::string operator[](int idx);

	// Adds new part to path
	inline Path operator/(std::string const& subitem) const;

	// Adds characters to the last part of path
	inline Path operator+(std::string const& add) const;
	inline Path operator+=(std::string const& add);

private:

	enum Type { UNKNOWN,
	            RELATIVE,
	            ABSOLUTE,
	            HOME,
	            CONFIG };

	typedef std::vector< std::string > Parts;

	Type type;
	Parts parts;

	// Ensures that path is either absolute or relative.
	inline void ensureAbsoluteOrRelative(void);

};

inline void ensurePathExists(Path const& p);

inline bool fileExists(Path const& p);


// ----------------------------------------
// Implementations of inline functions
// ----------------------------------------

inline Path Path::getHome(void)
{
	Path p;
	p.type = HOME;
	p.parts.clear();
	return p;
}

inline Path Path::getConfig(void)
{
	Path p;
	p.type = CONFIG;
	p.parts.clear();
	return p;
}

inline Path Path::getRoot(void)
{
	Path p;
	p.type = ABSOLUTE;
	p.parts.clear();
	return p;
}

inline Path Path::getUnknown(void)
{
	Path p;
	p.type = UNKNOWN;
	return p;
}

inline Path::Path(void) :
type(RELATIVE)
{
}

inline Path::Path(std::string const& p)
{
	std::string::const_iterator p_it;
	if (p.empty()) {
		type = RELATIVE;
		return;
	} else if (p.size() == 1 && p == "~") {
		type = HOME;
		return;
	} else if (p.size() >= 2 && p.substr(0, 2) == "~/") {
		type = HOME;
		p_it = p.begin() + 2;
	} else if (p[0] == '/') {
		type = ABSOLUTE;
		p_it = p.begin() + 1;
	} else if (p[0] == '~') {
		throw Exception("Unknown path \"" + p + "\"");
	} else {
		type = RELATIVE;
		p_it = p.begin();
	}
	std::string part;
	while (p_it != p.end()) {
		char c = *p_it;
		if (c != '/') {
			part += c;
		} else if (!part.empty()) {
			parts.push_back(part);
			part.clear();
		}
		p_it ++;
	}
	if (!part.empty()) {
		parts.push_back(part);
	}
}

inline Path::Path(Path const& p) :
type(p.type),
parts(p.parts)
{
}

inline Path Path::operator=(Path const& p)
{
	type = p.type;
	parts = p.parts;
	return *this;
}

inline std::string Path::toString(void) const
{
	std::string result;
	switch (type) {
	case UNKNOWN:
		throw Exception("Unable to convert path to string, because path is unknown");
		break;
	case RELATIVE:
		break;
	case ABSOLUTE:
		result = '/';
		break;
	case HOME:
		#ifndef WIN32
		result = getenv("HOME");
		result += '/';
		#else
		char path_cstr[MAX_PATH];
		SHGetFolderPath(0, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, 0, 0, path_cstr);
		result = path_cstr + '/';
		#endif
		break;
	case CONFIG:
		#ifndef WIN32
		result = getenv("HOME");
		result += "/.config/";
		#else
		char path_cstr[MAX_PATH];
		SHGetFolderPath(0, CSIDL_APPDATA | CSIDL_FLAG_CREATE, 0, 0, path_cstr);
		result = path_cstr + '/';
		#endif
		break;
	}

	// Fix Bill's evil characters
	#ifndef WIN32
	for (std::string::iterator result_it = result.begin();
	     result_it != result.end();
	     result_it ++) {
		if (*result_it == '\\') {
			*result_it = '/';
		}
	}
	#endif

	for (Parts::const_iterator parts_it = parts.begin();
	     parts_it != parts.end();
	     parts_it ++) {
		if (parts_it == parts.begin()) {
			result += *parts_it;
		} else {
			result += '/' + *parts_it;
		}
	}

	return result;
}

inline size_t Path::partsSize(void)
{
	if (isUnknown()) {
		throw Exception("Unable to get size of path parts, because path is unknown!");
	}
	ensureAbsoluteOrRelative();
	return parts.size();
}

inline std::string Path::operator[](int idx)
{
	if (isUnknown()) {
		throw Exception("Unable to get path part, because path is unknown!");
	}
	ensureAbsoluteOrRelative();
	if (idx < 0) {
		throw Exception("Unable to get path part, becaue of underflow!");
	}
	if (idx >= (int)parts.size()) {
		throw Exception("Unable to get path part, becaue of overflow!");
	}
	return parts[idx];
}

inline void Path::ensureAbsoluteOrRelative(void)
{
	HppAssert(type != UNKNOWN, "Type cannot be unknown when ensuring absolute/relative.");
	std::string parts_begin_str;
	switch (type) {
	case RELATIVE:
	case ABSOLUTE:
		return;
	case HOME:
		#ifndef WIN32
		parts_begin_str = getenv("HOME");
		#else
		char path_cstr[MAX_PATH];
		SHGetFolderPath(0, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, 0, 0, path_cstr);
		parts_begin_str = path_cstr;
		#endif
		break;
	case CONFIG:
		#ifndef WIN32
		parts_begin_str = getenv("HOME");
		parts_begin_str += "/.config";
		#else
		char path_cstr[MAX_PATH];
		SHGetFolderPath(0, CSIDL_APPDATA | CSIDL_FLAG_CREATE, 0, 0, path_cstr);
		parts_begin_str = path_cstr;
		#endif
		break;
	case UNKNOWN:
		HppAssert(false, "");
	}

	// Fix Bill's evil characters
	#ifndef WIN32
	for (std::string::iterator parts_begin_str_it = parts_begin_str.begin();
	     parts_begin_str_it != parts_begin_str.end();
	     parts_begin_str_it ++) {
		if (*parts_begin_str_it == '\\') {
			*parts_begin_str_it = '/';
		}
	}
	#endif
}

inline Path Path::operator/(std::string const& subitem) const
{
	if (isUnknown()) {
		throw Exception("Unable to modify path, because it is unknown!");
	}
	if (subitem.empty()) {
		throw Exception("Unable to modify path, because subdir/file is empty!");
	}
	Path result(*this);
	result.parts.push_back(subitem);
	return result;
}

inline Path Path::operator+(std::string const& add) const
{
	if (isUnknown()) {
		throw Exception("Unable to modify path, because it is unknown!");
	}
	if (parts.empty()) {
		throw Exception("Unable to modify last part of path, because it does not exist!");
	}
	Path result(*this);
	result.parts.back() += add;
	return result;
}

inline Path Path::operator+=(std::string const& add)
{
	if (isUnknown()) {
		throw Exception("Unable to modify path, because it is unknown!");
	}
	if (parts.empty()) {
		throw Exception("Unable to modify last part of path, because it does not exist!");
	}
	parts.back() += add;
	return *this;
}

inline void ensurePathExists(Path const& p)
{
	if (p.isUnknown()) {
		throw Exception("Unable to ensure that path exists, because path is unknown!");
	}
	Path p2(p);
	HppAssert(p2.partsSize() > 0, "Fail!");
	std::string p_str = "";

	for (size_t parts_id = 0; parts_id < p2.partsSize(); parts_id ++) {
		p_str += p2[0];
		if (p_str.empty()) {
			continue;
		}

		// Ensure current path part exists.
		struct stat sttmp;
		if (stat(p_str.c_str(), &sttmp) == -1) {
			#ifndef WIN32
			if (mkdir(p_str.c_str(), 0700) == -1) {
				throw Exception("Unable to create directory \"" + p_str + "\"!");
			}
			#else
			mkdir(p_str.c_str());
			#endif
		} else if (!S_ISDIR(sttmp.st_mode)) {
			throw Exception("\"" + p_str + "\" is not a directory!");
		}

		p_str += '/';

	}
}

inline bool fileExists(Path const& p)
{
	if (p.isUnknown()) {
		throw Exception("Unable to check if file exists, because path is unknown!");
	}
	std::string p_str = p.toString();
	std::ifstream file(p_str.c_str());
	if (file.is_open()) {
		file.close();
		return true;
	}
	file.close();
	return false;
}

}

#endif
