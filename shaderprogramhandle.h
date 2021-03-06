#ifndef HPP_SHADERPROGRAMHANDLE_H
#define HPP_SHADERPROGRAMHANDLE_H

#include "shaderprogram.h"
#include "inc_gl.h"

#include <vector>

namespace Hpp
{

class Shaderprogramhandle
{

	friend class Shaderprogram;

public:

	inline void enable(void);
	inline void disable(void);

	// Functions to set uniforms
	inline void setUniform(GLint uniform_loc, Matrix3 const& mat, bool transpose = false);
	inline void setUniform(GLint uniform_loc, Matrix4 const& mat, bool transpose = false);
	inline void setUniform(GLint uniform_loc, Vector3 const& v);
	inline void setUniform(GLint uniform_loc, Vector3 const& v, Real w);
	inline void setUniform(GLint uniform_loc, Color const& color, Pixelformat force_format = RGBA);
	inline void setUniform1f(GLint uniform_loc, GLfloat f0);
	inline void setUniform2f(GLint uniform_loc, GLfloat f0, GLfloat f1);
	inline void setUniform3f(GLint uniform_loc, GLfloat f0, GLfloat f1, GLfloat f2);
	inline void setUniform4f(GLint uniform_loc, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3);
	inline void setUniform1i(GLint uniform_loc, GLint i0);
	inline void setUniform2i(GLint uniform_loc, GLint i0, GLint i1);
	inline void setUniform3i(GLint uniform_loc, GLint i0, GLint i1, GLint i2);
	inline void setUniform4i(GLint uniform_loc, GLint i0, GLint i1, GLint i2, GLint i3);

	// Functions to set Bufferobjects
	inline void setBufferobject(GLint vertexattr_loc, Bufferobject const* buf);

	// Returns list of active uniforms. This is mainly for debugging purposes.
	inline Strings getListOfUniforms(void) const;

private:

	// ========================================
	// Types and functions for friends
	// ========================================

	typedef std::vector< GLint > Shortcuts;

	inline Shaderprogramhandle(Shaderprogram* program,
	                           GLuint prog_id,
	                           Shortcuts const& uniformshortcuts,
	                           Bools const& existing_vertexattrs);

private:

	Shaderprogram* program;
	GLuint prog_id;
	Shortcuts uniformshortcuts;
	Bools existing_vertexattrs;

	// Is the Shaderprogram enabled for rendering
	bool enabled;

	inline GLint getUniformRealLocation(uint16_t loc_raw) { return uniformshortcuts[loc_raw]; }

	inline void checkGlErrorsForUniformSetting(GLint uniform_loc_real) const;

};

inline void Shaderprogramhandle::enable(void)
{
	HppAssert(!enabled, "Already enabled!");
	HppCheckGlErrors();
	HppCheckForCorrectThread();

	// Inform Shaderprogram about this enabling
	program->handleEnabled();

	GlSystem::UseProgram(prog_id);

	HppCheckGlErrors();

	enabled = true;
}

inline void Shaderprogramhandle::disable(void)
{
	HppAssert(enabled, "Not enabled!");
	HppCheckForCorrectThread();
	HppCheckGlErrors();

	GlSystem::UseProgram(0);

	HppCheckGlErrors();

	// Inform Shaderprogram about this disabling
	program->handleDisabled();

	enabled = false;
}

inline void Shaderprogramhandle::setUniform(GLint uniform_loc, Matrix3 const& mat, bool transpose)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::UniformMatrix3fv(uniform_loc_real, 1, transpose, mat.getCells());
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform(GLint uniform_loc, Matrix4 const& mat, bool transpose)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::UniformMatrix4fv(uniform_loc_real, 1, transpose, mat.getCells());
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform(GLint uniform_loc, Vector3 const& v)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform3f(uniform_loc_real, v.x, v.y, v.z);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform(GLint uniform_loc, Vector3 const& v, Real w)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform4f(uniform_loc_real, v.x, v.y, v.z, w);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform(GLint uniform_loc, Color const& color, Pixelformat force_format)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);

	if (uniform_loc_real >= 0) {
		if (force_format == RGBA) {
			GlSystem::Uniform4f(uniform_loc_real, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
		} else if (force_format == RGB) {
			GlSystem::Uniform3f(uniform_loc_real, color.getRed(), color.getGreen(), color.getBlue());
		} else if (force_format == GRAYSCALE) {
			GlSystem::Uniform1f(uniform_loc_real, color.getValue());
		} else if (force_format == GRAYSCALE_ALPHA) {
			GlSystem::Uniform2f(uniform_loc_real, color.getValue(), color.getAlpha());
		} else if (force_format == ALPHA) {
			GlSystem::Uniform1f(uniform_loc_real, color.getAlpha());
		} else {
			throw Exception("Invalid uniform format!");
		}
	}

	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform1f(GLint uniform_loc, GLfloat f0)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform1f(uniform_loc_real, f0);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform2f(GLint uniform_loc, GLfloat f0, GLfloat f1)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform2f(uniform_loc_real, f0, f1);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform3f(GLint uniform_loc, GLfloat f0, GLfloat f1, GLfloat f2)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform3f(uniform_loc_real, f0, f1, f2);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform4f(GLint uniform_loc, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform4f(uniform_loc_real, f0, f1, f2, f3);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform1i(GLint uniform_loc, GLint i0)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform1i(uniform_loc_real, i0);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform2i(GLint uniform_loc, GLint i0, GLint i1)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform2i(uniform_loc_real, i0, i1);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform3i(GLint uniform_loc, GLint i0, GLint i1, GLint i2)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform3i(uniform_loc_real, i0, i1, i2);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setUniform4i(GLint uniform_loc, GLint i0, GLint i1, GLint i2, GLint i3)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLint uniform_loc_real = getUniformRealLocation(uniform_loc);
	if (uniform_loc_real >= 0) GlSystem::Uniform4i(uniform_loc_real, i0, i1, i2, i3);
	checkGlErrorsForUniformSetting(uniform_loc_real);
}

inline void Shaderprogramhandle::setBufferobject(GLint vertexattr_loc, Bufferobject const* buf)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");

	// Ensure this bufferobject really exist. If shader does
	// not have an attribute with that name, consider that
	// it wants to discard all information in it.
	if (!existing_vertexattrs[vertexattr_loc]) {
		return;
	}

	buf->useAsVertexAttribute(vertexattr_loc);

	HppCheckGlErrors();
}

Strings Shaderprogramhandle::getListOfUniforms(void) const
{
	GLsizei const NAME_MAXLEN = 1024;
	GLchar name[NAME_MAXLEN];

	GLint uniforms;
	GlSystem::GetProgramiv(prog_id, GL_ACTIVE_UNIFORMS, &uniforms);

	Strings result;
	result.reserve(uniforms);

	for (GLint uniform_id = 0; uniform_id < uniforms; ++ uniform_id) {
		GLsizei name_len;
		GLint uniform_size;
		GLenum uniform_type;
		GlSystem::GetActiveUniform(prog_id,
		                           uniform_id,
		                           NAME_MAXLEN,
		                           &name_len,
		                           &uniform_size,
		                           &uniform_type,
		                           name);
		result.push_back(std::string(name, name_len));
	}

	return result;
}

inline Shaderprogramhandle::Shaderprogramhandle(Shaderprogram* program,
                                                GLuint prog_id,
                                                Shortcuts const& uniformshortcuts,
                                                Bools const& existing_vertexattrs) :
program(program),
prog_id(prog_id),
uniformshortcuts(uniformshortcuts),
existing_vertexattrs(existing_vertexattrs),
enabled(false)
{
}

inline void Shaderprogramhandle::checkGlErrorsForUniformSetting(GLint uniform_loc_real) const
{
	#ifndef NDEBUG

	// First check that uniform is not out of range
	GLint uniforms;
	GlSystem::GetProgramiv(prog_id, GL_ACTIVE_UNIFORMS, &uniforms);
	if (uniform_loc_real >= uniforms) {
		throw Hpp::Exception("Uniform ID overflow!");
	}

	// Convert possible error to string
	std::string error_str;
	GLenum error = glGetError();
	switch (error) {

	case GL_NO_ERROR:
		return;

	case GL_INVALID_ENUM:
		error_str = "Invalid enum!";
		break;

	case GL_INVALID_VALUE:
		error_str = "Numeric argument out of range!";
		break;

	case GL_INVALID_OPERATION:
		error_str = "Invalid operation!";
		break;

	case GL_STACK_OVERFLOW:
		error_str = "Stack overflow!";
		break;

	case GL_STACK_UNDERFLOW:
		error_str = "Stack underflow!";
		break;

	case GL_OUT_OF_MEMORY:
		error_str = "Out of memory!";
		break;

	default:
		std::ostringstream errorcode_strm;
		errorcode_strm << error;
		std::string errorcode_str;
		errorcode_str = errorcode_strm.str();
		error_str = "Unrecognized error #" +  errorcode_str + "!";
		break;

	}

	// Get name of uniform
	GLsizei const NAME_MAXLEN = 1024;
	GLchar name[NAME_MAXLEN];
	GLsizei name_len;
	GLint uniform_size;
	GLenum uniform_type;
	GlSystem::GetActiveUniform(prog_id,
	                           uniform_loc_real,
	                           NAME_MAXLEN,
	                           &name_len,
	                           &uniform_size,
	                           &uniform_type,
	                           name);

	throw Hpp::Exception("Error while setting uniform \"" + std::string(name, name_len) + "\": " + error_str);

	#else
	(void)uniform_loc_real;
	#endif
}


}

#endif
