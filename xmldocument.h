#ifndef HPP_XMLDOCUMENT_H
#define HPP_XMLDOCUMENT_H

#include "xmlnode.h"
#include "path.h"
#include "exception.h"

namespace Hpp
{

class XMLDocument : public XMLNode
{

public:

	inline XMLDocument(Path const& filename);
	inline XMLDocument(std::string const& nodename);
	inline virtual ~XMLDocument(void);

	inline void save(Path const& path);

private:

};

inline XMLDocument::XMLDocument(Path const& filename) :
XMLNode(XMLNode::NORMAL, "")
{
	std::ifstream file(filename.toString().c_str());
	if (!file.is_open()) {
		throw Exception("Unable to open XML file \"" + filename.toString() + "\"!");
	}

	std::string file_contents;
	// Reserve enough space
	file.seekg(0, std::ios::end);
	size_t file_contents_size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Read file to memory
	size_t const CHUNK_SIZE = 1024*1024;
	char chunk[CHUNK_SIZE];
	while (file_contents.size() < file_contents_size) {
		size_t read_amount = std::min(file_contents_size - file_contents.size(), CHUNK_SIZE);
		file.read(chunk, read_amount);
		file_contents.insert(file_contents.end(), chunk, chunk + read_amount);
	}
	HppAssert(!file.eof(), "File ended prematurely!");
	file.close();

	std::string::const_iterator file_contents_begin = file_contents.begin();
	std::string::const_iterator file_contents_end = file_contents.end();
	deserialize(file_contents_begin, file_contents_end);
	if (file_contents_begin != file_contents_end) {
		throw Exception("XML file contains extra tags or garbage!");
	}
}

inline XMLDocument::XMLDocument(std::string const& nodename) :
XMLNode(XMLNode::NORMAL, nodename)
{
}

inline XMLDocument::~XMLDocument(void)
{
}

inline void XMLDocument::save(Path const& path)
{
	std::string path_str = path.toString();

	std::ofstream file(path_str.c_str());
	if (!file.is_open()) {
		throw Hpp::Exception("Unable to open file \"" + path_str + "\"!");
	}

	serialize(file, 0);

	file.close();
}

}

#endif
