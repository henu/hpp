#ifndef HPP_PATH_H
#define HPP_PATH_H

#include "charset.h"
#include "assert.h"
#include "exception.h"
#include "misc.h"
#include "time.h"
#include "cast.h"
#include "types.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <climits>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef WIN32
#include <grp.h>
#include <pwd.h>
#else
#include <io.h>
#include <shlobj.h>
#endif
#include <dirent.h>

namespace Hpp
{

class Path
{

public:

	enum Type { FILE, DIRECTORY, SYMLINK, UNKNOWN };

	struct DirChild
	{
		std::string name;
		Path::Type type;
	};
	typedef std::vector< DirChild > DirChildren;

	struct Metadata
	{
		Type type;

		std::string owner;
		// Always empty on windows
		std::string group;

		// Seconds is zero, if these are not supported
		Time modified;
		Time created;
	};

	inline static Path getHome(void);
	inline static Path getConfig(void);
	inline static Path getRoot(void);
	inline static Path getUnknown(void);

	inline Path(void);
	inline Path(std::string const& p);

	inline Path(Path const& p);
	inline Path operator=(Path const& p);

	inline std::string toString(bool compact = false) const;

	inline bool isUnknown(void) const { return roottype == UNKN; }
	inline bool isAbsolute(void) const { return roottype != UNKN && roottype != REL; }
	inline bool isRelative(void) const { return roottype == REL; }

	// Get target of symbolic link
	inline Path linkTarget(void) const;

	inline Type getType(void) const;
	inline bool isFile(void) const { return getType() == FILE || (getType() == SYMLINK && linkTarget().isFile()); }
	inline bool isDir(void) const { return getType() == DIRECTORY || (getType() == SYMLINK && linkTarget().isDir()); }
	inline bool isLink(void) const { return getType() == SYMLINK; }

	inline bool exists(void) const;

	inline void ensureDirExists(void) const;

	inline Metadata getMetadata(void) const;

	// Renames file/dir that this Path points to. Note, that
	// pointing of this Path will be modified. Also, if this
	// cannot be done atomically, then Exception is thrown.
	inline void rename(Path const& new_path) const;

	inline size_t getFileSize(void) const;

	// Resizes file
	inline void resizeFile(size_t new_size);

	// Lists directory contents. If you want "." and
	// ".." entries, set show_dotdots to true.
	inline void listDir(DirChildren& result, bool show_dotdots = false) const;

	// Converts relative Path to absolute using current working directory
	inline void convertToAbsolute(void);

	// Forces relative Path to absolute without any conversions.
	// Basically just adds "/" at the beginning. If path is
	// already absolute, then does nothing.
	inline void forceToAbsolute(void);

	// Reads pointed file as bytes/string. If target
	// is not file, then exception is thrown.
	inline void readBytes(ByteV& result) const;
	inline void readString(std::string& result) const;
	inline ByteV readBytes(void) const { ByteV result; readBytes(result); return result; }
	inline std::string readString(void) const { std::string result; readString(result); return result; }

	inline void writeBytes(ByteV const& bytes) const;
	inline void writeString(std::string const& str) const;

	// Returns name of that single file/folder at the end of whole Path
	inline std::string getFilename(void) const;
	inline std::string getFilenameWithoutExtension(size_t dots_to_include = 0) const;

	// Returns file extension (for example .png) if path has it
	inline std::string getExtension(size_t dots_to_include = 0) const;

	// Get/check parent of this file/dir
	inline bool hasParent(void) const;
	inline Path getParent(void) const;

	inline size_t partsSize(void) const;

	inline std::string operator[](int idx) const;

	// Adds new parts to path
	inline Path operator/(std::string const& subitem) const;
	inline Path operator/(Path const& p) const;

	// Adds characters to the last part of path
	inline Path operator+(std::string const& add) const;
	inline Path operator+=(std::string const& add);

	// Comparison operator for sorting. Sorted order is somewhat
	// arbitrary, at least among unknown, relative and absolute
	// paths, but its still good for keys to sets and maps.
	inline bool operator<(Path const& path) const;

	inline bool operator==(Path const& path) const;
	inline bool operator!=(Path const& path) const;

private:

	enum RootType { UNKN,
	                REL,
	                ABS,
	                HOME,
	                CONFIG };

	RootType roottype;
	Strings parts;

	// Ensures that path is either absolute or relative.
	inline void ensureAbsoluteOrRelative(void);

	inline static bool compareDirChildren(DirChild const& child1, DirChild const& child2);

};


// ----------------------------------------
// Implementations of inline functions
// ----------------------------------------

inline Path Path::getHome(void)
{
	Path p;
	p.roottype = HOME;
	p.parts.clear();
	return p;
}

inline Path Path::getConfig(void)
{
	Path p;
	p.roottype = CONFIG;
	p.parts.clear();
	return p;
}

inline Path Path::getRoot(void)
{
	Path p;
	p.roottype = ABS;
	p.parts.clear();
	return p;
}

inline Path Path::getUnknown(void)
{
	Path p;
	p.roottype = UNKN;
	return p;
}

inline Path::Path(void) :
roottype(REL)
{
}

inline Path::Path(std::string const& p)
{
	std::string::const_iterator p_it;
	if (p.empty()) {
		roottype = REL;
		return;
	} else if (p.size() == 1 && p == "~") {
		roottype = HOME;
		return;
	} else if (p.size() >= 2 && p.substr(0, 2) == "~/") {
		roottype = HOME;
		p_it = p.begin() + 2;
	} else if (p[0] == '/') {
		roottype = ABS;
		p_it = p.begin() + 1;
	} else if (p[0] == '~') {
		throw Exception("Unknown path \"" + p + "\"");
	} else {
		roottype = REL;
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
		++ p_it;
	}
	if (!part.empty()) {
		parts.push_back(part);
	}

	// Absolute paths in windows are considered relative
	// paths, so lets convert them to absolute.
	#ifdef WIN32
	if (!parts.empty() && parts[0].size() == 2 && parts[0][1] == ':') {
		roottype = ABS;
	}
	#endif
}

inline Path::Path(Path const& p) :
roottype(p.roottype),
parts(p.parts)
{
}

inline Path Path::operator=(Path const& p)
{
	roottype = p.roottype;
	parts = p.parts;
	return *this;
}

inline std::string Path::toString(bool compact) const
{
	std::string result;
	switch (roottype) {
	case UNKN:
		return "*** unknown ***";
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
	     ++ result_it) {
		if (*result_it == '\\') {
			*result_it = '/';
		}
	}
	#endif

	for (Strings::const_iterator parts_it = parts.begin();
	     parts_it != parts.end();
	     ++ parts_it) {
		if (parts_it == parts.begin()) {
			result += *parts_it;
		} else {
			result += '/' + *parts_it;
		}
	}

	return result;
}

inline Path Path::linkTarget(void) const
{
	#ifndef WIN32
	size_t TARGET_BUF_SIZE = 10 * 1024;
	char* target_buf = new char[TARGET_BUF_SIZE];
	ssize_t target_size = readlink(toString().c_str(), target_buf, TARGET_BUF_SIZE);
	if (target_size < 0) {
		delete[] target_buf;
		throw Exception("Unable to get target of symlink \"" + toString() + "\"!");
	}
	Path result(std::string(target_buf, target_size));
	delete[] target_buf;
	return result;
	#else
	throw Exception("Symlinks are not supported on Windows!");
	#endif
}

inline Path::Type Path::getType(void) const
{
	if (roottype == UNKN) {
		throw Exception("Unable to get type of unknown path!");
	}
	struct stat st;
	#ifndef WIN32
	if (lstat(toString().c_str(), &st)) {
	#else
	if (stat(toString().c_str(), &st)) {
	#endif
		throw Exception("Unable to get type of \"" + toString() + "\"!");
	}
	if (S_ISREG(st.st_mode)) return FILE;
	if (S_ISDIR(st.st_mode)) return DIRECTORY;
	#ifndef WIN32
	if (S_ISLNK(st.st_mode)) return SYMLINK;
	#endif
	return UNKNOWN;
}

inline bool Path::exists(void) const
{
	if (roottype == UNKN) {
		throw Exception("Unable to chcek if unknown path exists!");
	}
	struct stat st;
	#ifndef WIN32
	if (!lstat(toString().c_str(), &st)) {
	#else
	if (!stat(toString().c_str(), &st)) {
	#endif
		return true;
	}
	return errno != ENOENT;
}

inline void Path::ensureDirExists(void) const
{
	if (isUnknown()) {
		throw Exception("Unable to ensure existence of unknown path!");
	}
	Path p2(*this);
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

inline Path::Metadata Path::getMetadata(void) const
{
	if (isUnknown()) {
		throw Exception("Unable to get metadata of unknown path!");
	}

	struct stat st;
	#ifndef WIN32
	if (lstat(toString().c_str(), &st)) {
	#else
	if (stat(toString().c_str(), &st)) {
	#endif
		throw Exception("Unable to get metadata of \"" + toString() + "\"!");
	}

	Metadata result;

	// Type
	if (S_ISREG(st.st_mode)) result.type = FILE;
	else if (S_ISDIR(st.st_mode)) result.type = DIRECTORY;
	#ifndef WIN32
	else if (S_ISLNK(st.st_mode)) result.type = SYMLINK;
	#endif
	else result.type = UNKNOWN;

	// Owner and group
	#ifndef WIN32
	uid_t owner_id = st.st_uid;
	gid_t group_id = st.st_gid;
	struct passwd* owner_pw = getpwuid(owner_id);
	if (owner_pw) {
		result.owner = owner_pw->pw_name;
	}
	struct group* group_gr = getgrgid(group_id);
	if (group_gr) {
		result.group = group_gr->gr_name;
	}
	#endif

	// Last modification time
	result.modified = Time(st.st_mtime, 0);

	// Creation time is not yet supported
	result.created = Time(0, 0);

	return result;
}

inline void Path::rename(Path const& new_path) const
{
	if (::rename(toString().c_str(), new_path.toString().c_str())) {
		if (errno == EXDEV) {
			throw Exception("Unable to rename atomically!");
		}
		throw Exception("Unable to rename!");
	}
}

inline size_t Path::getFileSize(void) const
{
	if (!exists()) {
		throw Exception("Unable to get size of file \"" + toString() + "\" because it does not exist!");
	}
	if (!isFile()) {
		throw Exception("Unable to get size of file \"" + toString() + "\" because it is not a file!");
	}
	std::ifstream file(toString().c_str(), std::ios_base::binary);
	if (!file.is_open()) {
		throw Exception("Unable to get size of file \"" + toString() + "\" because file could not be opened!");
	}
	file.seekg(0, std::ios_base::end);
	size_t result = file.tellg();
	file.close();
	return result;
}

inline void Path::resizeFile(size_t new_size)
{
	if (!exists()) {
		throw Exception("Unable to resize file \"" + toString() + "\" because it does not exist!");
	}
	if (!isFile()) {
		throw Exception("Unable to resize file \"" + toString() + "\" because it is not a file!");
	}
	#ifndef WIN32
	if (truncate(toString().c_str(), new_size) != 0) {
		throw Exception("Resizing of file \"" + toString() + "\" has failed!");
	}
	#else
// TODO: Use SetEndOfFile on windows!
throw Exception("Resizing files is not supported on windows!");
	#endif
	if (getFileSize() != new_size) {
		throw Exception("Resizing of file \"" + toString() + "\" has failed! The size is now " + sizeToStr(getFileSize()) + " instead of " + sizeToStr(new_size) + "!");
	}
}

inline void Path::listDir(DirChildren& result, bool show_dotdots) const
{
	result.clear();

	std::string path_str = convertFromUtf8ToSystemCharset(toString());
	DIR* dir = opendir(path_str.c_str());
	if (dir == NULL) {
		throw Exception("Unable to open folder \"" + toString() + "\"!");
	}

	struct dirent* dir_ent;
	while ((dir_ent = readdir(dir)) != NULL) {
		DirChild new_child;
		// Get name
		#ifndef WIN32
		new_child.name = dir_ent->d_name;
		#else
		new_child.name = std::string(dir_ent->d_name, dir_ent->d_namlen);
		#endif
		new_child.name = convertFromSystemCharsetToUtf8(new_child.name);
		// Discard . and .. entries
		if (!show_dotdots && (new_child.name == "." || new_child.name == "..")) {
			continue;
		}
		// Get type
		#ifndef WIN32
		switch (dir_ent->d_type) {
		case DT_DIR:
			new_child.type = DIRECTORY;
			break;
		case DT_REG:
			new_child.type = FILE;
			break;
		case DT_LNK:
			new_child.type = SYMLINK;
			break;
		default:
			new_child.type = UNKNOWN;
			break;
		}
		#else
		struct stat sttmp;
		if (stat((path_str + "/" + new_child.name).c_str(), &sttmp) == -1) {
			// File/dir has disappeared!
		} else if (S_ISREG(sttmp.st_mode)) {
			new_child.type = FILE;
		} else if (S_ISDIR(sttmp.st_mode)) {
			new_child.type = DIRECTORY;
		} else {
			new_child.type = UNKNOWN;
		}
		#endif
		result.push_back(new_child);
	}

	if (closedir(dir) != 0)
	{
		throw Exception("Unable to close folder \"" + toString() + "\"!");
	}

	std::sort(result.begin(), result.end(), compareDirChildren);

}

inline void Path::convertToAbsolute(void)
{
	if (isUnknown()) {
		throw Exception("Unable to convert unknown path to absolute!");
	}
	std::string parts_begin_str;
	switch (roottype) {
	case REL:
// TODO: Make to work on windows!
#ifdef WIN32
HppAssert(false, "Not implemented yet!");
#endif
		{
			char cwd[PATH_MAX];
			if (getcwd(cwd, PATH_MAX) == NULL) {
				throw Exception("Unable to convert path to absolute, because current directory could not be obtained!");
			}
			parts_begin_str = cwd;
		}
		break;
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
	case UNKN:
		HppAssert(false, "");
	}

	// Fix Bill's evil characters
	#ifdef WIN32
	for (std::string::iterator parts_begin_str_it = parts_begin_str.begin();
	     parts_begin_str_it != parts_begin_str.end();
	     ++ parts_begin_str_it) {
		if (*parts_begin_str_it == '\\') {
			*parts_begin_str_it = '/';
		}
	}
	#endif

	roottype = ABS;
	Strings parts_begin = splitString(parts_begin_str, '/');
	Strings parts_begin_fixed;
	for (Strings::const_iterator parts_begin_it = parts_begin.begin();
	     parts_begin_it != parts_begin.end();
	     ++ parts_begin_it) {
	     	if (!parts_begin_it->empty()) {
			parts_begin_fixed.push_back(*parts_begin_it);
	     	}
	}
	parts.insert(parts.begin(), parts_begin_fixed.begin(), parts_begin_fixed.end());
}

inline void Path::forceToAbsolute(void)
{
	if (isUnknown()) {
		throw Exception("Unable to force unknown path to absolute!");
	}
	if (roottype == REL) {
		roottype = ABS;
	}
}

inline void Path::readBytes(ByteV& result) const
{
	if (isUnknown()) {
		throw Exception("Unable to read bytes from unknown path!");
	}
	if (!exists()) {
		throw Exception("Unable to read bytes from file! Reason: \"" + toString() + "\" does not exist!");
	}
	if (!isFile()) {
		throw Exception("Unable to read bytes from file! Reason: \"" + toString() + "\" is not file!");
	}
	// Open file
	std::ifstream file(toString().c_str(), std::ios::binary);
	if (!file.is_open()) {
		throw Exception("Unable to open file \"" + toString() + "\" for reading!");
	}
	// Get file size
	file.seekg(0, std::ios::end);
	size_t file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	// Read data
	result.clear();
	result.reserve(file_size);
	size_t const READBUF_SIZE = 256 * 1024;
	uint8_t* readbuf = new uint8_t[READBUF_SIZE];
	for (size_t offset = 0; offset < file_size; offset += READBUF_SIZE) {
		size_t read_amount = std::min(file_size - offset, READBUF_SIZE);
		file.read((char*)readbuf, read_amount);
		result.insert(result.end(), readbuf, readbuf + read_amount);
	}
	delete[] readbuf;
	file.close();
	HppAssert(result.size() == file_size, "Not all bytes could be read!");
}

inline void Path::readString(std::string& result) const
{
	if (isUnknown()) {
		throw Exception("Unable to read string from unknown path!");
	}
	if (!exists()) {
		throw Exception("Unable to read string from file! Reason: \"" + toString() + "\" does not exist!");
	}
	if (!isFile()) {
		throw Exception("Unable to read string from file! Reason: \"" + toString() + "\" is not file!");
	}
	// Open file
	std::ifstream file(toString().c_str(), std::ios::binary);
	if (!file.is_open()) {
		throw Exception("Unable to open file \"" + toString() + "\" for reading!");
	}
	// Get file size
	file.seekg(0, std::ios::end);
	size_t file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	// Read data
	result.clear();
	result.reserve(file_size);
	size_t const READBUF_SIZE = 256 * 1024;
	uint8_t* readbuf = new uint8_t[READBUF_SIZE];
	for (size_t offset = 0; offset < file_size; offset += READBUF_SIZE) {
		size_t read_amount = std::min(file_size - offset, READBUF_SIZE);
		file.read((char*)readbuf, read_amount);
		result.insert(result.end(), readbuf, readbuf + read_amount);
	}
	delete[] readbuf;
	file.close();
	HppAssert(result.size() == file_size, "Not all bytes could be read!");
}

inline void Path::writeBytes(ByteV const& bytes) const
{
	if (isUnknown()) {
		throw Exception("Unable to write to unknown path!");
	}
	// Open file
	std::ofstream file(toString().c_str(), std::ios::binary);
	if (!file.is_open()) {
		throw Exception("Unable to open file \"" + toString() + "\" for writing!");
	}
	file.write((char const*)&bytes[0], bytes.size());
	file.close();
}

inline void Path::writeString(std::string const& str) const
{
	if (isUnknown()) {
		throw Exception("Unable to write to unknown path!");
	}
	// Open file
	std::ofstream file(toString().c_str(), std::ios::binary);
	if (!file.is_open()) {
		throw Exception("Unable to open file \"" + toString() + "\" for writing!");
	}
	file.write(&str[0], str.size());
	file.close();
}

inline std::string Path::getFilename(void) const
{
	if (isUnknown()) {
		throw Exception("Unable to get filename of unknown path!");
	}
	if (parts.empty()) {
		if (roottype == REL || roottype == HOME || roottype == CONFIG) {
			Path abs = *this;
			abs.convertToAbsolute();
			return abs.getFilename();
		}
		throw Exception("Unable to get filename of root!");
	}
	return parts.back();
}

inline std::string Path::getFilenameWithoutExtension(size_t dots_to_include) const
{
	std::string filename = getFilename();
	for (size_t including_dots = 0;
	     including_dots <= dots_to_include;
	     ++ including_dots) {
		std::string::size_type dot_pos = filename.find_last_of('.');
		if (dot_pos == std::string::npos) {
			return getFilename();
		}
		filename = filename.substr(0, dot_pos);
	}
	return filename;
}

inline std::string Path::getExtension(size_t dots_to_include) const
{
	std::string filename = getFilename();
	std::string result = "";
	for (size_t including_dots = 0;
	     including_dots <= dots_to_include;
	     ++ including_dots) {
		std::string::size_type dot_pos = filename.find_last_of('.');
		if (dot_pos == std::string::npos) {
			return "";
		}
		if (including_dots == 0) {
			result = filename.substr(dot_pos + 1);
		} else {
			result = filename.substr(dot_pos + 1) + "." + result;
		}
		filename = filename.substr(0, dot_pos);
	}
	return result;
}

inline bool Path::hasParent(void) const
{
	// If there are no parts to reduce, then path needs to be
	// converted to absolute and hope it brings some parts
	if (parts.empty()) {
		Path abs(*this);
		abs.convertToAbsolute();
		return !abs.parts.empty();
	}
	return true;
}

inline Path Path::getParent(void) const
{
	Path result(*this);
	// If there are no parts to reduce, then path needs to be
	// converted to absolute and hope it brings some parts
	if (result.parts.empty()) {
		result.convertToAbsolute();
		if (result.parts.empty()) {
			throw Exception("Path has no parent!");
		}
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
	HppAssert(roottype != UNKN, "Type cannot be unknown when ensuring absolute/relative.");
	std::string parts_begin_str;
	switch (roottype) {
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
	case UNKN:
		HppAssert(false, "");
	}

	// Fix Bill's evil characters
	#ifdef WIN32
	for (std::string::iterator parts_begin_str_it = parts_begin_str.begin();
	     parts_begin_str_it != parts_begin_str.end();
	     ++ parts_begin_str_it) {
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

inline bool Path::operator<(Path const& path) const
{
	// Unknowns paths are always in front in order
	if (isUnknown()) return !path.isUnknown();
	if (path.isUnknown()) return false;

	// Ensure there are only relative or absolute paths left
	HppAssert((isRelative() || isAbsolute()) && !(isRelative() && isAbsolute()), "Path must be either relative or absolute in this point!");
	HppAssert((path.isRelative() || path.isAbsolute()) && !(path.isRelative() && path.isAbsolute()), "Path must be either relative or absolute in this point!");

	// Relative paths are always before absolute ones.
	if (isRelative() && path.isAbsolute()) return true;
	if (isAbsolute() && path.isRelative()) return false;

	HppAssert(isRelative() == path.isRelative(), "Both paths must be same type!");

	// In case absolute paths are not really absolute, they need
	// to be converted into absolutes. However, since only one
	// comparison part is wanted, we have to do some preparations.
	Path const* cmp1;
	Path const* cmp2;
	Path temp1;
	Path temp2;
	if (isRelative()) {
		cmp1 = this;
		cmp2 = &path;
	} else {
		if (roottype == ABS) {
			cmp1 = this;
		} else {
			temp1 = *this;
			temp1.convertToAbsolute();
			cmp1 = &temp1;
		}
		if (path.roottype == ABS) {
			cmp2 = &path;
		} else {
			temp2 = path;
			temp2.convertToAbsolute();
			cmp2 = &temp2;
		}
	}

	size_t cmp1_parts = cmp1->parts.size();
	size_t cmp2_parts = cmp2->parts.size();
	size_t min_parts = std::min(cmp1_parts, cmp2_parts);
	for (size_t part_id = 0; part_id < min_parts; ++ part_id) {
		std::string const& cmp1_part = cmp1->parts[part_id];
		std::string const& cmp2_part = cmp2->parts[part_id];
		if (cmp1_part < cmp2_part) return true;
		if (cmp1_part > cmp2_part) return false;
	}
	return cmp1_parts < cmp2_parts;
}

inline bool Path::operator==(Path const& path) const
{
// TODO/Optimize: Make real implementation!
	return !(*this < path) && !(path < *this);
}

inline bool Path::operator!=(Path const& path) const
{
	return !(*this == path);
}

inline bool Path::compareDirChildren(DirChild const& child1, DirChild const& child2)
{
	if (child1.type == DIRECTORY && child2.type != DIRECTORY) {
		return true;
	} else if (child1.type != DIRECTORY && child2.type == DIRECTORY) {
		return false;
	}
	return child1.name < child2.name;
}

}

#endif
