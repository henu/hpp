#ifndef HPP_MESH
#define HPP_MESH

#include "bufferobject.h"
#include "exception.h"

#include <string>
#include <map>

namespace Hpp
{

class Mesh
{

public:

	inline Mesh(void);
	inline ~Mesh(void);

	inline void setBuffer(std::string const& name, GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size);

	inline Bufferobject const* getBuffer(std::string const& name) const;

private:

	typedef std::map< std::string, Bufferobject* > Bufs;

	Bufs bufs;

};

inline Mesh::Mesh(void)
{
}

inline Mesh::~Mesh(void)
{
	for (Bufs::iterator bufs_it = bufs.begin();
	     bufs_it != bufs.end();
	     ++ bufs_it) {
		delete bufs_it->second;
	}
}

inline void Mesh::setBuffer(std::string const& name, GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size)
{
	// Remove possible old buffer
	Bufs::iterator bufs_find = bufs.find(name);
	if (bufs_find != bufs.end()) {
		delete bufs_find->second;
	}

	// Create and store new buffer
	bufs[name] = new Bufferobject(target, type, usage, components, data, size);
}

inline Bufferobject const* Mesh::getBuffer(std::string const& name) const
{
	Bufs::const_iterator bufs_find = bufs.find(name);
	if (bufs_find == bufs.end()) {
		throw Exception("Unable to get buffer \"" + name + "\" from mesh because it does not exist!");
	}
	return bufs_find->second;
}

}

#endif
