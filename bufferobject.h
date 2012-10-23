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

	inline Bufferobject(GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size);
	inline ~Bufferobject(void);

	inline void setNormalized(bool normalized) { this->normalized = normalized; }

	inline GLuint getBufferId(void) const { return buf_id; }
	inline GLenum getType(void) const { return type; }
	inline GLint getComponents(void) const { return components; }
	inline bool getNormalized(void) const { return normalized; }

private:

	GLuint buf_id;
	GLenum type;
	GLint components;

	bool normalized;

};

inline Bufferobject::Bufferobject(GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size) :
type(type),
components(components)
{
	Hpp::GlSystem::GenBuffers(1, &buf_id);
	Hpp::GlSystem::BindBuffer(GL_ARRAY_BUFFER, buf_id);
	Hpp::GlSystem::BufferData(GL_ARRAY_BUFFER, size, data, usage);
}

inline Bufferobject::~Bufferobject(void)
{
	Hpp::GlSystem::DeleteBuffers(1, &buf_id);
}

}

#endif
