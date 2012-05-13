#ifndef HPP_SHADERPROGRAM_H
#define HPP_SHADERPROGRAM_H

#include "gldebug.h"
#include "glsystem.h"
#include "shader.h"
#include "inc_gl.h"
#include "noncopyable.h"

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

	// Returns GLSL Id of enabled program. This may deprecate between
	// enables and disables.
	inline GLhandleARB getGLSLProgram(void) const { HppAssert(enabled, "Not enabled!"); return glsl_id; }

private:

	typedef std::vector< Shader const* > Shaders;

	struct LinkedProgram
	{
		std::vector< GLhandleARB > shaders;
		GLhandleARB program;
	};
	typedef std::map< Flags, LinkedProgram > LinkedPrograms;

	bool enabled;
	GLhandleARB glsl_id;

	Shaders shaders;

	LinkedPrograms lprogs;

	inline void linkProgram(Flags const& flags);

	inline void cleanLinkedPrograms(void);
	inline static void cleanCompiledShaders(std::vector< GLhandleARB > const& shaders);

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
	glsl_id = lprogs[flags].program;
	GlSystem::UseProgramObject(glsl_id);
	HppCheckGlErrors();

	enabled = true;
}

inline void Shaderprogram::disable(void)
{
	HppCheckForCorrectThread();
	HppAssert(enabled, "Not enabled!");
	GlSystem::UseProgramObject(0);
	HppCheckGlErrors();
	enabled = false;
}

inline void Shaderprogram::linkProgram(Flags const& flags)
{
	LinkedProgram new_lprog;

	std::string flags_srcmod;
	for (Flags::const_iterator flags_it = flags.begin();
	     flags_it != flags.end();
	     ++ flags_it) {
	     	std::string const& flag = *flags_it;
	     	flags_srcmod += "#define " + flag + "\n";
	}

	// Compile shaders
	for (Shaders::const_iterator shaders_it = shaders.begin();
	     shaders_it != shaders.end();
	     ++ shaders_it) {
		Shader const* shader = *shaders_it;

		HppCheckGlErrors();

		// Create and load shader
		GLhandleARB shader_id;
		if (shader->getType() == Shader::FRAGMENT_SHADER) {
			shader_id = GlSystem::CreateShaderObject(GL_FRAGMENT_SHADER);
		} else {
			shader_id = GlSystem::CreateShaderObject(GL_VERTEX_SHADER);
		}
		if (shader_id == 0) {
			cleanCompiledShaders(new_lprog.shaders);
			throw Exception("Unable to create new shader object!");
		}
		// Add it already, so it becomes released in case of error
		new_lprog.shaders.push_back(shader_id);

		// Get shader source and do modifications that are requested by flags
		std::string shader_src = flags_srcmod + shader->getSource();

		char const* src_c_str = shader_src.c_str();
		GLint src_size = shader_src.size();
		GlSystem::ShaderSource(shader_id, 1, &src_c_str, &src_size);
		GlSystem::CompileShader(shader_id);
		GLint compile_status;
		GlSystem::GetObjectParameteriv(shader_id, GL_COMPILE_STATUS, &compile_status);
		if (!compile_status) {
			// Get error string
			GLint error_str_len;
			GlSystem::GetObjectParameteriv(shader_id, GL_INFO_LOG_LENGTH, &error_str_len);
			GLchar* error_c_str = new GLchar[error_str_len+1];
			GlSystem::GetInfoLog(shader_id, error_str_len+1, reinterpret_cast< GLsizei* >(&error_str_len), error_c_str);
			// Clean Shaders
			cleanCompiledShaders(new_lprog.shaders);
			// Throw exception
			std::string error_str(error_c_str, error_str_len);
			delete[] error_c_str;
			throw Exception(std::string("Unable to compile GLSL shader! Reason: ") + error_str);
		}
		HppCheckGlErrors();
	}

	// Create new program
	new_lprog.program = GlSystem::CreateProgramObject();
	HppCheckGlErrors();
	if (new_lprog.program == 0) {
		cleanCompiledShaders(new_lprog.shaders);
		throw Exception("Unable to create new program object!");
	}
	HppCheckGlErrors();

	// Attach shaders
	for (std::vector< GLhandleARB >::const_iterator shaders_it = new_lprog.shaders.begin();
	     shaders_it != new_lprog.shaders.end();
	     ++ shaders_it) {
		GlSystem::AttachObject(new_lprog.program, *shaders_it);
	}

	// Link program
	GlSystem::LinkProgram(new_lprog.program);
	GLint link_status;
	GlSystem::GetObjectParameteriv(new_lprog.program, GL_LINK_STATUS, &link_status);
	if (!link_status) {
		// Get error string
		GLint error_str_len;
		GlSystem::GetObjectParameteriv(new_lprog.program, GL_INFO_LOG_LENGTH, &error_str_len);
		GLchar* error_c_str = new GLchar[error_str_len+1];
		GlSystem::GetInfoLog(new_lprog.program, error_str_len+1, reinterpret_cast< GLsizei* >(&error_str_len), error_c_str);
		// Clean Shaderprogram
		GlSystem::DeleteObject(new_lprog.program);
		// Clean Shaders
		cleanCompiledShaders(new_lprog.shaders);
		// Throw exception
		std::string error_str(error_c_str, error_str_len);
		delete[] error_c_str;
		throw Exception(std::string("Unable to link GLSL shader! Reason: ") + error_str);
	}

	lprogs[flags] = new_lprog;

}

inline void Shaderprogram::cleanLinkedPrograms(void)
{
	HppAssert(!enabled, "Must not be enabled!");
	for (LinkedPrograms::iterator lprogs_it = lprogs.begin();
	     lprogs_it != lprogs.end();
	     ++ lprogs_it) {
	     	LinkedProgram& lprog = lprogs_it->second;
	     	GlSystem::DeleteObject(lprog.program);
	     	cleanCompiledShaders(lprog.shaders);
	}
	lprogs.clear();
}

inline void Shaderprogram::cleanCompiledShaders(std::vector< GLhandleARB > const& shaders)
{
	for (std::vector< GLhandleARB >::const_iterator shaders_it = shaders.begin();
	     shaders_it != shaders.end();
	     ++ shaders_it) {
		GlSystem::DeleteObject(*shaders_it);
	}
}

}

#endif
