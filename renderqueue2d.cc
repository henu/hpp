#include "renderqueue2d.h"

namespace Hpp
{

// Shader codes
std::string const Renderqueue2d::SHADER_VRT =
"#version 330\n"
"\n"
"in vec2 pos;\n"
"in vec2 uv;\n"
"\n"
"out vec2 frag_uv;\n"
"\n"
"uniform mat3 viewmat;\n"
"\n"
"void main()\n"
"{\n"
"\n"
"	gl_Position = vec4(viewmat * vec3(pos.xy, 1), 1);\n"
"	frag_uv = uv;\n"
"}\n";

std::string const Renderqueue2d::SHADER_FRG =
"#version 330\n"
"\n"
"in vec2 frag_uv;\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"out vec4 final_color;\n"
"\n"
"void main()\n"
"{\n"
"	final_color = texture2D(tex, frag_uv);\n"
"}\n";

}
