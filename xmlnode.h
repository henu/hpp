#ifndef HPP_XMLNODE_H
#define HPP_XMLNODE_H

#include "unicodestring.h"
#include "assert.h"
#include "exception.h"

#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <ostream>

namespace Hpp
{

class XMLNode;
typedef std::vector< XMLNode* > XMLNodes;
typedef std::vector< XMLNode const* > XMLNodesConst;

class XMLNode
{

	friend class XMLDocument;

public:

	enum Type { NORMAL,
	            TEXT };

	inline virtual ~XMLNode(void);

	inline Type getType(void) const { return type; }

	// Normal node getters
	inline std::string getName(void) const { HppAssert(type == NORMAL, "Name can be got only from normal nodes!"); return name; }

	// Text node getters
	inline std::string getValue(void) const { HppAssert(type == TEXT, "Value can be got only from text nodes!"); return value; }

	// Attribute handling. This is only allowed for normal nodes
	inline void setAttribute(std::string const& name, std::string const& value);
	inline void setAttribute(std::string const& name, UnicodeString const& value) { setAttribute(name, value.stl_string()); }
	inline void setAttribute(std::string const& name, char const* value) { setAttribute(name, std::string(value)); }
	inline void setAttribute(UnicodeString const& name, std::string const& value) { setAttribute(name.stl_string(), value); }
	inline void setAttribute(UnicodeString const& name, UnicodeString const& value) { setAttribute(name.stl_string(), value.stl_string()); }
	inline void setAttribute(UnicodeString const& name, char const* value) { setAttribute(name.stl_string(), std::string(value)); }
	inline void setAttribute(char const* name, std::string const& value) { setAttribute(std::string(name), value); }
	inline void setAttribute(char const* name, UnicodeString const& value) { setAttribute(std::string(name), value.stl_string()); }
	inline void setAttribute(char const* name, char const* value) { setAttribute(std::string(name), std::string(value)); }
	inline std::string getAttribute(std::string const& name) const;
	inline bool attributeExists(std::string const& name) const;

	// Getters for children. These are only allowed for normal nodes
	inline XMLNodes getChildren(std::string const& name);
	inline XMLNodesConst getChildrenConst(std::string const& name) const;
	inline XMLNodes getChildren(void) { return children; }
	inline XMLNodesConst getChildrenConst(void) const { return XMLNodesConst(children.begin(), children.end()); }
	inline size_t getNumOfChildren(std::string const& name) const;
	inline XMLNode* getChild(std::string const& name, size_t index = 0);
	inline XMLNode const* getChildConst(std::string const& name, size_t index = 0) const;
	inline XMLNode* getChild(size_t index = 0);
	inline XMLNode const* getChildConst(size_t index = 0) const;

	// Adds child nodes. These are only allowed for normal nodes
	inline XMLNode* addNormalChild(std::string const& name);

private:

	// Functions for friends
	inline XMLNode(Type type, std::string const& data);
	inline void serialize(std::ostream& strm, size_t indent) const;
	inline void deserialize(std::string::const_iterator& str_begin, std::string::const_iterator& str_end);

private:

	typedef std::map< std::string, std::string > Attributes;

	Type type;

	// Normal
	std::string name;
	Attributes attrs;
	XMLNodes children;

	// Text
	std::string value;

	inline XMLNode(void) { }

	// Whitespace and comment cleaners
	inline static void cleanWhitespaceFromBegin(std::string::const_iterator& str_begin, std::string::const_iterator const& str_end);
	inline static void cleanWhitespaceFromEnd(std::string& str);
	inline static void cleanWhitespaceAndCommentsFromBegin(std::string::const_iterator& str_begin, std::string::const_iterator const& str_end);
	inline static void cleanWhitespaceAndCommentsFromEnd(std::string::const_iterator const& str_begin, std::string::const_iterator& str_end);

	// Read until str_begin == str_end or until *str_begin is one of given
	// characters
	inline static void readUntil(std::string::const_iterator& str_begin, std::string::const_iterator const& str_end, std::string const& chars);

};

inline XMLNode::~XMLNode(void)
{
	for (XMLNodes::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		delete *children_it;
	}
}

inline void XMLNode::setAttribute(std::string const& name, std::string const& value)
{
	HppAssert(type == NORMAL, "Attribute operations are only allowed for normal nodes!");
	attrs[name] = value;
}

inline std::string XMLNode::getAttribute(std::string const& name) const
{
	HppAssert(type == NORMAL, "Attribute operations are only allowed for normal nodes!");
	Attributes::const_iterator attrs_find = attrs.find(name);
	HppAssert(attrs_find != attrs.end(), "Attribute does not exist!");
	return attrs_find->second;
}

inline bool XMLNode::attributeExists(std::string const& name) const
{
	HppAssert(type == NORMAL, "Attribute operations are only allowed for normal nodes!");
	return attrs.find(name) != attrs.end();
}

inline XMLNodes XMLNode::getChildren(std::string const& name)
{
	HppAssert(type == NORMAL, "Child operations are only allowed for normal nodes!");
	XMLNodes result;
	for (XMLNodes::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	XMLNode* child = *children_it;
	     	if (child->getType() == NORMAL && child->getName() == name) {
	     		result.push_back(child);
	     	}
	}
	return result;
}

inline XMLNodesConst XMLNode::getChildrenConst(std::string const& name) const
{
	HppAssert(type == NORMAL, "Child operations are only allowed for normal nodes!");
	XMLNodesConst result;
	for (XMLNodes::const_iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	XMLNode const* child = *children_it;
	     	if (child->getType() == NORMAL && child->getName() == name) {
	     		result.push_back(child);
	     	}
	}
	return result;
}

inline size_t XMLNode::getNumOfChildren(std::string const& name) const
{
	HppAssert(type == NORMAL, "Child operations are only allowed for normal nodes!");
	size_t result = 0;
	for (XMLNodes::const_iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	XMLNode const* child = *children_it;
	     	if (child->getType() == NORMAL && child->getName() == name) {
	     		result ++;
	     	}
	}
	return result;
}

inline XMLNode* XMLNode::getChild(std::string const& name, size_t index)
{
	HppAssert(type == NORMAL, "Child operations are only allowed for normal nodes!");
	for (XMLNodes::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	XMLNode* child = *children_it;
	     	if (child->getType() == NORMAL && child->getName() == name) {
			if (index == 0) {
				return child;
			}
	     		index --;
	     	}
	}
	return NULL;
}

inline XMLNode const* XMLNode::getChildConst(std::string const& name, size_t index) const
{
	HppAssert(type == NORMAL, "Child operations are only allowed for normal nodes!");
	for (XMLNodes::const_iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	XMLNode const* child = *children_it;
	     	if (child->getType() == NORMAL && child->getName() == name) {
			if (index == 0) {
				return child;
			}
	     		index --;
	     	}
	}
	return NULL;
}

inline XMLNode* XMLNode::getChild(size_t index)
{
	HppAssert(type == NORMAL, "Child operations are only allowed for normal nodes!");
	if (index >= children.size()) return NULL;
	return children[index];
}

inline XMLNode const* XMLNode::getChildConst(size_t index) const
{
	HppAssert(type == NORMAL, "Child operations are only allowed for normal nodes!");
	if (index >= children.size()) return NULL;
	return children[index];
}

inline XMLNode* XMLNode::addNormalChild(std::string const& name)
{
	HppAssert(type == NORMAL, "Child operations are only allowed for normal nodes!");
	XMLNode* new_node = new XMLNode(NORMAL, name);
	children.push_back(new_node);
	return new_node;
}

inline XMLNode::XMLNode(Type type, std::string const& data) :
type(type)
{
	switch (type) {
	case NORMAL:
		name = data;
		break;
	case TEXT:
		value = data;
		break;
	default:
		HppAssert(false, "Unexpected node type!");
	}
}

inline void XMLNode::serialize(std::ostream& strm, size_t indent) const
{
	strm << std::string(indent, '\t');

	if (type == NORMAL) {
		strm << '<' << name;
		for (Attributes::const_iterator attrs_it = attrs.begin();
		     attrs_it != attrs.end();
		     attrs_it ++) {
			strm << ' ' << attrs_it->first << "=\"" << attrs_it->second << '\"';
		}

		if (children.empty()) {
			strm << " />" << std::endl;
		} else {
			strm << '>' << std::endl;
			for (XMLNodes::const_iterator children_it = children.begin();
			     children_it != children.end();
			     children_it ++) {
				XMLNode const* child = *children_it;
				child->serialize(strm, indent + 1);
			}
			strm << std::string(indent, '\t') << "</" << name << '>' << std::endl;
		}
	}

}

inline void XMLNode::deserialize(std::string::const_iterator& str_begin, std::string::const_iterator& str_end)
{
	// Clear whitespace and comments from the beginning and end of string
	// and also ensure there is content in string.
	if (str_begin == str_end) throw Exception("No XML node content!");
	cleanWhitespaceAndCommentsFromBegin(str_begin, str_end);
	if (str_begin == str_end) throw Exception("No XML node content!");
	cleanWhitespaceAndCommentsFromEnd(str_begin, str_end);

	// Normal node
	if (*str_begin == '<') {
		// Tag name
		str_begin ++;
		std::string::const_iterator tag_name_begin = str_begin;
		try {
			readUntil(str_begin, str_end, " \t\x0a\x0d>");
		}
		catch ( ... ) {
			throw Exception("Tag \"" + std::string(tag_name_begin, str_end) + "\" was not closed!");
		}
		std::string tag_name(tag_name_begin, str_begin);

		// Attributes and ending
		bool tag_closed;
		Attributes new_attrs;
		do {
			cleanWhitespaceFromBegin(str_begin, str_end);
			if (str_begin == str_end) {
				throw Exception("Tag \"" + tag_name + "\" is not closed!");
			}
			if (*str_begin == '/') {
				str_begin ++;
				if (str_begin == str_end || *str_begin != '>') {
					throw Exception("Tag \"" + tag_name + "\" is not closed properly!");
				}
				// Skip '>'
				str_begin ++;
				tag_closed = true;
				break;
			} else if (*str_begin == '>') {
				str_begin ++;
				tag_closed = false;
				break;
			}

			// Read name of attribute
			std::string::const_iterator attr_name_begin = str_begin;
			try {
				readUntil(str_begin, str_end, "=");
			}
			catch ( ... ) {
				throw Exception("Tag \"" + tag_name + "\" has attribute without value!");
			}
			std::string attr_name(attr_name_begin, str_begin);
			cleanWhitespaceFromEnd(attr_name);
			str_begin ++;
// TODO: Ensure there is no whitespace in attribute name

			// Ensure attribute is not already defined
			if (new_attrs.find(attr_name) != new_attrs.end()) {
				throw Exception("Tag \"" + tag_name + "\" has attribute \"" + attr_name + "\" defned more than once!");
			}

			// Read value of attribute
			std::string attr_value;
			char attr_value_open = 0;
			do {
				if (str_begin == str_end) {
					throw Exception("Tag \"" + tag_name + "\" has attribute(" + attr_name + ") without properly formated value!");
				}
				char c = *str_begin;
				str_begin ++;

				if (!attr_value_open && (c == '\'' || c == '\"')) {
					attr_value_open = c;
				} else if (attr_value_open && c == attr_value_open) {
					break;
				} else if (c == '\\') {
					if (str_begin == str_end) {
						throw Exception("Tag \"" + tag_name + "\" has attribute(" + attr_name + ") with invalid escapement in its value!");
					}
					attr_value += *str_begin;
					str_begin ++;
				} else {
					attr_value += c;
				}

			} while (true);

			new_attrs[attr_name] = attr_value;

		} while (true);

		type = NORMAL;
		name = tag_name;
		attrs = new_attrs;
		children.clear();

		// If tag was not closed, then it might have children
		if (!tag_closed) {
			cleanWhitespaceFromBegin(str_begin, str_end);
			while (str_begin != str_end) {
				// Check if this is closing of this tag
				if (*str_begin == '<' && str_end - str_begin >= 1 && *(str_begin + 1) == '/') {
					str_begin += 2;
					cleanWhitespaceFromBegin(str_begin, str_end);
					if (str_end - str_begin < static_cast< ssize_t >(tag_name.size()) || std::string(str_begin, str_begin + tag_name.size()) != tag_name) {
						throw Exception("Tag \"" + tag_name + "\" was not closed!");
					}
					str_begin += tag_name.size();
					cleanWhitespaceFromBegin(str_begin, str_end);
					if (str_begin == str_end || *str_begin != '>') {
						throw Exception("Closing tag of \"" + tag_name + "\" was not complete!");
					}
					// Skip '>'
					str_begin ++;
					// Reading of this tag is completed!
					break;
				}
				XMLNode* new_child = new XMLNode;
				children.push_back(new_child);
				try {
					new_child->deserialize(str_begin, str_end);
				}
				catch( ... ) {
					for (XMLNodes::iterator children_it = children.begin();
					     children_it != children.end();
					     children_it ++) {
						delete *children_it;
					}
					children.clear();
					throw;
				}
				cleanWhitespaceFromBegin(str_begin, str_end);
			}
		}

	}
	// Text node
	else {
		type = TEXT;
		value = "";
		while (str_begin != str_end) {
			char c = *str_begin;
			if (c == '<') {
				break;
			}
			value += c;
			str_begin ++;
		}
		// Remove whitespace from end
		cleanWhitespaceFromEnd(value);
	}
}

inline void XMLNode::cleanWhitespaceFromBegin(std::string::const_iterator& str_begin, std::string::const_iterator const& str_end)
{
	while (str_begin != str_end && (*str_begin == ' ' ||
	                                *str_begin == '\t' ||
	                                *str_begin == '\x0a' ||
	                                *str_begin == '\x0d')) {
		str_begin ++;
	}
}

inline void XMLNode::cleanWhitespaceFromEnd(std::string& str)
{
	while (!str.empty() && (*str.rbegin() == ' ' ||
	                        *str.rbegin() == '\t' ||
	                        *str.rbegin() == '\x0a' ||
	                        *str.rbegin() == '\x0d')) {
		str.resize(str.size() - 1);
	}
}

inline void XMLNode::cleanWhitespaceAndCommentsFromBegin(std::string::const_iterator& str_begin, std::string::const_iterator const& str_end)
{
	char const LF = 0x0A;
	char const CR = 0x0D;
	do {
		// End has been reached
		if (str_begin == str_end) break;

		char c = *str_begin;

		// Something that is not whitespace, nor comment
		if (c != '<' && c != LF && c != CR && c!= '\t' && c != ' ') break;

		// Comment?
		if (c == '<') {

			// If this is not comment, then break
			if (str_end - str_begin < 4) {
				break;
			}
			if (*(str_begin + 1) != '!' ||
			    *(str_begin + 2) != '-' ||
			    *(str_begin + 3) != '-') {
				break;
			}

			// This is comment
			str_begin += 4;

			// Read until "-->" is met.
			char comm_end[3] = { 0, 0, 0 };
			do {
				if (str_begin == str_end) {
					throw Exception("XML comment was not closed!");
				}
				comm_end[0] = comm_end[1];
				comm_end[1] = comm_end[2];
				comm_end[2] = *str_begin;
				str_begin ++;
			} while (std::strncmp(comm_end, "-->", 3) != 0);
			str_begin --;
		}

		str_begin ++;

	} while (true);
}

inline void XMLNode::cleanWhitespaceAndCommentsFromEnd(std::string::const_iterator const& str_begin, std::string::const_iterator& str_end)
{
	char const LF = 0x0A;
	char const CR = 0x0D;
	do {
		// Begin has been reached
		if (str_begin == str_end) break;

		str_end --;
		char c = *str_end;

		// Something that is not whitespace, nor end of comment
		if (c != '>' && c != LF && c != CR && c!= '\t' && c != ' ') {
			break;
		}

		// Comment?
		if (c == '>') {

			// If this is not comment, then break
			if (str_end - str_begin < 2) {
				break;
			}
			if (*(str_end - 2) != '-' ||
			    *(str_end - 1) != '-') {
				break;
			}

			// This is comment
			str_end -= 3;

			// Read until "<!--" is met.
			char comm_begin[4] = { 0, 0, 0 };
			do {
				if (str_begin == str_end) {
					throw Exception("Unexpected XML comment closing!");
				}
				comm_begin[3] = comm_begin[2];
				comm_begin[2] = comm_begin[1];
				comm_begin[1] = comm_begin[0];
				comm_begin[0] = *str_end;
				str_end --;
			} while (std::strncmp(comm_begin, "<!--", 4) != 0);
			str_end ++;
		}

	} while (true);
	str_end ++;
}

inline void XMLNode::readUntil(std::string::const_iterator& str_begin, std::string::const_iterator const& str_end, std::string const& chars)
{
	while (str_begin != str_end && chars.find(*str_begin) == std::string::npos) {
		str_begin ++;
	}
}

}

#endif
