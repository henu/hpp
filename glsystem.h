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
	inline static void AttachShader(GLuint program, GLuint shader);
	inline static void BindAttribLocation(GLuint program, GLuint index, GLchar const* name);
	inline static void BindBuffer(GLenum target, GLuint id);
	inline static void BindVertexArray(GLuint array);
	inline static void BufferData(GLenum target, GLsizei size, const void* data, GLenum usage);
	inline static void ClientActiveTexture(GLenum texture);
	inline static GLuint CreateProgram(void);
	inline static GLuint CreateShader(GLenum type);
	inline static void CompileShader(GLuint shader);
	inline static void DeleteBuffers(GLsizei n, GLuint const* buffers);
	inline static void DeleteProgram(GLuint program);
	inline static void DeleteShader(GLuint shader);
	inline static void DisableVertexAttribArray(GLuint index);
	inline static void EnableVertexAttribArray(GLuint index);
	inline static void GenBuffers(GLsizei n, GLuint* ids);
	inline static void GenVertexArrays(GLsizei n, GLuint* arrays);
	inline static void GetProgramInfoLog(GLuint program, GLsizei max_len, GLsizei* len, GLchar* log);
	inline static void GetShaderInfoLog(GLuint shader, GLsizei max_len, GLsizei* len, GLchar* log);
	inline static void GetProgramiv(GLuint program, GLenum param, GLint* value);
	inline static void GetShaderiv(GLuint shader, GLenum param, GLint* value);
	inline static GLint GetUniformLocation(GLuint program, GLchar const* name);
	inline static void LinkProgram(GLuint program);
	inline static void ShaderSource(GLuint shader, GLuint strs_count, GLchar const** strs, GLint* lens);
	inline static void Uniform1f(GLint location, GLfloat v0);
	inline static void Uniform1fv(GLint location, GLsizei count, GLfloat const* value);
	inline static void Uniform2f(GLint location, GLfloat v0, GLfloat v1);
	inline static void Uniform2fv(GLint location, GLsizei count, GLfloat const* value);
	inline static void Uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	inline static void Uniform3fv(GLint location, GLsizei count, GLfloat const* value);
	inline static void Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	inline static void Uniform4fv(GLint location, GLsizei count, GLfloat const* value);
	inline static void Uniform1i(GLint location, GLint v0);
	inline static void Uniform1iv(GLint location, GLsizei count, GLint const* value);
	inline static void Uniform2i(GLint location, GLint v0, GLint v1);
	inline static void Uniform2iv(GLint location, GLsizei count, GLint const* value);
	inline static void Uniform3i(GLint location, GLint v0, GLint v1, GLint v2);
	inline static void Uniform3iv(GLint location, GLsizei count, GLint const* value);
	inline static void Uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	inline static void Uniform4iv(GLint location, GLsizei count, GLint const* value);
	inline static void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	inline static void UseProgram(GLuint program);
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
	static PFNGLACTIVETEXTUREPROC systemActiveTexture;
	static PFNGLATTACHSHADERPROC systemAttachShader;
	static PFNGLBINDATTRIBLOCATIONPROC systemBindAttribLocation;
	static PFNGLBINDBUFFERPROC systemBindBuffer;
	static PFNGLBINDVERTEXARRAYPROC systemBindVertexArray;
	static PFNGLBUFFERDATAPROC systemBufferData;
	static PFNGLCLIENTACTIVETEXTUREPROC systemClientActiveTexture;
	static PFNGLCOMPILESHADERPROC systemCompileShader;
	static PFNGLCREATEPROGRAMPROC systemCreateProgram;
	static PFNGLCREATESHADERPROC systemCreateShader;
	static PFNGLDELETEBUFFERSPROC systemDeleteBuffers;
	static PFNGLDELETEPROGRAMPROC systemDeleteProgram;
	static PFNGLDELETESHADERPROC systemDeleteShader;
	static PFNGLDISABLEVERTEXATTRIBARRAYPROC systemDisableVertexAttribArray;
	static PFNGLENABLEVERTEXATTRIBARRAYPROC systemEnableVertexAttribArray;
	static PFNGLGENBUFFERSPROC systemGenBuffers;
	static PFNGLGENVERTEXARRAYSPROC systemGenVertexArrays;
	static PFNGLGETPROGRAMINFOLOGPROC systemGetProgramInfoLog;
	static PFNGLGETSHADERINFOLOGPROC systemGetShaderInfoLog;
	static PFNGLGETPROGRAMIVPROC systemGetProgramiv;
	static PFNGLGETSHADERIVPROC systemGetShaderiv;
	static PFNGLGETUNIFORMLOCATIONPROC systemGetUniformLocation;
	static PFNGLLINKPROGRAMPROC systemLinkProgram;
	static PFNGLSHADERSOURCEPROC systemShaderSource;
	static PFNGLUNIFORM1FPROC systemUniform1f;
	static PFNGLUNIFORM1FVPROC systemUniform1fv;
	static PFNGLUNIFORM2FPROC systemUniform2f;
	static PFNGLUNIFORM2FVPROC systemUniform2fv;
	static PFNGLUNIFORM3FPROC systemUniform3f;
	static PFNGLUNIFORM3FVPROC systemUniform3fv;
	static PFNGLUNIFORM4FPROC systemUniform4f;
	static PFNGLUNIFORM4FVPROC systemUniform4fv;
	static PFNGLUNIFORM1IPROC systemUniform1i;
	static PFNGLUNIFORM1IVPROC systemUniform1iv;
	static PFNGLUNIFORM2IPROC systemUniform2i;
	static PFNGLUNIFORM2IVPROC systemUniform2iv;
	static PFNGLUNIFORM3IPROC systemUniform3i;
	static PFNGLUNIFORM3IVPROC systemUniform3iv;
	static PFNGLUNIFORM4IPROC systemUniform4i;
	static PFNGLUNIFORM4IVPROC systemUniform4iv;
	static PFNGLUNIFORMMATRIX2FVPROC systemUniformMatrix2fv;
	static PFNGLUNIFORMMATRIX3FVPROC systemUniformMatrix3fv;
	static PFNGLUNIFORMMATRIX4FVPROC systemUniformMatrix4fv;
	static PFNGLUNIFORMMATRIX2X3FVPROC systemUniformMatrix2x3fv;
	static PFNGLUNIFORMMATRIX3X2FVPROC systemUniformMatrix3x2fv;
	static PFNGLUNIFORMMATRIX2X4FVPROC systemUniformMatrix2x4fv;
	static PFNGLUNIFORMMATRIX4X2FVPROC systemUniformMatrix4x2fv;
	static PFNGLUNIFORMMATRIX3X4FVPROC systemUniformMatrix3x4fv;
	static PFNGLUNIFORMMATRIX4X3FVPROC systemUniformMatrix4x3fv;
	static PFNGLUSEPROGRAMPROC systemUseProgram;
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

inline void GlSystem::BindAttribLocation(GLuint program, GLuint index, GLchar const* name)
{
	HppAssert(systemBindAttribLocation, "Function does not exist!");
	systemBindAttribLocation(program, index, name);
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

inline void GlSystem::AttachShader(GLuint program, GLuint shader)
{
	HppAssert(systemAttachShader, "Function does not exist!");
	systemAttachShader(program, shader);
}

inline GLuint GlSystem::CreateProgram(void)
{
	HppAssert(systemCreateProgram, "Function does not exist!");
	return systemCreateProgram();
}

inline GLuint GlSystem::CreateShader(GLenum type)
{
	HppAssert(systemCreateShader, "Function does not exist!");
	return systemCreateShader(type);
}

inline void GlSystem::CompileShader(GLuint shader)
{
	HppAssert(systemCompileShader, "Function does not exist!");
	systemCompileShader(shader);
}

inline void GlSystem::DeleteBuffers(GLsizei n, GLuint const* buffers)
{
	HppAssert(systemDeleteBuffers, "Function does not exist!");
	systemDeleteBuffers(n, buffers);
}

inline void GlSystem::DeleteProgram(GLuint program)
{
	HppAssert(systemDeleteProgram, "Function does not exist!");
	systemDeleteProgram(program);
}

inline void GlSystem::DeleteShader(GLuint shader)
{
	HppAssert(systemDeleteShader, "Function does not exist!");
	systemDeleteShader(shader);
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

inline void GlSystem::GetProgramInfoLog(GLuint program, GLsizei max_len, GLsizei* len, GLchar* log)
{
	HppAssert(systemGetProgramInfoLog, "Function does not exist!");
	systemGetProgramInfoLog(program, max_len, len, log);
}

inline void GlSystem::GetShaderInfoLog(GLuint shader, GLsizei max_len, GLsizei* len, GLchar* log)
{
	HppAssert(systemGetShaderInfoLog, "Function does not exist!");
	systemGetShaderInfoLog(shader, max_len, len, log);
}

inline void GlSystem::GetProgramiv(GLuint program, GLenum param, GLint* value)
{
	HppAssert(systemGetProgramiv, "Function does not exist!");
	systemGetProgramiv(program, param, value);
}

inline void GlSystem::GetShaderiv(GLuint shader, GLenum param, GLint* value)
{
	HppAssert(systemGetShaderiv, "Function does not exist!");
	systemGetShaderiv(shader, param, value);
}

inline GLint GlSystem::GetUniformLocation(GLuint program, GLchar const* name)
{
	HppAssert(systemGetUniformLocation, "Function does not exist!");
	return systemGetUniformLocation(program, name);
}

inline void GlSystem::LinkProgram(GLuint program)
{
	HppAssert(systemLinkProgram, "Function does not exist!");
	systemLinkProgram(program);
}

inline void GlSystem::ShaderSource(GLuint shader, GLuint strs_count, GLchar const** strs, GLint* lens)
{
	HppAssert(systemShaderSource, "Function does not exist!");
	systemShaderSource(shader, strs_count, strs, lens);
}

inline void GlSystem::Uniform1f(GLint location, GLfloat v0)
{
	HppAssert(systemUniform1f, "Function does not exist!");
	systemUniform1f(location, v0);
}

inline void GlSystem::Uniform1fv(GLint location, GLsizei count, GLfloat const* value)
{
	HppAssert(systemUniform1fv, "Function does not exist!");
	systemUniform1fv(location, count, value);
}

inline void GlSystem::Uniform2f(GLint location, GLfloat v0, GLfloat v1)
{
	HppAssert(systemUniform2f, "Function does not exist!");
	systemUniform2f(location, v0, v1);
}

inline void GlSystem::Uniform2fv(GLint location, GLsizei count, GLfloat const* value)
{
	HppAssert(systemUniform2fv, "Function does not exist!");
	systemUniform2fv(location, count, value);
}

inline void GlSystem::Uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
	HppAssert(systemUniform3f, "Function does not exist!");
	systemUniform3f(location, v0, v1, v2);
}

inline void GlSystem::Uniform3fv(GLint location, GLsizei count, GLfloat const* value)
{
	HppAssert(systemUniform3fv, "Function does not exist!");
	systemUniform3fv(location, count, value);
}

inline void GlSystem::Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	HppAssert(systemUniform4f, "Function does not exist!");
	systemUniform4f(location, v0, v1, v2, v3);
}

inline void GlSystem::Uniform4fv(GLint location, GLsizei count, GLfloat const* value)
{
	HppAssert(systemUniform4fv, "Function does not exist!");
	systemUniform4fv(location, count, value);
}

inline void GlSystem::Uniform1i(GLint location, GLint v0)
{
	HppAssert(systemUniform1i, "Function does not exist!");
	systemUniform1i(location, v0);
}

inline void GlSystem::Uniform1iv(GLint location, GLsizei count, GLint const* value)
{
	HppAssert(systemUniform1iv, "Function does not exist!");
	systemUniform1iv(location, count, value);
}

inline void GlSystem::Uniform2i(GLint location, GLint v0, GLint v1)
{
	HppAssert(systemUniform2i, "Function does not exist!");
	systemUniform2i(location, v0, v1);
}

inline void GlSystem::Uniform2iv(GLint location, GLsizei count, GLint const* value)
{
	HppAssert(systemUniform2iv, "Function does not exist!");
	systemUniform2iv(location, count, value);
}

inline void GlSystem::Uniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
	HppAssert(systemUniform3i, "Function does not exist!");
	systemUniform3i(location, v0, v1, v2);
}

inline void GlSystem::Uniform3iv(GLint location, GLsizei count, GLint const* value)
{
	HppAssert(systemUniform3iv, "Function does not exist!");
	systemUniform3iv(location, count, value);
}

inline void GlSystem::Uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
	HppAssert(systemUniform4i, "Function does not exist!");
	systemUniform4i(location, v0, v1, v2, v3);
}

inline void GlSystem::Uniform4iv(GLint location, GLsizei count, GLint const* value)
{
	HppAssert(systemUniform4iv, "Function does not exist!");
	systemUniform4iv(location, count, value);
}

inline void GlSystem::UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix2fv, "Function does not exist!");
	systemUniformMatrix2fv(location, count, transpose, value);
}

inline void GlSystem::UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix3fv, "Function does not exist!");
	systemUniformMatrix3fv(location, count, transpose, value);
}

inline void GlSystem::UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix4fv, "Function does not exist!");
	systemUniformMatrix4fv(location, count, transpose, value);
}

inline void GlSystem::UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix2x3fv, "Function does not exist!");
	systemUniformMatrix2x3fv(location, count, transpose, value);
}

inline void GlSystem::UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix3x2fv, "Function does not exist!");
	systemUniformMatrix3x2fv(location, count, transpose, value);
}

inline void GlSystem::UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix2x4fv, "Function does not exist!");
	systemUniformMatrix2x4fv(location, count, transpose, value);
}

inline void GlSystem::UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix4x2fv, "Function does not exist!");
	systemUniformMatrix4x2fv(location, count, transpose, value);
}

inline void GlSystem::UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix3x4fv, "Function does not exist!");
	systemUniformMatrix3x4fv(location, count, transpose, value);
}

inline void GlSystem::UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	HppAssert(systemUniformMatrix4x3fv, "Function does not exist!");
	systemUniformMatrix4x3fv(location, count, transpose, value);
}

inline void GlSystem::UseProgram(GLuint program)
{
	HppAssert(systemUseProgram, "Function does not exist!");
	systemUseProgram(program);
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
