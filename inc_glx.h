#ifndef HPP_INC_GLX_H
#define HPP_INC_GLX_H

#ifndef WIN32
#ifdef __APPLE__
#include <OpenGL/glx.h>
#else
#include <GL/glx.h>
#endif
#endif

#endif
