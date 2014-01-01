#include "depthmaterial.h"

namespace Hpp
{

// Shader codes
std::string const SHADER_VRT =
"#version 110\n"
"\n"
"attribute vec4 pos;\n"
"varying vec4 frag_pos;\n"
"\n"
"uniform mat4 mvmat;\n"
"uniform mat4 pmat;\n"
"\n"
"#ifdef ALPHAMASK\n"
"attribute vec2 uv;\n"
"varying vec2 frag_uv;\n"
"#endif\n"
"\n"
"void main()\n"
"{\n"
"\n"
"	frag_pos = mvmat * pos;\n"
"	gl_Position = (pmat * mvmat) * pos;\n"
"	#ifdef ALPHAMASK\n"
"	frag_uv = uv;\n"
"	#endif\n"
"}\n";

std::string const SHADER_FRG =
"#version 110\n"
"\n"
"varying vec4 frag_pos;\n"
"\n"
"uniform float near;\n"
"uniform float far;\n"
"\n"
"#ifdef ALPHAMASK\n"
"varying vec2 frag_uv;\n"
"uniform sampler2D alphamask;\n"
"#endif\n"
"\n"
"vec4 depthToColor(float depth)\n"
"{\n"
"	#ifdef PACK\n"
"	const vec4 bias = vec4(1.0 / 255.0, 1.0 / 255.0, 1.0 / 255.0, 0.0);\n"
"\n"
"	float r = depth;\n"
"	float g = fract(r * 255.0);\n"
"	float b = fract(g * 255.0);\n"
"	float a = fract(b * 255.0);\n"
"\n"
"	vec4 color = vec4(r, g, b, a);\n"
"\n"
"	return color - (color.yzww * bias);\n"
"	#else\n"
"	return vec4(vec3(1.0, 1.0, 1.0) * depth, 1.0);\n"
"	#endif\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"\n"
"	#ifdef ALPHAMASK\n"
"	float alphamask_alpha = texture2D(alphamask, frag_uv).w;\n"
"	if (alphamask_alpha < 0.5) {\n"
"		discard;\n"
"	}\n"
"	#endif\n"
"\n"
"	#ifndef RADIAL\n"
"	float depth_raw = (abs(frag_pos.z) - near) / (far - near);\n"
"	#else\n"
"	float depth_raw = length(frag_pos) / far;\n"
"	#endif\n"
"	#ifdef COLOR\n"
"	gl_FragColor = depthToColor(depth_raw);\n"
"	#else\n"
"	gl_FragDepth = depth_raw;\n"
"	#endif\n"
"\n"
"}\n";

Shaderprogram* Depthmaterial::program;

void Depthmaterial::initShaders(void)
{
	Strings uniformnames;
	Strings vertexattributenames;

	uniformnames.push_back("pmat");
	uniformnames.push_back("mvmat");
	uniformnames.push_back("near");
	uniformnames.push_back("far");
	uniformnames.push_back("alphamask");

	vertexattributenames.push_back("pos");
	vertexattributenames.push_back("uv");

	Shader shader_vrt;
	shader_vrt.load(SHADER_VRT, Shader::VERTEX_SHADER);
	Shader shader_frg;
	shader_frg.load(SHADER_FRG, Shader::FRAGMENT_SHADER);
	program = new Shaderprogram(uniformnames, vertexattributenames);
	program->attachShader(shader_vrt);
	program->attachShader(shader_frg);
}

void Depthmaterial::deinitShaders(void)
{
	delete program;
}

}
