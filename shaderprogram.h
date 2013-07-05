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
#include "types.h"

#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>

namespace Hpp
{

class Shaderprogramhandle;

class Shaderprogram : private NonCopyable
{

	friend class Shaderprogramhandle;

public:

	typedef std::set< std::string > Flags;

	// Constructor and destructor
	inline Shaderprogram(Strings const& uniforms = Strings(), Strings const& vertexattrs = Strings());
	inline ~Shaderprogram(void);

	inline void addUniformnames(Strings const& uniforms);
	inline void addVertexattributenames(Strings const& vertexattrs);

	// Attachs shaders to program
	inline void attachShader(Shader const& shader);

	// Creates handle to this shaderprogram with specific flags and names
	// of uniforms and vertexattributes in specific numeric shortcuts.
// TODO: In future, make it so that program keeps handles of all flags and user must not destroy them!
	Shaderprogramhandle* createHandle(Flags const& flags);

private:

	// ========================================
	// Types and functions for friends
	// ========================================

	struct LinkedProgram
	{
		GLuint prog_id;
		GLint attribs_by_shortcuts[Bufferobject::SHORTCUT_END];
	};

	// Shaderprogramhandles use these functions to
	// inform when they are enabled or disabled.
	inline void handleEnabled(void);
	inline void handleDisabled(void);

	// Adds Vertexattributes that are used. This way we know
	// what should be disabled when shader is disabled.
	inline void addUsedVertexattribute(GLuint vertexattribute_loc);

private:

	typedef std::vector< Shader > Shaders;

	typedef std::map< Flags, LinkedProgram > LinkedPrograms;

	typedef std::vector< GLuint > CompiledShaders;

	typedef std::vector< std::string > Lines;

	typedef std::set< GLuint > AttribsInUse;

	// These contain names of uniforms and vertexattributes. Their
	// locations in linked shaders will be the same (actually locations
	// of uniforms are defined by OpenGL, but we do a conversion table
	// so it looks like they are always in same location)
	Strings uniforms;
	Strings vertexattrs;

	// Shaderprogramhandle that is currently
	// enabled or NULL if nothing is enabled.
	bool handle_enabled;

	Shaders shaders;

	LinkedPrograms lprogs;

	AttribsInUse used_attribs;

	inline void linkProgram(Flags const& flags);

	inline void cleanLinkedPrograms(void);
	inline static void cleanCompiledShaders(CompiledShaders& shaders);

};

inline Shaderprogram::Shaderprogram(Strings const& uniforms, Strings const& vertexattrs) :
uniforms(uniforms),
vertexattrs(vertexattrs),
handle_enabled(false)
{
	HppCheckForCorrectThread();
}

inline Shaderprogram::~Shaderprogram(void)
{
	HppCheckForCorrectThread();
	cleanLinkedPrograms();
}

inline void Shaderprogram::addUniformnames(Strings const& uniforms)
{
	this->uniforms.insert(this->uniforms.end(), uniforms.begin(), uniforms.end());
}

inline void Shaderprogram::addVertexattributenames(Strings const& vertexattrs)
{
	this->vertexattrs.insert(this->vertexattrs.end(), vertexattrs.begin(), vertexattrs.end());
}

inline void Shaderprogram::attachShader(Shader const& shader)
{
	HppCheckForCorrectThread();
	cleanLinkedPrograms();
	shaders.push_back(shader);
}

inline void Shaderprogram::handleEnabled(void)
{
	HppAssert(!handle_enabled, "There is already a Shaderprogramhandle enabled!");
	HppAssert(used_attribs.empty(), "There are already some Bufferobjects bound!");
	handle_enabled = true;
}

inline void Shaderprogram::handleDisabled(void)
{
	HppAssert(handle_enabled, "There is no Shaderprogramhandle enabled!");
	handle_enabled = false;

	// Disable bufferobjects
	for (AttribsInUse::iterator used_attribs_it = used_attribs.begin();
	     used_attribs_it != used_attribs.end();
	     ++ used_attribs_it) {
		GlSystem::DisableVertexAttribArray(*used_attribs_it);
	}
	used_attribs.clear();
}

inline void Shaderprogram::addUsedVertexattribute(GLuint vertexattribute_loc)
{
	used_attribs.insert(vertexattribute_loc);
}

inline void Shaderprogram::linkProgram(Flags const& flags)
{
	HppAssert(!handle_enabled, "Must not be enabled!");
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

	// Set vertexattribute bindings
	for (size_t vertexattr_loc = 0;
	     vertexattr_loc < vertexattrs.size();
	     ++ vertexattr_loc) {
		GlSystem::BindAttribLocation(new_lprog.prog_id, vertexattr_loc, vertexattrs[vertexattr_loc].c_str());
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
	HppAssert(!handle_enabled, "Must not be enabled!");
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
