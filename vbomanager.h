#ifndef HPP_VBOMANAGER_H
#define HPP_VBOMANAGER_H

#include "mutex.h"
#include "rendbuf_enums.h"
#include "inc_gl.h"

#include <map>
#include <vector>

namespace Hpp
{

class VboManager
{

	friend class Display;

public:

	// VBO handling functions for other threads than OpenGL thread.
	//     The first function is used to ask for VBO creation next time the
	// VBO is being used. It will be identified by pointer "buf". Contents
	// of buf may *NOT* be modified after VBO job is assigned.
	//     The second function is for getting VBO if the used does not know
	// it's ID yet. If VBO does not exist yet, it will be created
	// immediately.
	//     The third function is to release VBO if caller is not in the
	// correct thread or if VBO was creted by this handler class.
	void addVBOJob(GLenum target, void const* buf, size_t buf_len);
	GLsizei getVBO(void const* buf, GLuint* caller_vbo_id);
	void releaseVBO(void const* buf, GLuint id);
	// Functions to handle needing of VBO id
	void registerVBONeeder(void const* buf, GLuint* vbo_id, RendbufEnums::VBOState* vbo_state);
	void unregisterVBONeeder(void const* buf, GLuint* vbo_id);

private:

	typedef std::map< GLuint*, RendbufEnums::VBOState* > VBONeeders;
	struct VBO
	{
		GLsizei buf_len;
		GLenum target;
		GLuint id;
		VBONeeders needers;
	};
	typedef std::map< void const*, VBO > VBOs;
	typedef std::vector< GLuint > ReleasableVBOs;

	// Data
	Mutex vbos_mutex;
	VBOs vbos;
	ReleasableVBOs vbos_release;

	// Constructor and destructor in private to prevent other instances of
	// this class.
	VboManager(void);
	~VboManager(void);

	// Tries to release VBOs that were unable to release later because the
	// execution was in wrong thread.
	void cleanReleasableVbos(void);

};

}

#endif
