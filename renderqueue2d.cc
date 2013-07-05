#include "renderqueue2d.h"

namespace Hpp
{

// Shader codes
std::string const Renderqueue2d::SHADER_VRT =
"#version 110\n"
"\n"
"attribute vec2 pos;\n"
"attribute vec2 uv;\n"
"attribute vec4 clr;\n"
"\n"
"varying vec2 frag_uv;\n"
"varying vec4 frag_color;\n"
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
"#version 110\n"
"\n"
"varying vec2 frag_uv;\n"
"varying vec4 frag_color;\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"void main()\n"
"{\n"
"	#ifndef FORCE_RGB_TO_ONE\n"
"	gl_FragColor = texture2D(tex, frag_uv) * frag_color;\n"
"	#else\n"
"	gl_FragColor = vec4(1, 1, 1, texture2D(tex, frag_uv).w) * frag_color;\n"
"	#endif\n"
"}\n";

Strings Renderqueue2d::uniformnames;
Strings Renderqueue2d::vertexattributenames;

Shaderprogram* Renderqueue2d::program = NULL;

void Renderqueue2d::initShaders(void)
{
	uniformnames.clear();
	uniformnames.push_back("viewmat");
	uniformnames.push_back("tex");

	vertexattributenames.clear();
	vertexattributenames.push_back("pos");
	vertexattributenames.push_back("uv");
	vertexattributenames.push_back("clr");

	// Initialize shaderprogram
	Shader shader_vrt;
	shader_vrt.load(SHADER_VRT, Shader::VERTEX_SHADER);
	Shader shader_frg;
	shader_frg.load(SHADER_FRG, Shader::FRAGMENT_SHADER);
	program = new Shaderprogram(uniformnames, vertexattributenames);
	program->attachShader(shader_vrt);
	program->attachShader(shader_frg);
}

void Renderqueue2d::deinitShaders(void)
{
	delete program;
	program = NULL;
}

}
