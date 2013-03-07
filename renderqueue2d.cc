#include "renderqueue2d.h"

namespace Hpp
{

// Shader codes
std::string const Renderqueue2d::SHADER_VRT =
"#version 330\n"
"\n"
"in vec2 pos;\n"
"in vec2 uv;\n"
"in vec4 clr;\n"
"\n"
"out vec2 frag_uv;\n"
"out vec4 frag_color;\n"
"\n"
"uniform mat3 viewmat;\n"
"\n"
"void main()\n"
"{\n"
"\n"
"	gl_Position = vec4(viewmat * vec3(pos.xy, 1), 1);\n"
"	frag_uv = uv;\n"
"	frag_color = clr;\n"
"}\n";

std::string const Renderqueue2d::SHADER_FRG =
"#version 330\n"
"\n"
"in vec2 frag_uv;\n"
"in vec4 frag_color;\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"out vec4 final_color;\n"
"\n"
"void main()\n"
"{\n"
"	#ifndef FORCE_RGB_TO_ONE\n"
"	final_color = texture2D(tex, frag_uv) * frag_color;\n"
"	#else\n"
"	final_color = vec4(1, 1, 1, texture2D(tex, frag_uv).w) * frag_color;\n"
"	#endif\n"
"}\n";

}
