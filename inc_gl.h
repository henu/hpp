#ifndef HPP_INC_GL_H
#define HPP_INC_GL_H

#ifdef WIN32
#include <GL/glew.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#endif
