#ifndef HPP_DATAMANAGERBASE_H
#define HPP_DATAMANAGERBASE_H

#include "path.h"
#include "types.h"
#include "misc.h"
#include <string>

namespace Hpp
{

class DatamanagerBase
{

public:

	inline DatamanagerBase(void) { }
	inline virtual ~DatamanagerBase(void) { }

	inline void scan(Path const& path);

protected:

	inline void addInterestedFileextension(std::string const& ext) { exts.push_back(toLower(ext)); }

private:

	// List of interested file extensions
	Strings exts;

	inline void scanRecursively(Path const& path, std::string const& name_prefix);

	// Gives interested file to subclass for handling.
	virtual void handleFile(std::string const& name, std::string const& extension, Path const& path) = 0;

};

inline void DatamanagerBase::scan(Path const& path)
{
	scanRecursively(path, "");
}

inline void DatamanagerBase::scanRecursively(Path const& path, std::string const& name_prefix)
{
	Path::DirChildren children;
	path.listDir(children);
	for (Path::DirChildren::const_iterator children_it = children.begin();
	     children_it != children.end();
	     ++ children_it) {
		Path::DirChild const& child = *children_it;
		if (child.type == Path::DIRECTORY) {
			scanRecursively(path / child.name, name_prefix + child.name + "/");
		} else if (child.type != Path::SYMLINK) {
			std::string child_name_lowcase = toLower(child.name);
			// Go all interested file extensions through and check if child matches with any of them
			for (Strings::const_iterator exts_it = exts.begin();
			     exts_it != exts.end();
			     ++ exts_it) {
				std::string const& ext = *exts_it;
				if (endsTo(child_name_lowcase, ext)) {
					std::string item_name = name_prefix + child.name.substr(0, child.name.size() - ext.size() - 1);
					handleFile(item_name, ext, path / child.name);
				}
			}
		}
	}
}

}

#endif
