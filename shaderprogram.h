#ifndef HPP_SHADERPROGRAM_H
#define HPP_SHADERPROGRAM_H

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
	inline void setUniform(Matrix3 const& mat, std::string const& name, bool transpose = false);
	inline void setUniform(Matrix4 const& mat, std::string const& name, bool transpose = false);

	// Returns GLSL Id of enabled program. This may deprecate between
	// enables and disables.
	inline GLuint getGLSLProgram(void) const { HppAssert(enabled, "Not enabled!"); return glsl_id; }

private:

	typedef std::vector< Shader const* > Shaders;

	typedef std::map< Flags, GLuint > LinkedPrograms;

	typedef std::vector< GLuint > CompiledShaders;

	bool enabled;
	GLuint glsl_id;

	Shaders shaders;

	LinkedPrograms lprogs;

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

	enabled = true;
}

inline void Shaderprogram::disable(void)
{
	HppCheckForCorrectThread();
	HppAssert(enabled, "Not enabled!");
	GlSystem::UseProgram(0);
	HppCheckGlErrors();
	enabled = false;
}

inline void Shaderprogram::setUniform(Matrix3 const& mat, std::string const& name, bool transpose)
{
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = Hpp::GlSystem::GetUniformLocation(glsl_id, name.c_str());
	if (transpose) {
		Hpp::GlSystem::UniformMatrix3fv(uniform_id, 1, GL_TRUE, mat.getCells());
	} else {
		Hpp::GlSystem::UniformMatrix3fv(uniform_id, 1, GL_FALSE, mat.getCells());
	}
}

inline void Shaderprogram::setUniform(Matrix4 const& mat, std::string const& name, bool transpose)
{
	HppAssert(enabled, "Not enabled!");
	GLuint uniform_id = Hpp::GlSystem::GetUniformLocation(glsl_id, name.c_str());
	Hpp::GlSystem::UniformMatrix4fv(uniform_id, 1, transpose, mat.getCells());
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
