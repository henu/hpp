#ifndef HPP_PATH_H
#define HPP_PATH_H

#include "charset.h"
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
#include <sys/types.h>
#ifdef WIN32
#include <io.h>
#include <shlobj.h>
#endif
#include <dirent.h>

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

	inline std::string toString(bool compact = false) const;

	inline bool isUnknown(void) const { return type == UNKNOWN; }
	inline bool isAbsolute(void) const { return type != UNKNOWN && type != REL; }
	inline bool isRelative(void) const { return type == REL; }

	inline void convertToAbsolute(void);

	// Returns the parent of this file/dir
	inline Path getParent(void) const;

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
	            REL,
	            ABS,
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

struct FolderChild
{
	enum Type { FILE, FOLDER, UNKNOWN };
	std::string name;
	Type type;
};
typedef std::vector< FolderChild > FolderChildren;

inline void listFolderChildren(FolderChildren& result, Path const& path);
inline bool compareFolderChildren(FolderChild const& child1, FolderChild const& child2);


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
	p.type = ABS;
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
type(REL)
{
}

inline Path::Path(std::string const& p)
{
	std::string::const_iterator p_it;
	if (p.empty()) {
		type = REL;
		return;
	} else if (p.size() == 1 && p == "~") {
		type = HOME;
		return;
	} else if (p.size() >= 2 && p.substr(0, 2) == "~/") {
		type = HOME;
		p_it = p.begin() + 2;
	} else if (p[0] == '/') {
		type = ABS;
		p_it = p.begin() + 1;
	} else if (p[0] == '~') {
		throw Exception("Unknown path \"" + p + "\"");
	} else {
		type = REL;
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

	// Absolute paths in windows are considered relative
	// paths, so lets convert them to absolute.
	#ifdef WIN32
	if (!parts.empty() && parts[0].size() == 2 && parts[0][1] == ':') {
		type = ABS;
	}
	#endif
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

inline std::string Path::toString(bool compact) const
{
	std::string result;
	switch (type) {
	case UNKNOWN:
		throw Exception("Unable to convert path to string, because path is unknown");
		break;
	case REL:
		break;
	case ABS:
		#ifndef WIN32
		result = '/';
		#endif
		break;
	case HOME:
		if (compact) {
			result += "~";
		} else {
			#ifndef WIN32
			result = getenv("HOME");
			#else
			char path_cstr[MAX_PATH];
			SHGetFolderPath(0, CSIDL_PERSONAL, 0, 0, path_cstr);
			result = path_cstr;
			#endif
		}
		result += '/';
		break;
	case CONFIG:
		#ifndef WIN32
		if (compact) {
			result += "~";
		} else {
			result = getenv("HOME");
		}
		result += "/.config/";
		#else
		{
			char path_cstr[MAX_PATH];
			SHGetFolderPath(0, CSIDL_LOCAL_APPDATA, 0, 0, path_cstr);
			result = path_cstr;
			result += '/';
		}
		#endif
		break;
	}

	// Fix Bill's evil characters
	#ifdef WIN32
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
	HppAssert(type != UNKNOWN, "Type cannot be unknown when ensuring absolute/relative.");
	std::string parts_begin_str;
	switch (type) {
	case REL:
// TODO: Make to work on windows!
#ifdef WIN32
HppAssert(false, "Not implemented yet!");
#endif
		{
			char cwd[PATH_MAX];
			if (getcwd(cwd, PATH_MAX) == NULL) {
				throw Hpp::Exception("Unable to convert path to absolute, because current directory could not be obtained!");
			}
			parts_begin_str = cwd;
		}
	case ABS:
		return;
	case HOME:
		#ifndef WIN32
		parts_begin_str = getenv("HOME");
		#else
		{
			char path_cstr[MAX_PATH];
			SHGetFolderPath(0, CSIDL_PERSONAL, 0, 0, path_cstr);
			parts_begin_str = path_cstr;
		}
		#endif
		break;
	case CONFIG:
		#ifndef WIN32
		parts_begin_str = getenv("HOME");
		parts_begin_str += "/.config";
		#else
		{
			char path_cstr[MAX_PATH];
			SHGetFolderPath(0, CSIDL_LOCAL_APPDATA, 0, 0, path_cstr);
			parts_begin_str = path_cstr;
		}
		#endif
		break;
	case UNKNOWN:
		HppAssert(false, "");
	}

	// Fix Bill's evil characters
	#ifdef WIN32
	for (std::string::iterator parts_begin_str_it = parts_begin_str.begin();
	     parts_begin_str_it != parts_begin_str.end();
	     parts_begin_str_it ++) {
		if (*parts_begin_str_it == '\\') {
			*parts_begin_str_it = '/';
		}
	}
	#endif

	type = ABS;
	Parts parts_begin = splitString(parts_begin_str, '/');
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

inline Path Path::getParent(void) const
{
	Path result(*this);
	// If there are no parts to reduce, then path needs to be
	// converted to absolute and hope it brings some parts
	if (!result.parts.empty()) {
		result.convertToAbsolute();
	}
	if (result.parts.empty()) {
		throw Hpp::Exception("Path has no parent!");
	}
	result.parts.pop_back();
	return result;
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
	case REL:
	case ABS:
		return;
	case HOME:
		#ifndef WIN32
		parts_begin_str = getenv("HOME");
		#else
		{
			char path_cstr[MAX_PATH];
			SHGetFolderPath(0, CSIDL_PERSONAL, 0, 0, path_cstr);
			parts_begin_str = path_cstr;
		}
		#endif
		break;
	case CONFIG:
		#ifndef WIN32
		parts_begin_str = getenv("HOME");
		parts_begin_str += "/.config";
		#else
		{
			char path_cstr[MAX_PATH];
			SHGetFolderPath(0, CSIDL_LOCAL_APPDATA, 0, 0, path_cstr);
			parts_begin_str = path_cstr;
		}
		#endif
		break;
	case UNKNOWN:
		HppAssert(false, "");
	}

	// Fix Bill's evil characters
	#ifdef WIN32
	for (std::string::iterator parts_begin_str_it = parts_begin_str.begin();
	     parts_begin_str_it != parts_begin_str.end();
	     parts_begin_str_it ++) {
		if (*parts_begin_str_it == '\\') {
			*parts_begin_str_it = '/';
		}
	}
	#endif
// TODO: Finish this method!
HppAssert(false, "Not implemented yet!");
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

	#ifndef WIN32
	std::string p_test("/");
	#else
	std::string p_test;
	#endif
	for (size_t part_id = 0; part_id < p2.partsSize(); part_id ++) {

		std::string part = p2[part_id];
		p_test += part + "/";

		// Ensure current path part exists.
		struct stat sttmp;
		if (stat(convertFromUtf8ToSystemCharset(p_test).c_str(), &sttmp) == -1) {
			#ifndef WIN32
			if (mkdir(convertFromUtf8ToSystemCharset(p_test).c_str(), 0700) == -1) {
				throw Exception("Unable to create directory \"" + p_test + "\"!");
			}
			#else
			mkdir(convertFromUtf8ToSystemCharset(p_test).c_str());
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

inline void listFolderChildren(FolderChildren& result, Path const& path)
{
	result.clear();

	std::string path_str = convertFromUtf8ToSystemCharset(path.toString());
	DIR* dir = opendir(path_str.c_str());
	if (dir == NULL) {
		throw Exception("Unable to open folder \"" + path.toString() + "\"!");
	}

	struct dirent* dir_ent;
	while ((dir_ent = readdir(dir)) != NULL) {
		FolderChild new_child;
		// Get name
		#ifndef WIN32
		new_child.name = dir_ent->d_name;
		#else
		new_child.name = std::string(dir_ent->d_name, dir_ent->d_namlen);
		#endif
		new_child.name = convertFromSystemCharsetToUtf8(new_child.name);
		// Get type
		#ifndef WIN32
		switch (dir_ent->d_type) {
		case DT_DIR:
			new_child.type = FolderChild::FOLDER;
			break;
		case DT_REG:
			new_child.type = FolderChild::FILE;
			break;
		default:
			new_child.type = FolderChild::UNKNOWN;
			break;
		}
		#else
		struct stat sttmp;
		if (stat((path_str + "/" + new_child.name).c_str(), &sttmp) == -1) {
			// File/dir has disappeared!
		} else if (S_ISREG(sttmp.st_mode)) {
			new_child.type = FolderChild::FILE;
		} else if (S_ISDIR(sttmp.st_mode)) {
			new_child.type = FolderChild::FOLDER;
		} else {
			new_child.type = FolderChild::UNKNOWN;
		}
		#endif
		result.push_back(new_child);
	}

	if (closedir(dir) != 0)
	{
		throw Exception("Unable to close folder \"" + path.toString() + "\"!");
	}

	std::sort(result.begin(), result.end(), compareFolderChildren);

}

inline bool compareFolderChildren(FolderChild const& child1, FolderChild const& child2)
{
	if (child1.type == FolderChild::FOLDER && child2.type != FolderChild::FOLDER) {
		return true;
	} else if (child1.type != FolderChild::FOLDER && child2.type == FolderChild::FOLDER) {
		return false;
	}
	return child1.name < child2.name;
}

}

#endif
