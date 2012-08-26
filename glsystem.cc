#include "glsystem.h"

#include "gldebug.h"
#include "cast.h"
#include "inc_glx.h"
#include "exception.h"

#ifdef WIN32
#include <windows.h>
#endif

namespace Hpp
{

PFNGLACTIVETEXTUREARBPROC GlSystem::systemActiveTexture = NULL;
PFNGLATTACHOBJECTARBPROC GlSystem::systemAttachObject = NULL;
PFNGLBINDBUFFERARBPROC GlSystem::systemBindBuffer = NULL;
PFNGLBINDVERTEXARRAYPROC GlSystem::systemBindVertexArray = NULL;
PFNGLBUFFERDATAARBPROC GlSystem::systemBufferData = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC GlSystem::systemClientActiveTexture = NULL;
PFNGLCOMPILESHADERARBPROC GlSystem::systemCompileShader = NULL;
PFNGLCREATEPROGRAMPROC GlSystem::systemCreateProgram = NULL;
PFNGLCREATESHADERPROC GlSystem::systemCreateShader = NULL;
PFNGLGENBUFFERSARBPROC GlSystem::systemGenBuffers = NULL;
PFNGLGENVERTEXARRAYSPROC GlSystem::systemGenVertexArrays = NULL;
PFNGLGETINFOLOGARBPROC GlSystem::systemGetInfoLog = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC GlSystem::systemGetObjectParameteriv = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC GlSystem::systemGetUniformLocation = NULL;
PFNGLDELETEBUFFERSARBPROC GlSystem::systemDeleteBuffers = NULL;
PFNGLDELETEPROGRAMPROC GlSystem::systemDeleteProgram = NULL;
PFNGLDELETESHADERPROC GlSystem::systemDeleteShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC GlSystem::systemDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC GlSystem::systemEnableVertexAttribArray = NULL;
PFNGLLINKPROGRAMARBPROC GlSystem::systemLinkProgram = NULL;
PFNGLSHADERSOURCEARBPROC GlSystem::systemShaderSource = NULL;
PFNGLUNIFORM1FARBPROC GlSystem::systemUniform1f = NULL;
PFNGLUNIFORM1FVARBPROC GlSystem::systemUniform1fv = NULL;
PFNGLUNIFORM2FARBPROC GlSystem::systemUniform2f = NULL;
PFNGLUNIFORM2FVARBPROC GlSystem::systemUniform2fv = NULL;
PFNGLUNIFORM3FARBPROC GlSystem::systemUniform3f = NULL;
PFNGLUNIFORM3FVARBPROC GlSystem::systemUniform3fv = NULL;
PFNGLUNIFORM4FARBPROC GlSystem::systemUniform4f = NULL;
PFNGLUNIFORM4FVARBPROC GlSystem::systemUniform4fv = NULL;
PFNGLUNIFORM1IARBPROC GlSystem::systemUniform1i = NULL;
PFNGLUNIFORM1IVARBPROC GlSystem::systemUniform1iv = NULL;
PFNGLUNIFORM2IARBPROC GlSystem::systemUniform2i = NULL;
PFNGLUNIFORM2IVARBPROC GlSystem::systemUniform2iv = NULL;
PFNGLUNIFORM3IARBPROC GlSystem::systemUniform3i = NULL;
PFNGLUNIFORM3IVARBPROC GlSystem::systemUniform3iv = NULL;
PFNGLUNIFORM4IARBPROC GlSystem::systemUniform4i = NULL;
PFNGLUNIFORM4IVARBPROC GlSystem::systemUniform4iv = NULL;
PFNGLUSEPROGRAMPROC GlSystem::systemUseProgram = NULL;
PFNGLVERTEXATTRIBPOINTERPROC GlSystem::systemVertexAttribPointer = NULL;

GLint GlSystem::tunit_count = 0;
GLint GlSystem::max_lights = 0;
size_t GlSystem::opengl_version_big;
size_t GlSystem::opengl_version_small;

void GlSystem::initialize(void)
{

	HppCheckGlErrors();

	// Get pointers to functions of extensions
	#ifndef WIN32
	systemActiveTexture = reinterpret_cast< PFNGLACTIVETEXTUREARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glActiveTextureARB")));
	systemAttachObject = reinterpret_cast< PFNGLATTACHOBJECTARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glAttachObjectARB")));
	systemBindBuffer = reinterpret_cast< PFNGLBINDBUFFERARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBindBufferARB")));
	systemBindVertexArray = reinterpret_cast< PFNGLBINDVERTEXARRAYPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBindVertexArray")));
	systemBufferData = reinterpret_cast< PFNGLBUFFERDATAARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBufferDataARB")));
	systemClientActiveTexture = reinterpret_cast< PFNGLCLIENTACTIVETEXTUREARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glClientActiveTextureARB")));
	systemCompileShader = reinterpret_cast< PFNGLCOMPILESHADERARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCompileShaderARB")));
	systemCreateProgram = reinterpret_cast< PFNGLCREATEPROGRAMPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCreateProgram")));
	systemCreateShader = reinterpret_cast< PFNGLCREATESHADERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCreateShader")));
	systemDeleteBuffers = reinterpret_cast< PFNGLDELETEBUFFERSARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteBuffersARB")));
	systemDeleteProgram = reinterpret_cast< PFNGLDELETEPROGRAMPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteProgram")));
	systemDeleteShader = reinterpret_cast< PFNGLDELETESHADERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteShader")));
	systemDisableVertexAttribArray = reinterpret_cast< PFNGLDISABLEVERTEXATTRIBARRAYPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDisableVertexAttribArray")));
	systemEnableVertexAttribArray = reinterpret_cast< PFNGLENABLEVERTEXATTRIBARRAYPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glEnableVertexAttribArray")));
	systemGenBuffers = reinterpret_cast< PFNGLGENBUFFERSARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGenBuffersARB")));
	systemGenVertexArrays = reinterpret_cast< PFNGLGENVERTEXARRAYSPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGenVertexArrays")));
	systemGetInfoLog = reinterpret_cast< PFNGLGETINFOLOGARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetInfoLogARB")));
	systemGetObjectParameteriv = reinterpret_cast< PFNGLGETOBJECTPARAMETERIVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetObjectParameterivARB")));
	systemGetUniformLocation = reinterpret_cast< PFNGLGETUNIFORMLOCATIONARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetUniformLocationARB")));
	systemLinkProgram = reinterpret_cast< PFNGLLINKPROGRAMARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glLinkProgramARB")));
	systemShaderSource = reinterpret_cast< PFNGLSHADERSOURCEARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glShaderSourceARB")));
	systemUniform1f = reinterpret_cast< PFNGLUNIFORM1FARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1fARB")));
	systemUniform1fv = reinterpret_cast< PFNGLUNIFORM1FVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1fvARB")));
	systemUniform2f = reinterpret_cast< PFNGLUNIFORM2FARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2fARB")));
	systemUniform2fv = reinterpret_cast< PFNGLUNIFORM2FVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2fvARB")));
	systemUniform3f = reinterpret_cast< PFNGLUNIFORM3FARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3fARB")));
	systemUniform3fv = reinterpret_cast< PFNGLUNIFORM3FVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3fvARB")));
	systemUniform4f = reinterpret_cast< PFNGLUNIFORM4FARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4fARB")));
	systemUniform4fv = reinterpret_cast< PFNGLUNIFORM4FVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4fvARB")));
	systemUniform1i = reinterpret_cast< PFNGLUNIFORM1IARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1iARB")));
	systemUniform1iv = reinterpret_cast< PFNGLUNIFORM1IVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1ivARB")));
	systemUniform2i = reinterpret_cast< PFNGLUNIFORM2IARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2iARB")));
	systemUniform2iv = reinterpret_cast< PFNGLUNIFORM2IVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2ivARB")));
	systemUniform3i = reinterpret_cast< PFNGLUNIFORM3IARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3iARB")));
	systemUniform3iv = reinterpret_cast< PFNGLUNIFORM3IVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3ivARB")));
	systemUniform4i = reinterpret_cast< PFNGLUNIFORM4IARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4iARB")));
	systemUniform4iv = reinterpret_cast< PFNGLUNIFORM4IVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4ivARB")));
	systemUseProgram = reinterpret_cast< PFNGLUSEPROGRAMPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUseProgram")));
	systemVertexAttribPointer = reinterpret_cast< PFNGLVERTEXATTRIBPOINTERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glVertexAttribPointer")));
	#else
	systemActiveTexture = reinterpret_cast< PFNGLACTIVETEXTUREARBPROC >(wglGetProcAddress("glActiveTextureARB"));
	systemAttachObject = reinterpret_cast< PFNGLATTACHOBJECTARBPROC >(wglGetProcAddress("glAttachObjectARB"));
	systemBindBuffer = reinterpret_cast< PFNGLBINDBUFFERARBPROC >(wglGetProcAddress("glBindBufferARB"));
	systemBindVertexArray = reinterpret_cast< PFNGLBINDVERTEXARRAYPROC >(wglGetProcAddress("glBindVertexBufferARB"));
	systemBufferData = reinterpret_cast< PFNGLBUFFERDATAARBPROC >(wglGetProcAddress("glBufferDataARB"));
	systemClientActiveTexture = reinterpret_cast< PFNGLCLIENTACTIVETEXTUREARBPROC >(wglGetProcAddress("glClientActiveTextureARB"));
	systemCompileShader = reinterpret_cast< PFNGLCOMPILESHADERARBPROC >(wglGetProcAddress("glCompileShaderARB"));
	systemCreateProgram = reinterpret_cast< PFNGLCREATEPROGRAMPROC >(wglGetProcAddress("glCreateProgram"));
	systemCreateShader = reinterpret_cast< PFNGLCREATESHADERPROC >(wglGetProcAddress("glCreateShader"));
	systemDeleteBuffers = reinterpret_cast< PFNGLDELETEBUFFERSARBPROC >(wglGetProcAddress("glDeleteBuffersARB"));
	systemDeleteProgram = reinterpret_cast< PFNGLDELETEPROGRAMPROC >(wglGetProcAddress("glDeleteProgram"));
	systemDeleteShader = reinterpret_cast< PFNGLDELETESHADERPROC >(wglGetProcAddress("glDeleteShader"));
	systemDisableVertexAttribArray = reinterpret_cast< PFNGLDISABLEVERTEXATTRIBARRAYPROC >(wglGetProcAddress("glDisableVertexAttribArray"));
	systemEnableVertexAttribArray = reinterpret_cast< PFNGLENABLEVERTEXATTRIBARRAYPROC >(wglGetProcAddress("glEnableVertexAttribArray"));
	systemGenBuffers = reinterpret_cast< PFNGLGENBUFFERSARBPROC >(wglGetProcAddress("glGenBuffersARB"));
	systemGenVertexArrays = reinterpret_cast< PFNGLGENVERTEXARRAYSPROC >(wglGetProcAddress("glGenVertexArraysARB"));
	systemGetInfoLog = reinterpret_cast< PFNGLGETINFOLOGARBPROC >(wglGetProcAddress("glGetInfoLogARB"));
	systemGetObjectParameteriv = reinterpret_cast< PFNGLGETOBJECTPARAMETERIVARBPROC >(wglGetProcAddress("glGetObjectParameterivARB"));
	systemGetUniformLocation = reinterpret_cast< PFNGLGETUNIFORMLOCATIONARBPROC >(wglGetProcAddress("glGetUniformLocationARB"));
	systemLinkProgram = reinterpret_cast< PFNGLLINKPROGRAMARBPROC >(wglGetProcAddress("glLinkProgramARB"));
	systemShaderSource = reinterpret_cast< PFNGLSHADERSOURCEARBPROC >(wglGetProcAddress("glShaderSourceARB"));
	systemUniform1f = reinterpret_cast< PFNGLUNIFORM1FARBPROC >(wglGetProcAddress("glUniform1fARB"));
	systemUniform1fv = reinterpret_cast< PFNGLUNIFORM1FVARBPROC >(wglGetProcAddress("glUniform1fvARB"));
	systemUniform2f = reinterpret_cast< PFNGLUNIFORM2FARBPROC >(wglGetProcAddress("glUniform2fARB"));
	systemUniform2fv = reinterpret_cast< PFNGLUNIFORM2FVARBPROC >(wglGetProcAddress("glUniform2fvARB"));
	systemUniform3f = reinterpret_cast< PFNGLUNIFORM3FARBPROC >(wglGetProcAddress("glUniform3fARB"));
	systemUniform3fv = reinterpret_cast< PFNGLUNIFORM3FVARBPROC >(wglGetProcAddress("glUniform3fvARB"));
	systemUniform4f = reinterpret_cast< PFNGLUNIFORM4FARBPROC >(wglGetProcAddress("glUniform4fARB"));
	systemUniform4fv = reinterpret_cast< PFNGLUNIFORM4FVARBPROC >(wglGetProcAddress("glUniform4fvARB"));
	systemUniform1i = reinterpret_cast< PFNGLUNIFORM1IARBPROC >(wglGetProcAddress("glUniform1iARB"));
	systemUniform1iv = reinterpret_cast< PFNGLUNIFORM1IVARBPROC >(wglGetProcAddress("glUniform1ivARB"));
	systemUniform2i = reinterpret_cast< PFNGLUNIFORM2IARBPROC >(wglGetProcAddress("glUniform2iARB"));
	systemUniform2iv = reinterpret_cast< PFNGLUNIFORM2IVARBPROC >(wglGetProcAddress("glUniform2ivARB"));
	systemUniform3i = reinterpret_cast< PFNGLUNIFORM3IARBPROC >(wglGetProcAddress("glUniform3iARB"));
	systemUniform3iv = reinterpret_cast< PFNGLUNIFORM3IVARBPROC >(wglGetProcAddress("glUniform3ivARB"));
	systemUniform4i = reinterpret_cast< PFNGLUNIFORM4IARBPROC >(wglGetProcAddress("glUniform4iARB"));
	systemUniform4iv = reinterpret_cast< PFNGLUNIFORM4IVARBPROC >(wglGetProcAddress("glUniform4ivARB"));
	systemUseProgram = reinterpret_cast< PFNGLUSEPROGRAMPROC >(wglGetProcAddress("glUseProgram"));
	systemVertexAttribPointer = reinterpret_cast< PFNGLVERTEXATTRIBPOINTERPROC >(wglGetProcAddress("glVertexAttribPointer"));
	#endif

	// Get properties
	glGetIntegerv(GL_MAX_LIGHTS, &max_lights);
// TODO: Ensure GL_MAX_TEXTURE_UNITS_ARB is the correct constant for this. Maybe it should be GL_MAX_TEXTURE_COORDS
//	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &tunit_count);
	HppCheckGlErrors();

	// Try to get number of texture units
	glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &tunit_count);
        GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		if (error == GL_INVALID_ENUM) {
			tunit_count = 1;
		} else {
			throw Exception("Unable to get number of texture units! OpenGL error code: " + sizeToStr(error));
		}
	}

	// Get OpenGL version
	std::string opengl_version(reinterpret_cast< char const* >(glGetString(GL_VERSION)));
	size_t dot_pos = opengl_version.find('.');
	opengl_version_big = strToSize(opengl_version.substr(0, dot_pos));
	size_t num_end = opengl_version.find_first_not_of("0123456789", dot_pos + 1);
	if (num_end != std::string::npos) {
		opengl_version_small = strToSize(opengl_version.substr(dot_pos+1, num_end - dot_pos - 1));
	} else {
		opengl_version_small = strToSize(opengl_version.substr(dot_pos+1));
	}

	HppCheckGlErrors();

}

}
