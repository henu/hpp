#ifndef HPP_BUFFEROBJECT
#define HPP_BUFFEROBJECT

#include "glsystem.h"
#include "noncopyable.h"
#include "gldebug.h"
#include "assert.h"

#include <GL/gl.h>

namespace Hpp
{

class Bufferobject : public NonCopyable
{

public:

	// These can be used to prevent slow name lookups.
	enum Shortcut
	{
		POS = 0,
		NORMAL = 1,
		TANGENT = 2,
		BINORMAL = 3,
		UV = 4,
		CLR = 5,
		INDEX = 6,
		SHORTCUT_END = 7
	};

	// The size is not given in bytes, but in elemenst
	inline Bufferobject(GLenum target, GLenum type, GLenum usage, GLint components, GLvoid const* data, size_t size);
	inline ~Bufferobject(void);

	// Draws Bufferobject as specific elements.
	inline void drawElements(GLenum mode, size_t offset = 0, size_t amount = size_t(-1)) const;

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
// TODO: Support GL_HALF_FLOAT, GL_FIXED, GL_INT_2_10_10_10_REV, and GL_UNSIGNED_INT_2_10_10_10_REV!
	size_t bytesize = size;
	if (type == GL_FLOAT) bytesize *= sizeof(GLfloat);
	else if (type == GL_DOUBLE) bytesize *= sizeof(GLdouble);
	else if (type == GL_BYTE) bytesize *= sizeof(GLbyte);
	else if (type == GL_UNSIGNED_BYTE) bytesize *= sizeof(GLubyte);
	else if (type == GL_SHORT) bytesize *= sizeof(GLshort);
	else if (type == GL_UNSIGNED_SHORT) bytesize *= sizeof(GLushort);
	else if (type == GL_INT) bytesize *= sizeof(GLint);
	else if (type == GL_UNSIGNED_INT) bytesize *= sizeof(GLuint);
	else HppAssert(false, "Invalid type!");

	GlSystem::GenBuffers(1, &buf_id);
	GlSystem::BindBuffer(target, buf_id);
	GlSystem::BufferData(target, bytesize, data, usage);
}

inline Bufferobject::~Bufferobject(void)
{
	GlSystem::DeleteBuffers(1, &buf_id);
}

inline void Bufferobject::drawElements(GLenum mode, size_t offset, size_t amount) const
{
	if (offset > size) {
		offset = size;
	}
	if (amount > size - offset) {
		amount = size - offset;
	}

	HppCheckGlErrors();
	GlSystem::BindBuffer(target, buf_id);
	HppCheckGlErrors();
	glDrawElements(mode, amount, type, (GLvoid*)offset);
	HppCheckGlErrors();
}

}

#endif
