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

	// The size is not given in bytes, but in elemenst
	inline void setBuffer(std::string const& name, GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size);
	inline void setBuffer(std::string const& name, Bufferobject* bo, bool release_automatically = false);

	inline Bufferobject const* getBuffer(std::string const& name) const;

private:

	struct Buffer
	{
		inline Buffer(void) { }
		inline Buffer(Bufferobject* bo, bool release_automatically) : bo(bo), release_automatically(release_automatically) { }
		inline Buffer(Buffer const& b) : bo(b.bo), release_automatically(b.release_automatically) { }
		Bufferobject* bo;
		bool release_automatically;
	};
	typedef std::map< std::string, Buffer > Buffers;

	Buffers bufs;

};

inline Mesh::Mesh(void)
{
}

inline Mesh::~Mesh(void)
{
	for (Buffers::iterator bufs_it = bufs.begin();
	     bufs_it != bufs.end();
	     ++ bufs_it) {
		if (bufs_it->second.release_automatically) {
			delete bufs_it->second.bo;
		}
	}
}

inline void Mesh::setBuffer(std::string const& name, GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size)
{
	// Remove possible old buffer
	Buffers::iterator bufs_find = bufs.find(name);
	if (bufs_find != bufs.end()) {
		if (bufs_find->second.release_automatically) {
			delete bufs_find->second.bo;
		}
	}

	// Create and store new buffer
	bufs[name] = Buffer(new Bufferobject(target, type, usage, components, data, size), true);
}

inline void Mesh::setBuffer(std::string const& name, Bufferobject* bo, bool release_automatically)
{
	// Remove possible old buffer
	Buffers::iterator bufs_find = bufs.find(name);
	if (bufs_find != bufs.end()) {
		if (bufs_find->second.release_automatically) {
			delete bufs_find->second.bo;
		}
	}

	// Create and store new buffer
	bufs[name] = Buffer(bo, release_automatically);
}

inline Bufferobject const* Mesh::getBuffer(std::string const& name) const
{
	Buffers::const_iterator bufs_find = bufs.find(name);
	if (bufs_find == bufs.end()) {
		throw Exception("Unable to get buffer \"" + name + "\" from mesh because it does not exist!");
	}
	return bufs_find->second.bo;
}

}

#endif
