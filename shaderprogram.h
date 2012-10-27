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
	inline void attachShader(Shader const* shader);

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

	// Returns GLSL Id of enabled program. This may deprecate between
	// enables and disables.
	inline GLuint getGLSLProgram(void) const { HppAssert(enabled, "Not enabled!"); return glsl_id; }

private:

	typedef std::vector< Shader const* > Shaders;

	typedef std::map< Flags, GLuint > LinkedPrograms;

	typedef std::vector< GLuint > CompiledShaders;

	// Index is same as vertex attribute index. If buf is NULL, then
	// it means there is no buffer and vertex attribute is disabled.
	struct BoundBufferobject
	{
		std::string name;
		Bufferobject const* buf;
	};
	typedef std::vector< BoundBufferobject > BoundBufferobjects;

	bool enabled;
	GLuint glsl_id;

	Shaders shaders;

	LinkedPrograms lprogs;

	BoundBufferobjects bbufs;

	inline void linkProgram(Flags const& flags);

	inline void cleanLinkedPrograms(void);
	inline static void cleanCompiledShaders(CompiledShaders& shaders);

};

inline Shaderprogram::Shaderprogram(void) :
enabled(false)
{
// TODO: Code support for threads!
HppCheckForCorrectThread();
}

inline Shaderprogram::~Shaderprogram(void)
{
// TODO: Code support for threads!
HppCheckForCorrectThread();
	cleanLinkedPrograms();
}

inline void Shaderprogram::attachShader(Shader const* shader)
{
// TODO: Code support for threads!
HppCheckForCorrectThread();
	cleanLinkedPrograms();
	shaders.push_back(shader);
}

inline void Shaderprogram::enable(Flags const& flags)
{
	HppCheckForCorrectThread();
// TODO: Ensure only one shaderprogram is enabled!
	HppAssert(!enabled, "Already enabled!");

	// Ensure program is linked with these flags
	if (lprogs.find(flags) == lprogs.end()) {
		linkProgram(flags);
	}

	HppAssert(lprogs.find(flags) != lprogs.end(), "Fail!");
	glsl_id = lprogs[flags];
	GlSystem::UseProgram(glsl_id);
	HppCheckGlErrors();

	HppAssert(bbufs.empty(), "There are already bound Bufferobjects!");

	enabled = true;
}

inline void Shaderprogram::disable(void)
{
	HppCheckForCorrectThread();
	HppAssert(enabled, "Not enabled!");
	HppCheckGlErrors();
	GlSystem::UseProgram(0);
	HppCheckGlErrors();

	// Disable bufferobjects
	for (BoundBufferobjects::iterator bbufs_it = bbufs.begin();
	     bbufs_it != bbufs.end();
	     ++ bbufs_it) {
		BoundBufferobject& bbuf = *bbufs_it;
		if (bbuf.buf) {
			GlSystem::DisableVertexAttribArray(bbufs_it - bbufs.begin());
		}
	}
	bbufs.clear();

	enabled = false;
}

inline void Shaderprogram::setUniform(std::string const& name, Matrix3 const& mat, bool transpose)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::UniformMatrix3fv(uniform_id, 1, transpose, mat.getCells());
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform(std::string const& name, Matrix4 const& mat, bool transpose)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::UniformMatrix4fv(uniform_id, 1, transpose, mat.getCells());
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform(std::string const& name, Vector3 const& v)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform3f(uniform_id, v.x, v.y, v.z);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform(std::string const& name, Vector3 const& v, Real w)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform4f(uniform_id, v.x, v.y, v.z, w);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform(std::string const& name, Color const& color, Pixelformat force_format)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
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
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform1f(uniform_id, f0);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform2f(std::string const& name, GLfloat f0, GLfloat f1)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform2f(uniform_id, f0, f1);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform3f(std::string const& name, GLfloat f0, GLfloat f1, GLfloat f2)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform3f(uniform_id, f0, f1, f2);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform4f(std::string const& name, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform4f(uniform_id, f0, f1, f2, f3);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform1i(std::string const& name, GLint i0)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform1i(uniform_id, i0);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform2i(std::string const& name, GLint i0, GLint i1)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform2i(uniform_id, i0, i1);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform3i(std::string const& name, GLint i0, GLint i1, GLint i2)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform3i(uniform_id, i0, i1, i2);
	HppCheckGlErrors();
}

inline void Shaderprogram::setUniform4i(std::string const& name, GLint i0, GLint i1, GLint i2, GLint i3)
{
	HppCheckGlErrors();
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = GlSystem::GetUniformLocation(glsl_id, name.c_str());
	GlSystem::Uniform4i(uniform_id, i0, i1, i2, i3);
	HppCheckGlErrors();
}

inline void Shaderprogram::setBufferobject(std::string const& name, Bufferobject const* buf)
{
	// First check if buffer with same name is already bound
	ssize_t vertexattrib_index = -1;
	for (BoundBufferobjects::iterator bbufs_it = bbufs.begin();
	     bbufs_it != bbufs.end();
	     ++ bbufs_it) {
		BoundBufferobject& bbuf = *bbufs_it;
		if (bbuf.buf && bbuf.name == name) {
			vertexattrib_index = bbufs_it - bbufs.begin();
			break;
		}
	}

	// If old buffer was not found, then new one needs to be enabled
	if (vertexattrib_index < 0) {
		vertexattrib_index = 0;
		while (vertexattrib_index < ssize_t(bbufs.size()) && bbufs[vertexattrib_index].buf) {
			++ vertexattrib_index;
		}
		// If no empty slot was found, then add new one
		if (vertexattrib_index == ssize_t(bbufs.size())) {
			bbufs.push_back(BoundBufferobject());
		}
		BoundBufferobject& bbuf = bbufs[vertexattrib_index];
		bbuf.name = name;
		bbuf.buf = NULL;
		// New vertexarray attribute needs to be enabled
		if (vertexattrib_index >= GlSystem::getMaxNumOfVertexattributes()) {
			throw Exception("Too many bufferobjects bound!");
		}
		GlSystem::EnableVertexAttribArray(vertexattrib_index);
	}

	// Set Bufferobject
	bbufs[vertexattrib_index].buf = buf;
	GlSystem::BindAttribLocation(glsl_id, vertexattrib_index, name.c_str());
	GlSystem::BindBuffer(buf->getTarget(), buf->getBufferId());
	GlSystem::VertexAttribPointer(vertexattrib_index, buf->getComponents(), buf->getType(), buf->getNormalized(), 0, NULL);

}

inline void Shaderprogram::linkProgram(Flags const& flags)
{
	GLuint new_lprog;

	std::string flags_srcmod;
	for (Flags::const_iterator flags_it = flags.begin();
	     flags_it != flags.end();
	     ++ flags_it) {
	     	std::string const& flag = *flags_it;
	     	flags_srcmod += "#define " + flag + "\n";
	}

	// Compile shaders
	CompiledShaders compiled_shaders;
	for (Shaders::const_iterator shaders_it = shaders.begin();
	     shaders_it != shaders.end();
	     ++ shaders_it) {
		Shader const* shader = *shaders_it;

		HppCheckGlErrors();

		// Create and load shader
		GLuint shader_id;
		if (shader->getType() == Shader::FRAGMENT_SHADER) {
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

		// Get shader source and do modifications that are requested by flags
		std::string shader_src = flags_srcmod + shader->getSource();

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
	new_lprog = GlSystem::CreateProgram();
	HppCheckGlErrors();
	if (new_lprog == 0) {
		cleanCompiledShaders(compiled_shaders);
		throw Exception("Unable to create new program object!");
	}
	HppCheckGlErrors();

	// Attach shaders
	for (CompiledShaders::const_iterator shaders_it = compiled_shaders.begin();
	     shaders_it != compiled_shaders.end();
	     ++ shaders_it) {
		GlSystem::AttachShader(new_lprog, *shaders_it);
	}

	// Link program
	GlSystem::LinkProgram(new_lprog);
	GLint link_status;
	GlSystem::GetProgramiv(new_lprog, GL_LINK_STATUS, &link_status);
	if (!link_status) {
		// Get error string
		GLint error_str_len;
		GlSystem::GetProgramiv(new_lprog, GL_INFO_LOG_LENGTH, &error_str_len);
		GLchar* error_c_str = new GLchar[error_str_len+1];
		GlSystem::GetProgramInfoLog(new_lprog, error_str_len+1, reinterpret_cast< GLsizei* >(&error_str_len), error_c_str);
		// Clean Shaderprogram
		GlSystem::DeleteProgram(new_lprog);
		// Clean Shaders
		cleanCompiledShaders(compiled_shaders);
		// Throw exception
		std::string error_str(error_c_str, error_str_len);
		delete[] error_c_str;
		throw Exception(std::string("Unable to link GLSL shader! Reason: ") + error_str);
	}

	// Clean Shaders
	cleanCompiledShaders(compiled_shaders);

	lprogs[flags] = new_lprog;

}

inline void Shaderprogram::cleanLinkedPrograms(void)
{
	HppAssert(!enabled, "Must not be enabled!");
	for (LinkedPrograms::iterator lprogs_it = lprogs.begin();
	     lprogs_it != lprogs.end();
	     ++ lprogs_it) {
	     	GLuint lprog = lprogs_it->second;
	     	GlSystem::DeleteProgram(lprog);
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
