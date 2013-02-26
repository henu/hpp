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

PFNGLACTIVETEXTUREPROC GlSystem::systemActiveTexture = NULL;
PFNGLATTACHSHADERPROC GlSystem::systemAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC GlSystem::systemBindAttribLocation = NULL;
PFNGLBINDBUFFERPROC GlSystem::systemBindBuffer = NULL;
PFNGLBINDFRAMEBUFFERPROC GlSystem::systemBindFramebuffer = NULL;
PFNGLBINDVERTEXARRAYPROC GlSystem::systemBindVertexArray = NULL;
PFNGLBUFFERDATAPROC GlSystem::systemBufferData = NULL;
PFNGLCOMPILESHADERPROC GlSystem::systemCompileShader = NULL;
PFNGLCREATEPROGRAMPROC GlSystem::systemCreateProgram = NULL;
PFNGLCREATESHADERPROC GlSystem::systemCreateShader = NULL;
PFNGLDELETEBUFFERSPROC GlSystem::systemDeleteBuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC GlSystem::systemDeleteFramebuffers = NULL;
PFNGLDELETEPROGRAMPROC GlSystem::systemDeleteProgram = NULL;
PFNGLDELETESHADERPROC GlSystem::systemDeleteShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC GlSystem::systemDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC GlSystem::systemEnableVertexAttribArray = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC GlSystem::systemFramebufferTexture = NULL;
PFNGLFRAMEBUFFERTEXTURE1DPROC GlSystem::systemFramebufferTexture1D = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC GlSystem::systemFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERTEXTURE3DPROC GlSystem::systemFramebufferTexture3D = NULL;
PFNGLGENBUFFERSPROC GlSystem::systemGenBuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC GlSystem::systemGenFramebuffers = NULL;
PFNGLGENVERTEXARRAYSPROC GlSystem::systemGenVertexArrays = NULL;
PFNGLGETPROGRAMINFOLOGPROC GlSystem::systemGetProgramInfoLog = NULL;
PFNGLGETSHADERINFOLOGPROC GlSystem::systemGetShaderInfoLog = NULL;
PFNGLGETPROGRAMIVPROC GlSystem::systemGetProgramiv = NULL;
PFNGLGETSHADERIVPROC GlSystem::systemGetShaderiv = NULL;
PFNGLGETUNIFORMLOCATIONPROC GlSystem::systemGetUniformLocation = NULL;
PFNGLLINKPROGRAMPROC GlSystem::systemLinkProgram = NULL;
PFNGLSHADERSOURCEPROC GlSystem::systemShaderSource = NULL;
PFNGLUNIFORM1FPROC GlSystem::systemUniform1f = NULL;
PFNGLUNIFORM1FVPROC GlSystem::systemUniform1fv = NULL;
PFNGLUNIFORM2FPROC GlSystem::systemUniform2f = NULL;
PFNGLUNIFORM2FVPROC GlSystem::systemUniform2fv = NULL;
PFNGLUNIFORM3FPROC GlSystem::systemUniform3f = NULL;
PFNGLUNIFORM3FVPROC GlSystem::systemUniform3fv = NULL;
PFNGLUNIFORM4FPROC GlSystem::systemUniform4f = NULL;
PFNGLUNIFORM4FVPROC GlSystem::systemUniform4fv = NULL;
PFNGLUNIFORM1IPROC GlSystem::systemUniform1i = NULL;
PFNGLUNIFORM1IVPROC GlSystem::systemUniform1iv = NULL;
PFNGLUNIFORM2IPROC GlSystem::systemUniform2i = NULL;
PFNGLUNIFORM2IVPROC GlSystem::systemUniform2iv = NULL;
PFNGLUNIFORM3IPROC GlSystem::systemUniform3i = NULL;
PFNGLUNIFORM3IVPROC GlSystem::systemUniform3iv = NULL;
PFNGLUNIFORM4IPROC GlSystem::systemUniform4i = NULL;
PFNGLUNIFORM4IVPROC GlSystem::systemUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FVPROC GlSystem::systemUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC GlSystem::systemUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC GlSystem::systemUniformMatrix4fv = NULL;
PFNGLUNIFORMMATRIX2X3FVPROC GlSystem::systemUniformMatrix2x3fv = NULL;
PFNGLUNIFORMMATRIX3X2FVPROC GlSystem::systemUniformMatrix3x2fv = NULL;
PFNGLUNIFORMMATRIX2X4FVPROC GlSystem::systemUniformMatrix2x4fv = NULL;
PFNGLUNIFORMMATRIX4X2FVPROC GlSystem::systemUniformMatrix4x2fv = NULL;
PFNGLUNIFORMMATRIX3X4FVPROC GlSystem::systemUniformMatrix3x4fv = NULL;
PFNGLUNIFORMMATRIX4X3FVPROC GlSystem::systemUniformMatrix4x3fv = NULL;
PFNGLUSEPROGRAMPROC GlSystem::systemUseProgram = NULL;
PFNGLVERTEXATTRIBPOINTERPROC GlSystem::systemVertexAttribPointer = NULL;

GLint GlSystem::tunit_count = 0;
GLint GlSystem::max_vertexattributes = 0;
GLint GlSystem::max_lights = 0;
size_t GlSystem::opengl_version_big;
size_t GlSystem::opengl_version_small;

void GlSystem::initialize(void)
{

	HppCheckGlErrors();

	// Get pointers to functions of extensions
	#ifndef WIN32
	systemActiveTexture = reinterpret_cast< PFNGLACTIVETEXTUREPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glActiveTexture")));
	systemAttachShader = reinterpret_cast< PFNGLATTACHSHADERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glAttachShader")));
	systemBindAttribLocation = reinterpret_cast< PFNGLBINDATTRIBLOCATIONPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBindAttribLocation")));
	systemBindBuffer = reinterpret_cast< PFNGLBINDBUFFERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBindBuffer")));
	systemBindFramebuffer = reinterpret_cast< PFNGLBINDFRAMEBUFFERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBindFramebuffer")));
	systemBindVertexArray = reinterpret_cast< PFNGLBINDVERTEXARRAYPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBindVertexArray")));
	systemBufferData = reinterpret_cast< PFNGLBUFFERDATAPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glBufferData")));
	systemCompileShader = reinterpret_cast< PFNGLCOMPILESHADERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCompileShader")));
	systemCreateProgram = reinterpret_cast< PFNGLCREATEPROGRAMPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCreateProgram")));
	systemCreateShader = reinterpret_cast< PFNGLCREATESHADERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glCreateShader")));
	systemDeleteBuffers = reinterpret_cast< PFNGLDELETEBUFFERSPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteBuffers")));
	systemDeleteFramebuffers = reinterpret_cast< PFNGLDELETEFRAMEBUFFERSPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteFramebuffers")));
	systemDeleteProgram = reinterpret_cast< PFNGLDELETEPROGRAMPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteProgram")));
	systemDeleteShader = reinterpret_cast< PFNGLDELETESHADERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDeleteShader")));
	systemDisableVertexAttribArray = reinterpret_cast< PFNGLDISABLEVERTEXATTRIBARRAYPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glDisableVertexAttribArray")));
	systemEnableVertexAttribArray = reinterpret_cast< PFNGLENABLEVERTEXATTRIBARRAYPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glEnableVertexAttribArray")));
	systemFramebufferTexture = reinterpret_cast< PFNGLFRAMEBUFFERTEXTUREPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glFramebufferTexture")));
	systemFramebufferTexture1D = reinterpret_cast< PFNGLFRAMEBUFFERTEXTURE1DPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glFramebufferTexture1D")));
	systemFramebufferTexture2D = reinterpret_cast< PFNGLFRAMEBUFFERTEXTURE2DPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glFramebufferTexture2D")));
	systemFramebufferTexture3D = reinterpret_cast< PFNGLFRAMEBUFFERTEXTURE3DPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glFramebufferTexture3D")));
	systemGenBuffers = reinterpret_cast< PFNGLGENBUFFERSPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGenBuffers")));
	systemGenFramebuffers = reinterpret_cast< PFNGLGENFRAMEBUFFERSPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGenFramebuffers")));
	systemGenVertexArrays = reinterpret_cast< PFNGLGENVERTEXARRAYSPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGenVertexArrays")));
	systemGetProgramInfoLog = reinterpret_cast< PFNGLGETPROGRAMINFOLOGPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetProgramInfoLog")));
	systemGetShaderInfoLog = reinterpret_cast< PFNGLGETSHADERINFOLOGPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetShaderInfoLog")));
	systemGetProgramiv = reinterpret_cast< PFNGLGETPROGRAMIVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetProgramiv")));
	systemGetShaderiv = reinterpret_cast< PFNGLGETSHADERIVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetShaderiv")));
	systemGetUniformLocation = reinterpret_cast< PFNGLGETUNIFORMLOCATIONPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glGetUniformLocation")));
	systemLinkProgram = reinterpret_cast< PFNGLLINKPROGRAMPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glLinkProgram")));
	systemShaderSource = reinterpret_cast< PFNGLSHADERSOURCEPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glShaderSource")));
	systemUniform1f = reinterpret_cast< PFNGLUNIFORM1FPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1f")));
	systemUniform1fv = reinterpret_cast< PFNGLUNIFORM1FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1fv")));
	systemUniform2f = reinterpret_cast< PFNGLUNIFORM2FPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2f")));
	systemUniform2fv = reinterpret_cast< PFNGLUNIFORM2FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2fv")));
	systemUniform3f = reinterpret_cast< PFNGLUNIFORM3FPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3f")));
	systemUniform3fv = reinterpret_cast< PFNGLUNIFORM3FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3fv")));
	systemUniform4f = reinterpret_cast< PFNGLUNIFORM4FPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4f")));
	systemUniform4fv = reinterpret_cast< PFNGLUNIFORM4FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4fv")));
	systemUniform1i = reinterpret_cast< PFNGLUNIFORM1IPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1i")));
	systemUniform1iv = reinterpret_cast< PFNGLUNIFORM1IVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform1iv")));
	systemUniform2i = reinterpret_cast< PFNGLUNIFORM2IPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2i")));
	systemUniform2iv = reinterpret_cast< PFNGLUNIFORM2IVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform2iv")));
	systemUniform3i = reinterpret_cast< PFNGLUNIFORM3IPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3i")));
	systemUniform3iv = reinterpret_cast< PFNGLUNIFORM3IVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform3iv")));
	systemUniform4i = reinterpret_cast< PFNGLUNIFORM4IPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4i")));
	systemUniform4iv = reinterpret_cast< PFNGLUNIFORM4IVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniform4iv")));
	systemUniformMatrix2fv = reinterpret_cast< PFNGLUNIFORMMATRIX2FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix2fv")));
	systemUniformMatrix3fv = reinterpret_cast< PFNGLUNIFORMMATRIX3FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix3fv")));
	systemUniformMatrix4fv = reinterpret_cast< PFNGLUNIFORMMATRIX4FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix4fv")));
	systemUniformMatrix2x3fv = reinterpret_cast< PFNGLUNIFORMMATRIX2X3FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix2x3fv")));
	systemUniformMatrix3x2fv = reinterpret_cast< PFNGLUNIFORMMATRIX3X2FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix3x2fv")));
	systemUniformMatrix2x4fv = reinterpret_cast< PFNGLUNIFORMMATRIX2X4FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix2x4fv")));
	systemUniformMatrix4x2fv = reinterpret_cast< PFNGLUNIFORMMATRIX4X2FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix4x2fv")));
	systemUniformMatrix3x4fv = reinterpret_cast< PFNGLUNIFORMMATRIX3X4FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix3x4fv")));
	systemUniformMatrix4x3fv = reinterpret_cast< PFNGLUNIFORMMATRIX4X3FVPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUniformMatrix4x3fv")));
	systemUseProgram = reinterpret_cast< PFNGLUSEPROGRAMPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glUseProgram")));
	systemVertexAttribPointer = reinterpret_cast< PFNGLVERTEXATTRIBPOINTERPROC >(glXGetProcAddress(reinterpret_cast< GLubyte const* >("glVertexAttribPointer")));
	#else
	systemActiveTexture = reinterpret_cast< PFNGLACTIVETEXTUREPROC >(wglGetProcAddress("glActiveTexture"));
	systemAttachShader = reinterpret_cast< PFNGLATTACHSHADERPROC >(wglGetProcAddress("glAttachShader"));
	systemBindAttribLocation = reinterpret_cast< PFNGLBINDATTRIBLOCATIONPROC >(wglGetProcAddress("glBindAttribLocation"));
	systemBindBuffer = reinterpret_cast< PFNGLBINDBUFFERPROC >(wglGetProcAddress("glBindBuffer"));
	systemBindFramebuffer = reinterpret_cast< PFNGLBINDFRAMEBUFFERPROC >(wglGetProcAddress("glBindFramebuffer"));
	systemBindVertexArray = reinterpret_cast< PFNGLBINDVERTEXARRAYPROC >(wglGetProcAddress("glBindVertexBuffer"));
	systemBufferData = reinterpret_cast< PFNGLBUFFERDATAPROC >(wglGetProcAddress("glBufferData"));
	systemCompileShader = reinterpret_cast< PFNGLCOMPILESHADERPROC >(wglGetProcAddress("glCompileShader"));
	systemCreateProgram = reinterpret_cast< PFNGLCREATEPROGRAMPROC >(wglGetProcAddress("glCreateProgram"));
	systemCreateShader = reinterpret_cast< PFNGLCREATESHADERPROC >(wglGetProcAddress("glCreateShader"));
	systemDeleteBuffers = reinterpret_cast< PFNGLDELETEBUFFERSPROC >(wglGetProcAddress("glDeleteBuffers"));
	systemDeleteFramebuffers = reinterpret_cast< PFNGLDELETEFRAMEBUFFERSPROC >(wglGetProcAddress("glDeleteFramebuffers"));
	systemDeleteProgram = reinterpret_cast< PFNGLDELETEPROGRAMPROC >(wglGetProcAddress("glDeleteProgram"));
	systemDeleteShader = reinterpret_cast< PFNGLDELETESHADERPROC >(wglGetProcAddress("glDeleteShader"));
	systemDisableVertexAttribArray = reinterpret_cast< PFNGLDISABLEVERTEXATTRIBARRAYPROC >(wglGetProcAddress("glDisableVertexAttribArray"));
	systemEnableVertexAttribArray = reinterpret_cast< PFNGLENABLEVERTEXATTRIBARRAYPROC >(wglGetProcAddress("glEnableVertexAttribArray"));
	systemFramebufferTexture = reinterpret_cast< PFNGLFRAMEBUFFERTEXTUREPROC >(wglGetProcAddress("glFramebufferTexture"));
	systemFramebufferTexture1D = reinterpret_cast< PFNGLFRAMEBUFFERTEXTURE1DPROC >(wglGetProcAddress("glFramebufferTexture1D"));
	systemFramebufferTexture2D = reinterpret_cast< PFNGLFRAMEBUFFERTEXTURE2DPROC >(wglGetProcAddress("glFramebufferTexture2D"));
	systemFramebufferTexture3D = reinterpret_cast< PFNGLFRAMEBUFFERTEXTURE3DPROC >(wglGetProcAddress("glFramebufferTexture3D"));
	systemGenBuffers = reinterpret_cast< PFNGLGENBUFFERSPROC >(wglGetProcAddress("glGenBuffers"));
	systemGenFramebuffers = reinterpret_cast< PFNGLGENFRAMEBUFFERSPROC >(wglGetProcAddress("glGenFramebuffers"));
	systemGenVertexArrays = reinterpret_cast< PFNGLGENVERTEXARRAYSPROC >(wglGetProcAddress("glGenVertexArrays"));
	systemGetProgramInfoLog = reinterpret_cast< PFNGLGETPROGRAMINFOLOGPROC >(wglGetProcAddress("glGetProgramInfoLog"));
	systemGetShaderInfoLog = reinterpret_cast< PFNGLGETSHADERINFOLOGPROC >(wglGetProcAddress("glGetShaderInfoLog"));
	systemGetProgramiv = reinterpret_cast< PFNGLGETPROGRAMIVPROC >(wglGetProcAddress("glGetProgramiv"));
	systemGetShaderiv = reinterpret_cast< PFNGLGETSHADERIVPROC >(wglGetProcAddress("glGetShaderiv"));
	systemGetUniformLocation = reinterpret_cast< PFNGLGETUNIFORMLOCATIONPROC >(wglGetProcAddress("glGetUniformLocation"));
	systemLinkProgram = reinterpret_cast< PFNGLLINKPROGRAMPROC >(wglGetProcAddress("glLinkProgram"));
	systemShaderSource = reinterpret_cast< PFNGLSHADERSOURCEPROC >(wglGetProcAddress("glShaderSource"));
	systemUniform1f = reinterpret_cast< PFNGLUNIFORM1FPROC >(wglGetProcAddress("glUniform1f"));
	systemUniform1fv = reinterpret_cast< PFNGLUNIFORM1FVPROC >(wglGetProcAddress("glUniform1fv"));
	systemUniform2f = reinterpret_cast< PFNGLUNIFORM2FPROC >(wglGetProcAddress("glUniform2f"));
	systemUniform2fv = reinterpret_cast< PFNGLUNIFORM2FVPROC >(wglGetProcAddress("glUniform2fv"));
	systemUniform3f = reinterpret_cast< PFNGLUNIFORM3FPROC >(wglGetProcAddress("glUniform3f"));
	systemUniform3fv = reinterpret_cast< PFNGLUNIFORM3FVPROC >(wglGetProcAddress("glUniform3fv"));
	systemUniform4f = reinterpret_cast< PFNGLUNIFORM4FPROC >(wglGetProcAddress("glUniform4f"));
	systemUniform4fv = reinterpret_cast< PFNGLUNIFORM4FVPROC >(wglGetProcAddress("glUniform4fv"));
	systemUniform1i = reinterpret_cast< PFNGLUNIFORM1IPROC >(wglGetProcAddress("glUniform1i"));
	systemUniform1iv = reinterpret_cast< PFNGLUNIFORM1IVPROC >(wglGetProcAddress("glUniform1iv"));
	systemUniform2i = reinterpret_cast< PFNGLUNIFORM2IPROC >(wglGetProcAddress("glUniform2i"));
	systemUniform2iv = reinterpret_cast< PFNGLUNIFORM2IVPROC >(wglGetProcAddress("glUniform2iv"));
	systemUniform3i = reinterpret_cast< PFNGLUNIFORM3IPROC >(wglGetProcAddress("glUniform3i"));
	systemUniform3iv = reinterpret_cast< PFNGLUNIFORM3IVPROC >(wglGetProcAddress("glUniform3iv"));
	systemUniform4i = reinterpret_cast< PFNGLUNIFORM4IPROC >(wglGetProcAddress("glUniform4i"));
	systemUniform4iv = reinterpret_cast< PFNGLUNIFORM4IVPROC >(wglGetProcAddress("glUniform4iv"));
	systemUniformMatrix2fv = reinterpret_cast< PFNGLUNIFORMMATRIX2FVPROC >(wglGetProcAddress("glUniformMatrix2fv"));
	systemUniformMatrix3fv = reinterpret_cast< PFNGLUNIFORMMATRIX3FVPROC >(wglGetProcAddress("glUniformMatrix3fv"));
	systemUniformMatrix4fv = reinterpret_cast< PFNGLUNIFORMMATRIX4FVPROC >(wglGetProcAddress("glUniformMatrix4fv"));
	systemUniformMatrix2x3fv = reinterpret_cast< PFNGLUNIFORMMATRIX2X3FVPROC >(wglGetProcAddress("glUniformMatrix2x3fv"));
	systemUniformMatrix3x2fv = reinterpret_cast< PFNGLUNIFORMMATRIX3X2FVPROC >(wglGetProcAddress("glUniformMatrix3x2fv"));
	systemUniformMatrix2x4fv = reinterpret_cast< PFNGLUNIFORMMATRIX2X4FVPROC >(wglGetProcAddress("glUniformMatrix2x4fv"));
	systemUniformMatrix4x2fv = reinterpret_cast< PFNGLUNIFORMMATRIX4X2FVPROC >(wglGetProcAddress("glUniformMatrix4x2fv"));
	systemUniformMatrix3x4fv = reinterpret_cast< PFNGLUNIFORMMATRIX3X4FVPROC >(wglGetProcAddress("glUniformMatrix3x4fv"));
	systemUniformMatrix4x3fv = reinterpret_cast< PFNGLUNIFORMMATRIX4X3FVPROC >(wglGetProcAddress("glUniformMatrix4x3fv"));
	systemUseProgram = reinterpret_cast< PFNGLUSEPROGRAMPROC >(wglGetProcAddress("glUseProgram"));
	systemVertexAttribPointer = reinterpret_cast< PFNGLVERTEXATTRIBPOINTERPROC >(wglGetProcAddress("glVertexAttribPointer"));
	#endif

	// Get properties
	glGetIntegerv(GL_MAX_LIGHTS, &max_lights);
// TODO: Ensure GL_MAX_TEXTURE_UNITS is the correct constant for this. Maybe it should be GL_MAX_TEXTURE_COORDS
//	glGetIntegerv(GL_MAX_TEXTURE_UNITS_, &tunit_count);
	HppCheckGlErrors();

	// Try to get number of texture units
	glGetIntegerv(GL_MAX_TEXTURE_COORDS, &tunit_count);
        GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		if (error == GL_INVALID_ENUM) {
			tunit_count = 1;
		} else {
			throw Exception("Unable to get number of texture units! OpenGL error code: " + sizeToStr(error));
		}
	}

	// Try to get maximum number of vertex attributes
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertexattributes);
        error = glGetError();
	if (error != GL_NO_ERROR) {
		if (error == GL_INVALID_ENUM) {
			max_vertexattributes = 1;
		} else {
			throw Exception("Unable to get maximum number of vertexattributes! OpenGL error code: " + sizeToStr(error));
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
