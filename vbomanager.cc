#include "vbomanager.h"

#include "display.h"
#include "assert.h"
#include "lock.h"
#include "glsystem.h"
#include "rendbuf_enums.h"
#include "inc_gl.h"

namespace Hpp
{

void VboManager::addVBOJob(GLenum target, void const* buf, size_t buf_len)
{
	Lock vbos_lock(vbos_mutex);
	HppAssert(vbos.find(buf) == vbos.end(), "That VBO already exists!");
	VBO new_vbo;
	new_vbo.buf_len = buf_len;
	new_vbo.target = target;
	new_vbo.id = 0;
	vbos[buf] = new_vbo;
}

GLsizei VboManager::getVBO(void const* buf, GLuint* caller_vbo_id)
{
	HppAssert(Display::isThisRenderingThread(), "VboManager::getVBO may be only called from OpenGL thread!");
	Lock vbos_lock(vbos_mutex);
	VBOs::iterator vbos_find = vbos.find(buf);
	HppAssert(vbos_find != vbos.end(), "VBO not found!");
	VBO& vbo = vbos_find->second;
	if (vbo.id == 0) {
		GlSystem::GenBuffers(1, &vbo.id);
		GlSystem::BindBuffer(vbo.target, vbo.id);
		GlSystem::BufferData(vbo.target, vbo.buf_len, buf, GL_STATIC_DRAW_ARB);
		// Now go all needers through and update their ID too
		for (VBONeeders::iterator needers_it = vbos_find->second.needers.begin();
		     needers_it != vbos_find->second.needers.end();
		     needers_it ++) {
			GLuint* vbo_id = needers_it->first;
			RendbufEnums::VBOState* vbo_state = needers_it->second;
			// Skip the called
			if (caller_vbo_id == vbo_id) {
				continue;
			}
			// The assignment is done in loop, since some other
			// thread may be reading this, and there is no
			// protection other than this loop.
// TODO: Is this safe?
			do {
				*vbo_id = vbo.id;
			} while (*vbo_id != vbo.id);
			do {
				*vbo_state = RendbufEnums::VBO_READY_FROM_MANAGER;
			} while (*vbo_state != RendbufEnums::VBO_READY_FROM_MANAGER);
		}
		vbos_find->second.needers.clear();
		GlSystem::BindBuffer(vbo.target, 0);
	}
	return vbo.id;
}

void VboManager::releaseVBO(void const* buf, GLuint id)
{
	Lock vbos_lock(vbos_mutex);
	// First check if this class knows about this buffer
	VBOs::iterator vbos_find = vbos.find(buf);
	if (vbos_find != vbos.end()) {
		// If it's zero, set it anyway. It means that VBO is not allocated yet!
		id = vbos_find->second.id;
		HppAssert(vbos_find->second.needers.empty(), "There are still needers of VBO that is being released!");
		vbos.erase(vbos_find);
	}
	if (id != 0) {
		HppAssert(std::find(vbos_release.begin(), vbos_release.end(), id) == vbos_release.end(), "This VBO is already being marked as released!");
		vbos_release.push_back(id);
	}
}

void VboManager::registerVBONeeder(void const* buf, GLuint* vbo_id, RendbufEnums::VBOState* vbo_state)
{
	Lock vbos_lock(vbos_mutex);
	VBOs::iterator vbos_find = vbos.find(buf);
	HppAssert(vbos_find != vbos.end(), "Try to register buffer that does not exist!");
	// First check if this VBO is already got
	if (vbos_find->second.id != 0) {
		*vbo_id = vbos_find->second.id;
		*vbo_state = RendbufEnums::VBO_READY_FROM_MANAGER;
		return;
	}
	// VBO is not yet loaded, so add this as a waiting Autobuf
	HppAssert(vbos_find->second.needers.find(vbo_id) == vbos_find->second.needers.end(), "Needer already found!");
	vbos_find->second.needers[vbo_id] = vbo_state;
}

void VboManager::unregisterVBONeeder(void const* buf, GLuint* vbo_id)
{
	Lock vbos_lock(vbos_mutex);
	VBOs::iterator vbos_find = vbos.find(buf);
	HppAssert(vbos_find != vbos.end(), "Buffer does not exist!");
// TODO: Is it good idea to let needer not exist?
	vbos_find->second.needers.erase(vbo_id);
}

VboManager::VboManager(void)
{
}

VboManager::~VboManager(void)
{
	Lock vbos_lock(vbos_mutex);
	HppAssert(vbos_release.empty(), "There are unreleased VBOs!");
}

void VboManager::cleanReleasableVbos(void)
{
	Lock vbos_lock(vbos_mutex);
	if (!vbos_release.empty()) {
		GlSystem::DeleteBuffers(vbos_release.size(), &vbos_release[0]);
		vbos_release.clear();
	}
}

}
