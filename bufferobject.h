#ifndef HPP_BUFFEROBJECT
#define HPP_BUFFEROBJECT

#include "glsystem.h"
#include "noncopyable.h"

#include <GL/gl.h>

namespace Hpp
{

class Bufferobject : public NonCopyable
{

public:

	inline Bufferobject(GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size);
	inline ~Bufferobject(void);

	// Draws Bufferobject as specific elements.
	inline void drawElements(GLenum mode, size_t offset = 0) const;

	inline void setNormalized(bool normalized) { this->normalized = normalized; }

	inline GLuint getBufferId(void) const { return buf_id; }
	inline GLenum getTarget(void) const { return target; }
	inline GLenum getType(void) const { return type; }
	inline GLint getComponents(void) const { return components; }
	inline GLenum getSize(void) const { return size; }
	inline bool getNormalized(void) const { return normalized; }

private:

	GLuint buf_id;
	GLenum target;
	GLenum type;
	GLint components;
	size_t size;

	bool normalized;

};

inline Bufferobject::Bufferobject(GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size) :
target(target),
type(type),
components(components),
size(size)
{
	Hpp::GlSystem::GenBuffers(1, &buf_id);
	Hpp::GlSystem::BindBuffer(target, buf_id);
	Hpp::GlSystem::BufferData(target, size, data, usage);
}

inline Bufferobject::~Bufferobject(void)
{
	Hpp::GlSystem::DeleteBuffers(1, &buf_id);
}

inline void Bufferobject::drawElements(GLenum mode, size_t offset) const
{
	Hpp::GlSystem::BindBuffer(target, buf_id);
	glDrawElements(mode, size, type, (GLvoid*)offset);
}

}

#endif
