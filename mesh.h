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

	// Type for ID of buffer. There are several predefined IDs.
	// User can make new ones, but they need to have positive value.
	typedef int16_t BufId;
	static BufId const POS = -7;
	static BufId const NORMAL = -6;
	static BufId const TANGENT = -5;
	static BufId const BINORMAL = -4;
	static BufId const UV = -3;
	static BufId const COLOR = -2;
	static BufId const INDEX = -1;

	inline Mesh(void);
	inline ~Mesh(void);

	// The size is not given in bytes, but in elemenst
	inline void setBuffer(BufId buf_id, GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size);
	inline void setBuffer(BufId buf_id, Bufferobject* bo, bool release_automatically = false);

	inline Bufferobject const* getBuffer(BufId buf_id) const;

private:

	static BufId const NUM_OF_DEFAULT_BUFIDS = 7;

	struct Buffer
	{
		inline Buffer(void) : bo(NULL), release_automatically(false) { }
		inline Buffer(Bufferobject* bo, bool release_automatically) : bo(bo), release_automatically(release_automatically) { }
		inline Buffer(Buffer const& b) : bo(b.bo), release_automatically(b.release_automatically) { }
		Bufferobject* bo;
		bool release_automatically;
	};
	typedef std::vector< Buffer > Buffers;

	Buffers bufs;

};

inline Mesh::Mesh(void)
{
	bufs.assign(NUM_OF_DEFAULT_BUFIDS, Buffer());
}

inline Mesh::~Mesh(void)
{
	for (Buffers::iterator bufs_it = bufs.begin();
	     bufs_it != bufs.end();
	     ++ bufs_it) {
		Buffer& buf = *bufs_it;
		if (buf.release_automatically) {
			delete buf.bo;
		}
	}
}

inline void Mesh::setBuffer(BufId buf_id, GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size)
{
	BufId real_buf_id = buf_id + NUM_OF_DEFAULT_BUFIDS;
	HppAssert(real_buf_id >= 0, "Buffer ID underflow!");

	// Remove possible old buffer
	if (real_buf_id < BufId(bufs.size())) {
		delete bufs[real_buf_id].bo;
	}
	// There is not enough slots, so add them
	else {
		while (real_buf_id >= BufId(bufs.size())) {
			bufs.push_back(Buffer());
		}
	}

	// Create and store new buffer
	Bufferobject* bo = new Bufferobject(target, type, usage, components, data, size);
	bufs[real_buf_id] = Buffer(bo, true);
}

inline void Mesh::setBuffer(BufId buf_id, Bufferobject* bo, bool release_automatically)
{
	BufId real_buf_id = buf_id + NUM_OF_DEFAULT_BUFIDS;
	HppAssert(real_buf_id >= 0, "Buffer ID underflow!");

	// Remove possible old buffer
	if (real_buf_id < BufId(bufs.size())) {
		delete bufs[real_buf_id].bo;
	}
	// There is not enough slots, so add them
	else {
		while (real_buf_id >= BufId(bufs.size())) {
			bufs.push_back(Buffer());
		}
	}

	// Store buffer
	bufs[real_buf_id] = Buffer(bo, release_automatically);
}

inline Bufferobject const* Mesh::getBuffer(BufId buf_id) const
{
	BufId real_buf_id = buf_id + NUM_OF_DEFAULT_BUFIDS;
	HppAssert(real_buf_id >= 0, "Buffer ID underflow!");

	if (real_buf_id >= BufId(bufs.size())) {
		throw Exception("Unable to get buffer #" + ssizeToStr(buf_id) + " from mesh because it does not exist!");
	}
	Bufferobject const* result = bufs[real_buf_id].bo;
	if (!result) {
		throw Exception("Unable to get buffer #" + ssizeToStr(buf_id) + " from mesh because it does not exist!");
	}
	return result;
}

}

#endif
