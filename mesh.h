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
	inline Bufferobject const* getBuffer(Bufferobject::Shortcut shortcut) const;

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
	Bufferobject* bufs_by_shortcut[Bufferobject::SHORTCUT_END];

};

inline Mesh::Mesh(void)
{
	Hpp::toZero(bufs_by_shortcut, sizeof(bufs_by_shortcut));
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
	Bufferobject* bo = new Bufferobject(target, type, usage, components, data, size);
	bufs[name] = Buffer(bo, true);

	// If buffer has shortcut name, then store it as shortcut too
	if (name == "pos") {
		bufs_by_shortcut[Bufferobject::POS] = bo;
	} else if (name == "normal") {
		bufs_by_shortcut[Bufferobject::NORMAL] = bo;
	} else if (name == "tangent") {
		bufs_by_shortcut[Bufferobject::TANGENT] = bo;
	} else if (name == "binormal") {
		bufs_by_shortcut[Bufferobject::BINORMAL] = bo;
	} else if (name == "uv") {
		bufs_by_shortcut[Bufferobject::UV] = bo;
	} else if (name == "clr") {
		bufs_by_shortcut[Bufferobject::CLR] = bo;
	} else if (name == "index") {
		bufs_by_shortcut[Bufferobject::INDEX] = bo;
	}

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

	// If buffer has shortcut name, then store it as shortcut too
	if (name == "pos") {
		bufs_by_shortcut[Bufferobject::POS] = bo;
	} else if (name == "normal") {
		bufs_by_shortcut[Bufferobject::NORMAL] = bo;
	} else if (name == "tangent") {
		bufs_by_shortcut[Bufferobject::TANGENT] = bo;
	} else if (name == "binormal") {
		bufs_by_shortcut[Bufferobject::BINORMAL] = bo;
	} else if (name == "uv") {
		bufs_by_shortcut[Bufferobject::UV] = bo;
	} else if (name == "clr") {
		bufs_by_shortcut[Bufferobject::CLR] = bo;
	} else if (name == "index") {
		bufs_by_shortcut[Bufferobject::INDEX] = bo;
	}
}

inline Bufferobject const* Mesh::getBuffer(std::string const& name) const
{
	Buffers::const_iterator bufs_find = bufs.find(name);
	if (bufs_find == bufs.end()) {
		throw Exception("Unable to get buffer \"" + name + "\" from mesh because it does not exist!");
	}
	return bufs_find->second.bo;
}

inline Bufferobject const* Mesh::getBuffer(Bufferobject::Shortcut shortcut) const
{
	HppAssert(shortcut < Bufferobject::SHORTCUT_END, "Invalid attribute shortcut!");
	Bufferobject const* result = bufs_by_shortcut[shortcut];
	if (!result) {
		throw Exception("Unable to get buffer by shortcut from mesh because it does not exist!");
	}
	return result;
}

}

#endif
