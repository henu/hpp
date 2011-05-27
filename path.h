#ifndef HPP_PATH_H
#define HPP_PATH_H

#include "assert.h"
#include "exception.h"
#include "misc.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <climits>
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
	inline bool isAbsolute(void) const { return type != UNKNOWN && type != RELATIVE; }
	inline bool isRelative(void) const { return type == RELATIVE; }

	inline void convertToAbsolute(void);

	inline size_t partsSize(void) const;

	inline std::string operator[](int idx) const;

	// Adds new parts to path
	inline Path operator/(std::string const& subitem) const;
	inline Path operator/(Path const& p) const;

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

inline void Path::convertToAbsolute(void)
{
// TODO: Make to work on windows!
#ifdef WIN32
HppAssert(false, "Not implemented yet!");
#endif
	HppAssert(type != UNKNOWN, "Type cannot be unknown when ensuring absolute/relative.");
	std::string parts_begin_str;
	switch (type) {
	case RELATIVE:
		{
			char cwd[PATH_MAX];
			if (getcwd(cwd, PATH_MAX) == NULL) {
				throw Hpp::Exception("Unable to convert path to absolute, because current directory could not be obtained!");
			}
			parts_begin_str = cwd;
		}
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

	type = RELATIVE;
	Parts parts_begin = explode(parts_begin_str, '/');
	Parts parts_begin_fixed;
	for (Parts::const_iterator parts_begin_it = parts_begin.begin();
	     parts_begin_it != parts_begin.end();
	     parts_begin_it ++) {
	     	if (!parts_begin_it->empty()) {
			parts_begin_fixed.push_back(*parts_begin_it);
	     	}
	}
	parts.insert(parts.begin(), parts_begin_fixed.begin(), parts_begin_fixed.end());
}

inline size_t Path::partsSize(void) const
{
	if (isUnknown()) {
		throw Exception("Unable to get size of path parts, because path is unknown!");
	}
	Path p2(*this);
	p2.ensureAbsoluteOrRelative();
	return p2.parts.size();
}

inline std::string Path::operator[](int idx) const
{
	if (isUnknown()) {
		throw Exception("Unable to get path part, because path is unknown!");
	}
	if (idx < 0) {
		throw Exception("Unable to get path part, because of underflow!");
	}
	Path p2(*this);
	p2.ensureAbsoluteOrRelative();
	if (idx >= (int)p2.parts.size()) {
		throw Exception("Unable to get path part, because of overflow!");
	}
	return p2.parts[idx];
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
		throw Exception("Cannot concatenate to unknown path!");
	}
	if (subitem.empty()) {
		throw Exception("Unable to modify path, because subdir/file is empty!");
	}
	Path result(*this);
	result.parts.push_back(subitem);
	return result;
}

inline Path Path::operator/(Path const& p) const
{
	if (isUnknown()) {
		throw Exception("Cannot concatenate to unknown path!");
	}
	if (p.isUnknown()) {
		throw Exception("Cannot concatenate an unknown path!");
	}
	if (!p.isRelative()) {
		throw Exception("Concatenation must be done to relative path!");
	}
	Path result(*this);
	result.parts.insert(result.parts.end(), p.parts.begin(), p.parts.end());
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
		throw Exception("Unable to ensure existence of unknown path!");
	}
	Path p2(p);
	p2.convertToAbsolute();

// TODO: Make to work on windows!
#ifdef WIN32
HppAssert(false, "Not implemented yet!");
#endif
	std::string p_test("/");
	for (size_t part_id = 0; part_id < p2.partsSize(); part_id ++) {

		std::string part = p2[part_id];
		p_test += part + "/";

		// Ensure current path part exists.
		struct stat sttmp;
		if (stat(p_test.c_str(), &sttmp) == -1) {
			#ifndef WIN32
			if (mkdir(p_test.c_str(), 0700) == -1) {
				throw Exception("Unable to create directory \"" + p_test + "\"!");
			}
			#else
			mkdir(p_test.c_str());
			#endif
		} else if (!S_ISDIR(sttmp.st_mode)) {
			throw Exception("\"" + p_test + "\" is not a directory!");
		}

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
