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

	// Constructors and "constructors"
	inline XMLDocument(Path const& filename);
	inline XMLDocument(std::string const& nodename);
	inline static XMLDocument fromString(std::string const& data);
	inline virtual ~XMLDocument(void);

	inline void save(Path const& path);

private:

	inline XMLDocument(void);

};

inline XMLDocument::XMLDocument(Path const& filename) :
XMLNode(XMLNode::NORMAL, "")
{
	// Read file to memory
	std::string file_contents = filename.readString();

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

inline XMLDocument XMLDocument::fromString(std::string const& data)
{
	XMLDocument result;

	std::string::const_iterator data_begin = data.begin();
	std::string::const_iterator data_end = data.end();
	result.deserialize(data_begin, data_end);
	if (data_begin != data_end) {
		throw Exception("XML file contains extra tags or garbage!");
	}

	return result;
}

inline XMLDocument::~XMLDocument(void)
{
}

inline void XMLDocument::save(Path const& path)
{
	std::string path_str = path.toString();

	std::ofstream file(path_str.c_str());
	if (!file.is_open()) {
		throw Exception("Unable to open file \"" + path_str + "\"!");
	}

	serialize(file, 0);

	file.close();
}

inline XMLDocument::XMLDocument(void) :
XMLNode(XMLNode::NORMAL, "")
{
}

}

#endif
