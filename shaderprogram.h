#ifndef HPP_SHADERPROGRAM_H
#define HPP_SHADERPROGRAM_H

#include "bufferobject.h"
#include "gldebug.h"
#include "glsystem.h"
#include "shader.h"
#include "inc_gl.h"
#include "noncopyable.h"
#include "matrix3.h"
#include "matrix4.h"

#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>

namespace Hpp
{

class Shaderprogram : private NonCopyable
{

public:

	typedef std::set< std::string > Flags;

	// Constructor and destructor
	inline Shaderprogram(void);
	inline ~Shaderprogram(void);

	// Attachs shaders to program
	inline void attachShader(Shader const& shader);

	// Enables/disables shader
	inline void enable(Flags const& flags = Flags());
	inline void disable(void);

	// Functions to set uniforms
	inline void setUniform(std::string const& name, Matrix3 const& mat, bool transpose = false);
	inline void setUniform(std::string const& name, Matrix4 const& mat, bool transpose = false);
	inline void setUniform(std::string const& name, Vector3 const& v);
	inline void setUniform(std::string const& name, Vector3 const& v, Real w);
	inline void setUniform(std::string const& name, Color const& color, Pixelformat force_format = RGBA);
	inline void setUniform1f(std::string const& name, GLfloat f0);
	inline void setUniform2f(std::string const& name, GLfloat f0, GLfloat f1);
	inline void setUniform3f(std::string const& name, GLfloat f0, GLfloat f1, GLfloat f2);
	inline void setUniform4f(std::string const& name, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3);
	inline void setUniform1i(std::string const& name, GLint i0);
	inline void setUniform2i(std::string const& name, GLint i0, GLint i1);
	inline void setUniform3i(std::string const& name, GLint i0, GLint i1, GLint i2);
	inline void setUniform4i(std::string const& name, GLint i0, GLint i1, GLint i2, GLint i3);

	inline void setBufferobject(std::string const& name, Bufferobject const* buf);
	inline void setBufferobject(Bufferobject::Shortcut shortcut, Bufferobject const* buf);

	// Returns GLSL Id of enabled program. This may deprecate between
	// enables and disables.
	inline GLuint getGLSLProgram(void) const;

private:

	typedef std::vector< Shader > Shaders;

	typedef std::map< std::string, GLuint > AttribLocations;

	struct LinkedProgram
	{
		GLuint prog_id;
		AttribLocations attribs;
		GLint attribs_by_shortcuts[Bufferobject::SHORTCUT_END];
	};
	typedef std::map< Flags, LinkedProgram > LinkedPrograms;

	typedef std::vector< GLuint > CompiledShaders;

	typedef std::vector< std::string > Lines;

	typedef std::set< GLuint > AttribsInUse;

	// If shader is enabled, then this pointer points to enabled program.
	LinkedProgram* enabled_program;

	Shaders shaders;

	LinkedPrograms lprogs;

	AttribsInUse used_attribs;

	inline void linkProgram(Flags const& flags);

	inline void cleanLinkedPrograms(void);
	inline static void cleanCompiledShaders(CompiledShaders& shaders);

};

inline Shaderprogram::Shaderprogram(void) :
enabled_program(NULL)
{
	HppCheckForCorrectThread();
}

inline Shaderprogram::~Shaderprogram(void)
{
	HppCheckForCorrectThread();
	cleanLinkedPrograms();
}

inline void Shaderprogram::attachShader(Shader const& shader)
{
	HppCheckForCorrectThread();
	cleanLinkedPrograms();
	shaders.push_back(shader);
}

inline void Shaderprogram::enable(Flags const& flags)
{
	HppCheckGlErrors();
	HppCheckForCorrectThread();
// TODO: Ensure only one shaderprogram is enabled!
	HppAssert(!enabled_program, "Already enabled!");

	// Ensure program is linked with these flags
	LinkedPrograms::iterator lprogs_find = lprogs.find(flags);
	if (lprogs_find == lprogs.end()) {
		linkProgram(flags);
		lprogs_find = lprogs.find(flags);
	}

	HppAssert(lprogs_find != lprogs.end(), "Fail!");
	enabled_program = &lprogs_find->second;
	GlSystem::UseProgram(enabled_program->prog_id);
	HppCheckGlErrors();

	HppAssert(used_attribs.empty(), "There are already bound Bufferobjects!");
}

inline void Shaderprogram::disable(void)
{
	HppCheckForCorrectThread();
	HppAssert(enabled_program, "Not enabled!");
	HppCheckGlErrors();
	GlSystem::UseProgram(0);
	HppCheckGlErrors();

	// Disable bufferobjects
	for (AttribsInUse::iterator used_attribs_it = used_attribs.begin();
	     used_attribs_it != used_attribs.end();
	     ++ used_attribs_it) {
		GlSystem::DisableVertexAttribArray(*used_attribs_it);
	}
	used_attribs.clear();

	enabled_program = NULL;
}

inline void Shaderprogram::setUniform(std::string const& name, Matrix3 const& mat, bool transpose)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::UniformMatrix3fv(uniform_id, 1, transpose, mat.getCells());
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform(std::string const& name, Matrix4 const& mat, bool transpose)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::UniformMatrix4fv(uniform_id, 1, transpose, mat.getCells());
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform(std::string const& name, Vector3 const& v)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform3f(uniform_id, v.x, v.y, v.z);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform(std::string const& name, Vector3 const& v, Real w)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform4f(uniform_id, v.x, v.y, v.z, w);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform(std::string const& name, Color const& color, Pixelformat force_format)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	if (force_format == RGBA) {
		GlSystem::Uniform4f(uniform_id, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	} else if (force_format == RGB) {
		GlSystem::Uniform3f(uniform_id, color.getRed(), color.getGreen(), color.getBlue());
	} else if (force_format == GRAYSCALE) {
		GlSystem::Uniform1f(uniform_id, color.getValue());
	} else if (force_format == GRAYSCALE_ALPHA) {
		GlSystem::Uniform2f(uniform_id, color.getValue(), color.getAlpha());
	} else if (force_format == ALPHA) {
		GlSystem::Uniform1f(uniform_id, color.getAlpha());
	} else {
		throw Exception("Invalid uniform format!");
	}

	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform1f(std::string const& name, GLfloat f0)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform1f(uniform_id, f0);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform2f(std::string const& name, GLfloat f0, GLfloat f1)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform2f(uniform_id, f0, f1);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform3f(std::string const& name, GLfloat f0, GLfloat f1, GLfloat f2)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform3f(uniform_id, f0, f1, f2);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform4f(std::string const& name, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform4f(uniform_id, f0, f1, f2, f3);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform1i(std::string const& name, GLint i0)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform1i(uniform_id, i0);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform2i(std::string const& name, GLint i0, GLint i1)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform2i(uniform_id, i0, i1);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform3i(std::string const& name, GLint i0, GLint i1, GLint i2)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform3i(uniform_id, i0, i1, i2);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform4i(std::string const& name, GLint i0, GLint i1, GLint i2, GLint i3)
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(enabled_program->prog_id, name.c_str());
	GlSystem::Uniform4i(uniform_id, i0, i1, i2, i3);
	HppCheckGlErrors();
}

inline void Shaderprogram::setBufferobject(std::string const& name, Bufferobject const* buf)
{
	HppCheckGlErrors();

	// Find vertex attribute location. If shader does not
	// have an attribute with that name, consider that
	// it wants to discard all information in it.
	AttribLocations::iterator attribs_find = enabled_program->attribs.find(name);
	if (attribs_find == enabled_program->attribs.end()) {
		return;
	}
	GLuint attrib_location = attribs_find->second;

	GlSystem::EnableVertexAttribArray(attrib_location);
	GlSystem::BindBuffer(buf->getTarget(), buf->getBufferId());
	GlSystem::VertexAttribPointer(attrib_location, buf->getComponents(), buf->getType(), buf->getNormalized(), 0, NULL);

	used_attribs.insert(attrib_location);

	HppCheckGlErrors();
}

inline void Shaderprogram::setBufferobject(Bufferobject::Shortcut shortcut, Bufferobject const* buf)
{
	HppCheckGlErrors();

	// Find vertex attribute location. If shader does not
	// have an attribute with that name, consider that
	// it wants to discard all information in it.
	HppAssert(shortcut < Bufferobject::SHORTCUT_END, "Invalid attribute shortcut!");
	GLint attrib_location = enabled_program->attribs_by_shortcuts[shortcut];
	if (attrib_location < 0) {
		return;
	}

	GlSystem::EnableVertexAttribArray(attrib_location);
	GlSystem::BindBuffer(buf->getTarget(), buf->getBufferId());
	GlSystem::VertexAttribPointer(attrib_location, buf->getComponents(), buf->getType(), buf->getNormalized(), 0, NULL);

	used_attribs.insert(attrib_location);

	HppCheckGlErrors();
}

inline GLuint Shaderprogram::getGLSLProgram(void) const
{
	HppCheckGlErrors();
	HppAssert(enabled_program, "Not enabled!");
	return enabled_program->prog_id;
}

inline void Shaderprogram::linkProgram(Flags const& flags)
{
	HppAssert(!enabled_program, "Must not be enabled!");
	HppCheckGlErrors();

	LinkedProgram new_lprog;
	for (size_t i = 0; i < Bufferobject::SHORTCUT_END; ++ i) {
		new_lprog.attribs_by_shortcuts[i] = -1;
	}

	// Convert flags to lines of codes that make some defines
	Lines lines_flags;
	for (Flags::const_iterator flags_it = flags.begin();
	     flags_it != flags.end();
	     ++ flags_it) {
	     	std::string const& flag = *flags_it;
	     	lines_flags.push_back("#define " + flag + "\n");
	}

	// Compile shaders
	CompiledShaders compiled_shaders;
	for (Shaders::const_iterator shaders_it = shaders.begin();
	     shaders_it != shaders.end();
	     ++ shaders_it) {
		Shader const& shader = *shaders_it;

		HppCheckGlErrors();

		// Create and load shader
		GLuint shader_id;
		if (shader.getType() == Shader::FRAGMENT_SHADER) {
			shader_id = GlSystem::CreateShader(GL_FRAGMENT_SHADER);
		} else {
			shader_id = GlSystem::CreateShader(GL_VERTEX_SHADER);
		}
		if (shader_id == 0) {
			cleanCompiledShaders(compiled_shaders);
			throw Exception("Unable to create new shader object!");
		}
		// Add it already, so it becomes released in case of error
		compiled_shaders.push_back(shader_id);

		// Get shader source and convert it to lines of code
		Lines lines_src = splitString(shader.getSource(), '\n');

		// Merge code lines from flags and from shader sources.
		// If there is version information in shader sources,
		// then ensure it is at the first line. If there are no
		// flags, then no modifications to the source is made.
		Lines lines;
		if (flags.empty()) {
			lines = lines_src;
		} else {
			// Search for version information
			for (Lines::iterator lines_find = lines_src.begin();
			     lines_find != lines_src.end();
			     ++ lines_find) {
				std::string const& line = *lines_find;
				if (startsWith(trim(line), "#version")) {
					lines.push_back(line);
					lines_src.erase(lines_find);
					break;
				}
			}
			// Add flags
			lines.insert(lines.end(), lines_flags.begin(), lines_flags.end());
			// Add sources
			lines.insert(lines.end(), lines_src.begin(), lines_src.end());
		}

		// Convert lines to string
		std::string shader_src = joinStrings(lines, "\n");

		char const* src_c_str = shader_src.c_str();
		GLint src_size = shader_src.size();
		GlSystem::ShaderSource(shader_id, 1, &src_c_str, &src_size);
		GlSystem::CompileShader(shader_id);
		GLint compile_status;
		GlSystem::GetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
		if (!compile_status) {
			// Get error string
			GLint error_str_len;
			GlSystem::GetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &error_str_len);
			GLchar* error_c_str = new GLchar[error_str_len+1];
			GlSystem::GetShaderInfoLog(shader_id, error_str_len+1, reinterpret_cast< GLsizei* >(&error_str_len), error_c_str);
			// Clean Shaders
			cleanCompiledShaders(compiled_shaders);
			// Throw exception
			std::string error_str(error_c_str, error_str_len);
			delete[] error_c_str;
			throw Exception(std::string("Unable to compile GLSL shader! Reason: ") + error_str);
		}
		HppCheckGlErrors();
	}

	// Create new program
	new_lprog.prog_id = GlSystem::CreateProgram();
	HppCheckGlErrors();
	if (new_lprog.prog_id == 0) {
		cleanCompiledShaders(compiled_shaders);
		throw Exception("Unable to create new program object!");
	}
	HppCheckGlErrors();

	// Attach shaders
	for (CompiledShaders::const_iterator shaders_it = compiled_shaders.begin();
	     shaders_it != compiled_shaders.end();
	     ++ shaders_it) {
		GlSystem::AttachShader(new_lprog.prog_id, *shaders_it);
	}

	// Link program
	GlSystem::LinkProgram(new_lprog.prog_id);
	GLint link_status;
	GlSystem::GetProgramiv(new_lprog.prog_id, GL_LINK_STATUS, &link_status);
	if (!link_status) {
		// Get error string
		GLint error_str_len;
		GlSystem::GetProgramiv(new_lprog.prog_id, GL_INFO_LOG_LENGTH, &error_str_len);
		GLchar* error_c_str = new GLchar[error_str_len+1];
		GlSystem::GetProgramInfoLog(new_lprog.prog_id, error_str_len+1, reinterpret_cast< GLsizei* >(&error_str_len), error_c_str);
		// Clean Shaderprogram
		GlSystem::DeleteProgram(new_lprog.prog_id);
		// Clean Shaders
		cleanCompiledShaders(compiled_shaders);
		// Throw exception
		std::string error_str(error_c_str, error_str_len);
		delete[] error_c_str;
		throw Exception(std::string("Unable to link GLSL shader! Reason: ") + error_str);
	}

	// Clean Shaders
	cleanCompiledShaders(compiled_shaders);

	// Get locations of attributes
	GLint attribs_count;
	GlSystem::GetProgramiv(new_lprog.prog_id, GL_ACTIVE_ATTRIBUTES, &attribs_count);
	size_t const NAMEBUF_MAXLEN = 1024;
	char namebuf[NAMEBUF_MAXLEN];
	for (size_t attrib_id = 0; attrib_id < size_t(attribs_count); ++ attrib_id) {
		GLsizei namebuf_len;
		GLint buf_size;
		GLenum buf_type;
		GlSystem::GetActiveAttrib(new_lprog.prog_id, attrib_id, NAMEBUF_MAXLEN, &namebuf_len, &buf_size, &buf_type, namebuf);
		std::string name(namebuf, namebuf_len);
		GLint attrib_location = GlSystem::GetAttribLocation(new_lprog.prog_id, name.c_str());
		if (attrib_location < 0) {
			std::cerr << "WARNING: Attribute \"" << name << "\" location could not be found, but glGetActiveAttrib() claims it exists!" << std::endl;
			continue;
		}
		new_lprog.attribs[name] = attrib_location;
		// Store shortcuts too
		if (name == "pos") {
			new_lprog.attribs_by_shortcuts[Bufferobject::POS] = attrib_location;
		} else if (name == "normal") {
			new_lprog.attribs_by_shortcuts[Bufferobject::NORMAL] = attrib_location;
		} else if (name == "tangent") {
			new_lprog.attribs_by_shortcuts[Bufferobject::TANGENT] = attrib_location;
		} else if (name == "binormal") {
			new_lprog.attribs_by_shortcuts[Bufferobject::BINORMAL] = attrib_location;
		} else if (name == "uv") {
			new_lprog.attribs_by_shortcuts[Bufferobject::UV] = attrib_location;
		} else if (name == "clr") {
			new_lprog.attribs_by_shortcuts[Bufferobject::CLR] = attrib_location;
		} else if (name == "index") {
			new_lprog.attribs_by_shortcuts[Bufferobject::INDEX] = attrib_location;
		}
	}

	lprogs[flags] = new_lprog;

	HppCheckGlErrors();
}

inline void Shaderprogram::cleanLinkedPrograms(void)
{
	HppAssert(!enabled_program, "Must not be enabled!");
	for (LinkedPrograms::const_iterator lprogs_it = lprogs.begin();
	     lprogs_it != lprogs.end();
	     ++ lprogs_it) {
	     	LinkedProgram const& lprog = lprogs_it->second;
	     	GlSystem::DeleteProgram(lprog.prog_id);
	}
	lprogs.clear();
}

inline void Shaderprogram::cleanCompiledShaders(CompiledShaders& shaders)
{
	for (CompiledShaders::const_iterator shaders_it = shaders.begin();
	     shaders_it != shaders.end();
	     ++ shaders_it) {
		GlSystem::DeleteShader(*shaders_it);
	}
}

}

#endif
