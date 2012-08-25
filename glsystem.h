#ifndef HPP_GLSYSTEM_H
#define HPP_GLSYSTEM_H

#include "inc_gl.h"
#include "assert.h"

#include <set>


namespace Hpp
{

class GlSystem
{

	// Friends
	friend class Display;

public:

	// Functions related to extensions. These are got automatically. You
	// have to ensure the correct extension exists!
	inline static void ActiveTexture(GLenum texture);
	inline static void AttachObject(GLhandleARB program, GLhandleARB shader);
	inline static void BindBuffer(GLenum target, GLuint id);
	inline static void BindVertexArray(GLuint array);
	inline static void BufferData(GLenum target, GLsizei size, const void* data, GLenum usage);
	inline static void ClientActiveTexture(GLenum texture);
	inline static GLhandleARB CreateProgramObject(void);
	inline static GLhandleARB CreateShaderObject(GLenum type);
	inline static void CompileShader(GLhandleARB shader);
	inline static void DeleteBuffers(GLsizei n, GLuint const* buffers);
	inline static void DeleteObject(GLhandleARB shader);
	inline static void DisableVertexAttribArray(GLuint index);
	inline static void EnableVertexAttribArray(GLuint index);
	inline static void GenBuffers(GLsizei n, GLuint* ids);
	inline static void GenVertexArrays(GLsizei n, GLuint* arrays);
	inline static void GetInfoLog(GLhandleARB shader, GLsizei max_len, GLsizei* len, GLcharARB* log);
	inline static void GetObjectParameteriv(GLhandleARB shader, GLenum param, GLint* value);
	inline static GLint GetUniformLocation(GLhandleARB program, GLchar const* name);
	inline static void LinkProgram(GLhandleARB program);
	inline static void ShaderSource(GLhandleARB shader, GLuint strs_count, GLcharARB const** strs, GLint* lens);
	inline static void Uniform1i(GLint location, GLint v0);
	inline static void Uniform2i(GLint location, GLint v0, GLint v1);
	inline static void Uniform3i(GLint location, GLint v0, GLint v1, GLint v2);
	inline static void Uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	inline static void Uniform1f(GLint location, GLfloat v0);
	inline static void Uniform2f(GLint location, GLfloat v0, GLfloat v1);
	inline static void Uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	inline static void Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	inline static void UseProgramObject(GLhandleARB program);
	inline static void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);

	// Get functions
	inline static GLint GetInteger(GLenum pname);

	// Functions to get GL properties
	inline static GLint getNumOfTextureunits(void);
	inline static GLint getMaxNumOfLights(void);

	inline static bool extensionExists(std::string const& ext);

	inline static bool openglVersionAtLeast(size_t big_part, size_t small_part);

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	// Initializes system. This means that for example pointers to
	// functions of extensions are got. This is called by Display.
	static void initialize(void);

private:

	// Static pointers to functions of extensions
	static PFNGLACTIVETEXTUREARBPROC systemActiveTexture;
	static PFNGLATTACHOBJECTARBPROC systemAttachObject;
	static PFNGLBINDBUFFERARBPROC systemBindBuffer;
	static PFNGLBINDVERTEXARRAYPROC systemBindVertexArray;
	static PFNGLBUFFERDATAARBPROC systemBufferData;
	static PFNGLCLIENTACTIVETEXTUREARBPROC systemClientActiveTexture;
	static PFNGLCOMPILESHADERARBPROC systemCompileShader;
	static PFNGLCREATEPROGRAMOBJECTARBPROC systemCreateProgramObject;
	static PFNGLCREATESHADEROBJECTARBPROC systemCreateShaderObject;
	static PFNGLDELETEBUFFERSARBPROC systemDeleteBuffers;
	static PFNGLDELETEOBJECTARBPROC systemDeleteObject;
	static PFNGLDISABLEVERTEXATTRIBARRAYPROC systemDisableVertexAttribArray;
	static PFNGLENABLEVERTEXATTRIBARRAYPROC systemEnableVertexAttribArray;
	static PFNGLGENBUFFERSARBPROC systemGenBuffers;
	static PFNGLGENVERTEXARRAYSPROC systemGenVertexArrays;
	static PFNGLGETINFOLOGARBPROC systemGetInfoLog;
	static PFNGLGETOBJECTPARAMETERIVARBPROC systemGetObjectParameteriv;
	static PFNGLGETUNIFORMLOCATIONARBPROC systemGetUniformLocation;
	static PFNGLLINKPROGRAMARBPROC systemLinkProgram;
	static PFNGLSHADERSOURCEARBPROC systemShaderSource;
	static PFNGLUNIFORM1IARBPROC systemUniform1i;
	static PFNGLUNIFORM2IARBPROC systemUniform2i;
	static PFNGLUNIFORM3IARBPROC systemUniform3i;
	static PFNGLUNIFORM4IARBPROC systemUniform4i;
	static PFNGLUNIFORM1FARBPROC systemUniform1f;
	static PFNGLUNIFORM2FARBPROC systemUniform2f;
	static PFNGLUNIFORM3FARBPROC systemUniform3f;
	static PFNGLUNIFORM4FARBPROC systemUniform4f;
	static PFNGLUSEPROGRAMOBJECTARBPROC systemUseProgramObject;
	static PFNGLVERTEXATTRIBPOINTERPROC systemVertexAttribPointer;

	// Static values of various GL properties
	static GLint tunit_count;
	static GLint max_lights;

	// OpenGL Version
	static size_t opengl_version_big;
	static size_t opengl_version_small;

	// Constructor and destructor in private. No instances can be made from
	// this class
	GlSystem(void);
	~GlSystem(void);

};


inline void GlSystem::ActiveTexture(GLenum texture)
{
	HppAssert(systemActiveTexture, "Function does not exist!");
	systemActiveTexture(texture);
}

inline void GlSystem::BindBuffer(GLenum target, GLuint id)
{
	HppAssert(systemBindBuffer, "Function does not exist!");
	systemBindBuffer(target, id);
}

inline void GlSystem::BindVertexArray(GLuint array)
{
	HppAssert(systemBindVertexArray, "Function does not exist!");
	systemBindVertexArray(array);
}

inline void GlSystem::BufferData(GLenum target, GLsizei size, const void* data, GLenum usage)
{
	HppAssert(systemBufferData, "Function does not exist!");
	systemBufferData(target, size, data, usage);
}

inline void GlSystem::ClientActiveTexture(GLenum texture)
{
	HppAssert(systemClientActiveTexture, "Function does not exist!");
	systemClientActiveTexture(texture);
}

inline void GlSystem::AttachObject(GLhandleARB program, GLhandleARB shader)
{
	HppAssert(systemAttachObject, "Function does not exist!");
	systemAttachObject(program, shader);
}

inline GLhandleARB GlSystem::CreateProgramObject(void)
{
	HppAssert(systemCreateProgramObject, "Function does not exist!");
	return systemCreateProgramObject();
}

inline GLhandleARB GlSystem::CreateShaderObject(GLenum type)
{
	HppAssert(systemCreateShaderObject, "Function does not exist!");
	return systemCreateShaderObject(type);
}

inline void GlSystem::CompileShader(GLhandleARB shader)
{
	HppAssert(systemCompileShader, "Function does not exist!");
	systemCompileShader(shader);
}

inline void GlSystem::DeleteBuffers(GLsizei n, GLuint const* buffers)
{
	HppAssert(systemDeleteBuffers, "Function does not exist!");
	systemDeleteBuffers(n, buffers);
}

inline void GlSystem::DeleteObject(GLhandleARB shader)
{
	HppAssert(systemDeleteObject, "Function does not exist!");
	systemDeleteObject(shader);
}

inline void GlSystem::DisableVertexAttribArray(GLuint index)
{
	HppAssert(systemDisableVertexAttribArray, "Function does not exist!");
	systemDisableVertexAttribArray(index);
}

inline void GlSystem::EnableVertexAttribArray(GLuint index)
{
	HppAssert(systemEnableVertexAttribArray, "Function does not exist!");
	systemEnableVertexAttribArray(index);
}

inline void GlSystem::GenBuffers(GLsizei n, GLuint* ids)
{
	HppAssert(systemGenBuffers, "Function does not exist!");
	systemGenBuffers(n, ids);
}

inline void GlSystem::GenVertexArrays(GLsizei n, GLuint* arrays)
{
	HppAssert(systemGenVertexArrays, "Function does not exist!");
	systemGenVertexArrays(n, arrays);
}

inline void GlSystem::GetInfoLog(GLhandleARB shader, GLsizei max_len, GLsizei* len, GLcharARB* log)
{
	HppAssert(systemGetInfoLog, "Function does not exist!");
	systemGetInfoLog(shader, max_len, len, log);
}

inline void GlSystem::GetObjectParameteriv(GLhandleARB shader, GLenum param, GLint* value)
{
	HppAssert(systemGetObjectParameteriv, "Function does not exist!");
	systemGetObjectParameteriv(shader, param, value);
}

inline GLint GlSystem::GetUniformLocation(GLhandleARB program, GLchar const* name)
{
	HppAssert(systemGetUniformLocation, "Function does not exist!");
	return systemGetUniformLocation(program, name);
}

inline void GlSystem::LinkProgram(GLhandleARB program)
{
	HppAssert(systemLinkProgram, "Function does not exist!");
	systemLinkProgram(program);
}

inline void GlSystem::ShaderSource(GLhandleARB shader, GLuint strs_count, GLcharARB const** strs, GLint* lens)
{
	HppAssert(systemShaderSource, "Function does not exist!");
	systemShaderSource(shader, strs_count, strs, lens);
}

inline void GlSystem::Uniform1i(GLint location, GLint v0)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform1i(location, v0);
}

inline void GlSystem::Uniform2i(GLint location, GLint v0, GLint v1)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform2i(location, v0, v1);
}

inline void GlSystem::Uniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform3i(location, v0, v1, v2);
}

inline void GlSystem::Uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform4i(location, v0, v1, v2, v3);
}

inline void GlSystem::Uniform1f(GLint location, GLfloat v0)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform1f(location, v0);
}

inline void GlSystem::Uniform2f(GLint location, GLfloat v0, GLfloat v1)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform2f(location, v0, v1);
}

inline void GlSystem::Uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform3f(location, v0, v1, v2);
}

inline void GlSystem::Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform4f(location, v0, v1, v2, v3);
}

inline void GlSystem::UseProgramObject(GLhandleARB program)
{
	HppAssert(systemUseProgramObject, "Function does not exist!");
	systemUseProgramObject(program);
}

inline void GlSystem::VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
	HppAssert(systemVertexAttribPointer, "Function does not exist!");
	systemVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

inline GLint GlSystem::GetInteger(GLenum pname)
{
	GLint result;
	glGetIntegerv(pname, &result);
	return result;
}

inline GLint GlSystem::getNumOfTextureunits(void)
{
	HppAssert(tunit_count > 0, "");
	return tunit_count;
}

inline GLint GlSystem::getMaxNumOfLights(void)
{
	HppAssert(max_lights > 0, "");
	return max_lights;
}

inline bool GlSystem::extensionExists(std::string const& ext)
{
	static std::set< std::string > exts;
	static bool exts_got = false;

	if (!exts_got) {
		char const* exts_c_str = reinterpret_cast< char const* >(glGetString(GL_EXTENSIONS));
		std::string ext2;
		char const* it = exts_c_str;
		while (*it != 0) {
			if (*it != ' ') {
				ext2 += *it;
			} else {
				if (!ext2.empty()) {
					exts.insert(ext2);
				}
				ext2.clear();
			}
			it ++;
		}
		if (!ext2.empty()) {
			exts.insert(ext2);
		}
		exts_got = true;
	}

	return exts.find(ext) != exts.end();
}

inline bool GlSystem::openglVersionAtLeast(size_t big_part, size_t small_part)
{
	if (opengl_version_big < big_part) return false;
	if (opengl_version_big > big_part) return true;
	if (opengl_version_small >= small_part) return true;
	return false;
}

}

#endif
