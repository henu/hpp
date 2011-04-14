#ifndef HPP_RENDBUF_ENUMS_H
#define HPP_RENDBUF_ENUMS_H

#include "inc_gl.h"


namespace Hpp
{

namespace RendbufEnums
{

enum Datatype { VERTEX,
                NORMAL,
                COLOR,
                TEXCOORD };
enum Drawtype { TRIANGLES,
                QUADS };
enum VBOType { ARRAY = GL_ARRAY_BUFFER,
               ELEMENTS = GL_ELEMENT_ARRAY_BUFFER };
enum VBOState { NO_VBO,
                VBO_NOT_READY,
                VBO_READY,
                VBO_READY_FROM_MANAGER };

}

}

#endif
