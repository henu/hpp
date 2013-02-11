#include "genericmaterial.h"

namespace Hpp
{

// Shader codes
std::string const SHADER_VRT =
"#version 330 core\n"
"\n"
"in vec4 pos;\n"
"in vec4 normal;\n"
"in vec3 vert_color;\n"
"#ifdef CMAP\n"
"in vec2 uv;\n"
"#endif\n"
"#ifdef NMAP\n"
"#ifndef CMAP\n"
"in vec2 uv;\n"
"#endif\n"
"#endif\n"
"\n"
"#ifndef SHADELESS\n"
"#ifdef LIGHT\n"
"uniform vec4 light_pos_viewspace;\n"
"#ifdef NMAP\n"
"in vec3 tangent;\n"
"in vec3 binormal;\n"
"out vec3 light_pos_tspace;\n"
"#else\n"
"out vec3 frag_normal;\n"
"out vec3 light_pos_vertexspace_nrmz;\n"
"out float light_pos_vertexspace_len;\n"
"#endif\n"
"#endif\n"
"#endif\n"
"\n"
"out vec2 frag_uv;\n"
"\n"
"uniform mat4 mvmat;\n"
"uniform mat4 pmat;\n"
"\n"
"void main()\n"
"{\n"
"\n"
"	gl_Position = (pmat * mvmat) * pos;\n"
"\n"
"	#ifdef CMAP\n"
"	frag_uv = uv;\n"
"	#endif\n"
"	#ifdef NMAP\n"
"	#ifndef CMAP\n"
"	frag_uv = uv;\n"
"	#endif\n"
"	#endif\n"
"\n"
"	// Calculate light direction, etc.\n"
"	#ifdef LIGHT\n"
"		vec3 light_pos_vertexspace;\n"
"		if (light_pos_viewspace.w > 0.5) {\n"
"			vec3 vertex_pos_eye = vec3(mvmat * pos);\n"
"			light_pos_vertexspace = light_pos_viewspace.xyz - vertex_pos_eye;\n"
"		} else {\n"
"			light_pos_vertexspace = light_pos_viewspace.xyz;\n"
"		}\n"
"\n"
"		// Calculate normal stuff\n"
"		#ifdef NMAP\n"
"// TODO: Try without normalizations! It has been tried before, and it's pretty sure it works, at least when there is no scaling!\n"
"			vec3 tspace_tangent = normalize((mvmat * vec4(tangent.xyz, 0.0)).xyz);\n"
"			vec3 tspace_binormal = normalize((mvmat * vec4(binormal.xyz, 0.0)).xyz);\n"
"			vec3 tspace_normal = normalize((mvmat * vec4(normal.xyz, 0.0)).xyz);\n"
"\n"
"			light_pos_tspace.x = dot(light_pos_vertexspace, tspace_tangent);\n"
"			light_pos_tspace.y = dot(light_pos_vertexspace, tspace_binormal);\n"
"			light_pos_tspace.z = dot(light_pos_vertexspace, tspace_normal);\n"
"		#else\n"
"// TODO: Try without normalizations! It's pretty sure it works, at least when there is no scaling!\n"
"			light_pos_vertexspace_nrmz = normalize(light_pos_vertexspace);\n"
"			light_pos_vertexspace_len = length(light_pos_vertexspace);\n"
"			frag_normal = normalize(mvmat * vec4(normal.xyz, 0.0)).xyz;\n"
"		#endif\n"
"\n"
"	#endif\n"
"\n"
"}\n";

std::string const SHADER_FRG =
"#version 330 core\n"
"\n"
"// Global color\n"
"#ifdef COLOR\n"
"uniform vec4 material_color;\n"
"#endif\n"
"#ifdef CMAP\n"
"uniform sampler2D cmap;\n"
"#endif\n"
"#ifdef NMAP\n"
"uniform sampler2D nmap;\n"
"#endif\n"
"#ifdef SMAP\n"
"uniform sampler2D smap;\n"
"#endif\n"
"\n"
"in vec2 frag_uv;\n"
"\n"
"// Ambient light\n"
"#ifdef AMBIENT_LIGHT\n"
"uniform vec4 ambient_light;\n"
"#endif\n"
"\n"
"#ifndef SHADELESS\n"
"#ifdef LIGHT\n"
"uniform vec3 light_color;\n"
"uniform vec4 light_pos;\n"
"#ifdef NMAP\n"
"in vec3 light_pos_tspace;\n"
"#else\n"
"in vec3 frag_normal;\n"
"in vec3 light_pos_vertexspace_nrmz;\n"
"in float light_pos_vertexspace_len;\n"
"#endif\n"
"#ifdef ATTENU_C\n"
"uniform float light_constant_attenuation;\n"
"#endif\n"
"#ifdef ATTENU_L\n"
"uniform float light_linear_attenuation;\n"
"#endif\n"
"#ifdef ATTENU_Q\n"
"uniform float light_quadratic_attenuation;\n"
"#endif\n"
"#endif\n"
"#endif\n"
"\n"
"out vec4 final_color;\n"
"\n"
"void main()\n"
"{\n"
"\n"
"	#ifdef COLOR\n"
"	vec3 color = material_color.xyz;\n"
"	#endif\n"
"\n"
"	#ifdef NMAP\n"
"		vec3 nmap_normal = texture2D(nmap, frag_uv).xyz;\n"
"		nmap_normal.x = nmap_normal.x * 2.0 - 1.0;\n"
"		nmap_normal.y = nmap_normal.y * 2.0 - 1.0;\n"
"		vec3 frag_normal = normalize(nmap_normal);\n"
"	#endif\n"
"	#ifdef CMAP\n"
"		color *= texture2D(cmap, frag_uv).xyz;\n"
"	#endif\n"
"\n"
"	// If shadeless, then do not calculate lights\n"
"	#ifdef SHADELESS\n"
"		vec3 light = vec3(1.0, 1.0, 1.0);\n"
"	#else\n"
"\n"
"		// Apply possible ambient light\n"
"		#ifdef AMBIENT_LIGHT\n"
"			vec3 light = ambient_light.xyz;\n"
"		#else\n"
"			vec3 light = vec3(0.0, 0.0, 0.0);\n"
"		#endif\n"
"\n"
"		// Apply possible lightsource\n"
"		#ifdef LIGHT\n"
"			vec3 light_diffuse = light_color;\n"
"			float light_diffuse_distance;\n"
"			#ifdef NMAP\n"
"				light_diffuse *= max(dot(frag_normal, normalize(light_pos_tspace)), 0.0);\n"
"				if (light_pos.w > 0.5) light_diffuse_distance = length(light_pos_tspace);\n"
"			#else\n"
"// TODO: Try to use tangentspace?\n"
"				light_diffuse *= max(dot(frag_normal, light_pos_vertexspace_nrmz), 0.0);\n"
"				if (light_pos.w > 0.5) light_diffuse_distance = light_pos_vertexspace_len;\n"
"			#endif\n"
"\n"
"			if (light_pos.w > 0.5) {\n"
"				float attenuation_m_inv = 0.0;\n"
"				#ifdef ATTENU_C\n"
"					attenuation_m_inv += light_constant_attenuation;\n"
"				#endif\n"
"				#ifdef ATTENU_L\n"
"					attenuation_m_inv += light_linear_attenuation * light_diffuse_distance;\n"
"				#endif\n"
"				#ifdef ATTENU_Q\n"
"					attenuation_m_inv += light_quadratic_attenuation * light_diffuse_distance * light_diffuse_distance;\n"
"				#endif\n"
"				if (attenuation_m_inv > 0.0) light_diffuse /= attenuation_m_inv;\n"
"			}\n"
"\n"
"			light += light_diffuse;\n"
"		#endif\n"
"\n"
"	#endif\n"
"\n"
"	#ifdef COLOR\n"
"	final_color = vec4(color.xyz * light, 1);\n"
"	#else\n"
"	final_color = vec4(light.xyz, 1);\n"
"	#endif\n"
"\n"
"}\n";

float const GenericMaterial::AMBIENT_LIGHT_ON_THRESHOLD = 0.005;
float const GenericMaterial::NEEDS_LIGHT_THRESHOLD = 0.005;
float const GenericMaterial::TRANSLUCENT_THRESHOLD = 0.995;

Shaderprogram* GenericMaterial::program;

void GenericMaterial::initShaders(void)
{
	Shader shader_vrt;
	shader_vrt.load(SHADER_VRT, Shader::VERTEX_SHADER);
	Shader shader_frg;
	shader_frg.load(SHADER_FRG, Shader::FRAGMENT_SHADER);
	program = new Shaderprogram();
	program->attachShader(shader_vrt);
	program->attachShader(shader_frg);
}

void GenericMaterial::deinitShaders(void)
{
	delete program;
}

}
