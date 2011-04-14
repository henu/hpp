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
PFNGLBUFFERDATAARBPROC GlSystem::systemBufferData = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC GlSystem::systemClientActiveTexture = NULL;
PFNGLCOMPILESHADERARBPROC GlSystem::systemCompileShader = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC GlSystem::systemCreateProgramObject = NULL;
PFNGLCREATESHADEROBJECTARBPROC GlSystem::systemCreateShaderObject = NULL;
PFNGLGENBUFFERSARBPROC GlSystem::systemGenBuffers = NULL;
PFNGLGETINFOLOGARBPROC GlSystem::systemGetInfoLog = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC GlSystem::systemGetObjectParameteriv = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC GlSystem::systemGetUniformLocation = NULL;
PFNGLDELETEBUFFERSARBPROC GlSystem::systemDeleteBuffers = NULL;
PFNGLDELETEOBJECTARBPROC GlSystem::systemDeleteObject = NULL;
PFNGLLINKPROGRAMARBPROC GlSystem::systemLinkProgram = NULL;
PFNGLSHADERSOURCEARBPROC GlSystem::systemShaderSource = NULL;
PFNGLUNIFORM1IARBPROC GlSystem::systemUniform1i = NULL;
PFNGLUNIFORM2IARBPROC GlSystem::systemUniform2i = NULL;
PFNGLUNIFORM3IARBPROC GlSystem::systemUniform3i = NULL;
PFNGLUNIFORM4IARBPROC GlSystem::systemUniform4i = NULL;
PFNGLUNIFORM1FARBPROC GlSystem::systemUniform1f = NULL;
PFNGLUNIFORM2FARBPROC GlSystem::systemUniform2f = NULL;
PFNGLUNIFORM3FARBPROC GlSystem::systemUniform3f = NULL;
PFNGLUNIFORM4FARBPROC GlSystem::systemUniform4f = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC GlSystem::systemUseProgramObject = NULL;
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
	systemBufferData = reinterpret_cast< PFNGLBUFFERDATAARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBufferDataARB")));
	systemClientActiveTexture = reinterpret_cast< PFNGLCLIENTACTIVETEXTUREARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glClientActiveTextureARB")));
	systemCompileShader = reinterpret_cast< PFNGLCOMPILESHADERARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCompileShaderARB")));
	systemCreateProgramObject = reinterpret_cast< PFNGLCREATEPROGRAMOBJECTARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCreateProgramObjectARB")));
	systemCreateShaderObject = reinterpret_cast< PFNGLCREATESHADEROBJECTARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCreateShaderObjectARB")));
	systemDeleteBuffers = reinterpret_cast< PFNGLDELETEBUFFERSARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteBuffersARB")));
	systemDeleteObject = reinterpret_cast< PFNGLDELETEOBJECTARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteObjectARB")));
	systemGenBuffers = reinterpret_cast< PFNGLGENBUFFERSARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGenBuffersARB")));
	systemGetInfoLog = reinterpret_cast< PFNGLGETINFOLOGARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetInfoLogARB")));
	systemGetObjectParameteriv = reinterpret_cast< PFNGLGETOBJECTPARAMETERIVARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetObjectParameterivARB")));
	systemGetUniformLocation = reinterpret_cast< PFNGLGETUNIFORMLOCATIONARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetUniformLocationARB")));
	systemLinkProgram = reinterpret_cast< PFNGLLINKPROGRAMARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glLinkProgramARB")));
	systemShaderSource = reinterpret_cast< PFNGLSHADERSOURCEARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glShaderSourceARB")));
	systemUniform1i = reinterpret_cast< PFNGLUNIFORM1IARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1iARB")));
	systemUniform2i = reinterpret_cast< PFNGLUNIFORM2IARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2iARB")));
	systemUniform3i = reinterpret_cast< PFNGLUNIFORM3IARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3iARB")));
	systemUniform4i = reinterpret_cast< PFNGLUNIFORM4IARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4iARB")));
	systemUniform1f = reinterpret_cast< PFNGLUNIFORM1FARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1fARB")));
	systemUniform2f = reinterpret_cast< PFNGLUNIFORM2FARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2fARB")));
	systemUniform3f = reinterpret_cast< PFNGLUNIFORM3FARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3fARB")));
	systemUniform4f = reinterpret_cast< PFNGLUNIFORM4FARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4fARB")));
	systemUseProgramObject = reinterpret_cast< PFNGLUSEPROGRAMOBJECTARBPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUseProgramObjectARB")));
	#else
	systemActiveTexture = reinterpret_cast< PFNGLACTIVETEXTUREARBPROC >(wglGetProcAddress("glActiveTextureARB"));
	systemAttachObject = reinterpret_cast< PFNGLATTACHOBJECTARBPROC >(wglGetProcAddress("glAttachObjectARB"));
	systemBindBuffer = reinterpret_cast< PFNGLBINDBUFFERARBPROC >(wglGetProcAddress("glBindBufferARB"));
	systemBufferData = reinterpret_cast< PFNGLBUFFERDATAARBPROC >(wglGetProcAddress("glBufferDataARB"));
	systemClientActiveTexture = reinterpret_cast< PFNGLCLIENTACTIVETEXTUREARBPROC >(wglGetProcAddress("glClientActiveTextureARB"));
	systemCompileShader = reinterpret_cast< PFNGLCOMPILESHADERARBPROC >(wglGetProcAddress("glCompileShaderARB"));
	systemCreateProgramObject = reinterpret_cast< PFNGLCREATEPROGRAMOBJECTARBPROC >(wglGetProcAddress("glCreateProgramObjectARB"));
	systemCreateShaderObject = reinterpret_cast< PFNGLCREATESHADEROBJECTARBPROC >(wglGetProcAddress("glCreateShaderObjectARB"));
	systemDeleteBuffers = reinterpret_cast< PFNGLDELETEBUFFERSARBPROC >(wglGetProcAddress("glDeleteBuffersARB"));
	systemDeleteObject = reinterpret_cast< PFNGLDELETEOBJECTARBPROC >(wglGetProcAddress("glDeleteObjectARB"));
	systemGenBuffers = reinterpret_cast< PFNGLGENBUFFERSARBPROC >(wglGetProcAddress("glGenBuffersARB"));
	systemGetInfoLog = reinterpret_cast< PFNGLGETINFOLOGARBPROC >(wglGetProcAddress("glGetInfoLogARB"));
	systemGetObjectParameteriv = reinterpret_cast< PFNGLGETOBJECTPARAMETERIVARBPROC >(wglGetProcAddress("glGetObjectParameterivARB"));
	systemGetUniformLocation = reinterpret_cast< PFNGLGETUNIFORMLOCATIONARBPROC >(wglGetProcAddress("glGetUniformLocationARB"));
	systemLinkProgram = reinterpret_cast< PFNGLLINKPROGRAMARBPROC >(wglGetProcAddress("glLinkProgramARB"));
	systemShaderSource = reinterpret_cast< PFNGLSHADERSOURCEARBPROC >(wglGetProcAddress("glShaderSourceARB"));
	systemUniform1i = reinterpret_cast< PFNGLUNIFORM1IARBPROC >(wglGetProcAddress("glUniform1iARB"));
	systemUniform2i = reinterpret_cast< PFNGLUNIFORM2IARBPROC >(wglGetProcAddress("glUniform2iARB"));
	systemUniform3i = reinterpret_cast< PFNGLUNIFORM3IARBPROC >(wglGetProcAddress("glUniform3iARB"));
	systemUniform4i = reinterpret_cast< PFNGLUNIFORM4IARBPROC >(wglGetProcAddress("glUniform4iARB"));
	systemUniform1f = reinterpret_cast< PFNGLUNIFORM1FARBPROC >(wglGetProcAddress("glUniform1fARB"));
	systemUniform2f = reinterpret_cast< PFNGLUNIFORM2FARBPROC >(wglGetProcAddress("glUniform2fARB"));
	systemUniform3f = reinterpret_cast< PFNGLUNIFORM3FARBPROC >(wglGetProcAddress("glUniform3fARB"));
	systemUniform4f = reinterpret_cast< PFNGLUNIFORM4FARBPROC >(wglGetProcAddress("glUniform4fARB"));
	systemUseProgramObject = reinterpret_cast< PFNGLUSEPROGRAMOBJECTARBPROC >(wglGetProcAddress("glUseProgramObjectARB"));
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
