#include "genericmaterial.h"

namespace Hpp
{

// Shader codes
std::string const SHADER_VRT =
"\n#ifndef CMAP\n"
"varying vec4 color;"
"\n#endif\n"
"\n#ifdef NMAP\n"
"varying vec3 light_pos_tspace;"
"\n#else\n"
"varying vec3 normal;"
"varying vec3 light_pos_local_nrmz;"
"varying float light_pos_local_len;"
"\n#endif\n"
""
"void main(void)"
"{"
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"	gl_TexCoord[0] = gl_MultiTexCoord0;"
""
"\n#ifndef CMAP\n"
"	color = gl_FrontMaterial.diffuse;"
"\n#endif\n"
""
"	vec3 vertex_pos_eye = vec3(gl_ModelViewMatrix * gl_Vertex);"
"	vec3 light_pos_abs = gl_LightSource[0].position.xyz;"
""
"	vec3 light_pos_local = light_pos_abs - vertex_pos_eye;"
""
"\n#ifdef NMAP\n"
"	vec3 tangent = vec3(gl_MultiTexCoord1);"
"	vec3 binormal = vec3(gl_MultiTexCoord2);"
""
// TODO: Try without normalizations! It has been tried before, and it's pretty sure it works, at least when there is no scaling!
"	vec3 tspace_tangent = normalize(gl_NormalMatrix * tangent);"
"	vec3 tspace_binormal = normalize(gl_NormalMatrix * binormal);"
"	vec3 tspace_normal = normalize(gl_NormalMatrix * gl_Normal);"
""
"	light_pos_tspace.x = dot(light_pos_local, tspace_tangent);"
"	light_pos_tspace.y = dot(light_pos_local, tspace_binormal);"
"	light_pos_tspace.z = dot(light_pos_local, tspace_normal);"
"\n#else\n"
// TODO: Try without normalizations! It's pretty sure it works, at least when there is no scaling!
"	light_pos_local_nrmz = normalize(light_pos_local);"
"	light_pos_local_len = length(light_pos_local);"
"	normal = normalize(gl_NormalMatrix * gl_Normal);"
"\n#endif\n"
"}";

std::string const SHADER_FRG =
"\n#ifdef CMAP\n"
"uniform sampler2D cmap;"
"\n#else\n"
"varying vec4 color;"
"\n#endif\n"
"\n#ifdef NMAP\n"
"uniform sampler2D nmap;"
"varying vec3 light_pos_tspace;"
"\n#else\n"
"varying vec3 normal;"
"varying vec3 light_pos_local_nrmz;"
"varying float light_pos_local_len;"
"\n#endif\n"
""
"void main (void)"
"{"
"\n#ifdef NMAP\n"
"	vec3 nmap_normal = texture2D(nmap, gl_TexCoord[0].xy).xyz;"
"	nmap_normal.x = nmap_normal.x * 2.0 - 1.0;"
"	nmap_normal.y = nmap_normal.y * 2.0 - 1.0;"
"	vec3 normal = normalize(nmap_normal);"
"\n#endif\n"
"\n#ifdef CMAP\n"
"	vec4 color = texture2D(cmap, vec2(gl_TexCoord[0]));"
"\n#endif\n"
""
"	vec4 light = gl_LightSource[0].diffuse;"
"\n#ifdef NMAP\n"
"	light *= max(dot(normal, normalize(light_pos_tspace)), 0.0);"
"	float light_dst = length(light_pos_tspace);"
"\n#else\n"
// TODO: Try to use tangentspace?
"	light *= max(dot(normal, light_pos_local_nrmz), 0.0);"
"	float light_dst = light_pos_local_len;"
""
"\n#endif\n"
""
"	float attenuation_m_inv = 0;"
"\n#ifdef ATTENU_C\n"
"	attenuation_m_inv += gl_LightSource[0].constantAttenuation;"
"\n#endif\n"
"\n#ifdef ATTENU_L\n"
"	attenuation_m_inv += gl_LightSource[0].linearAttenuation * light_dst;"
"\n#endif\n"
"\n#ifdef ATTENU_Q\n"
"	attenuation_m_inv += gl_LightSource[0].quadraticAttenuation * light_dst * light_dst;"
"\n#endif\n"
"	if (attenuation_m_inv > 0) light /= attenuation_m_inv;"
""
"	gl_FragColor = color * light;"
"}";

Shader* GenericMaterial::shader_vrt;
Shader* GenericMaterial::shader_frg;
Shaderprogram* GenericMaterial::program;

void GenericMaterial::initShaders(void)
{
	shader_vrt = new Shader();
	shader_vrt->load(SHADER_VRT, Shader::VERTEX_SHADER);
	shader_frg = new Shader();
	shader_frg->load(SHADER_FRG, Shader::FRAGMENT_SHADER);
	program = new Shaderprogram();
	program->attachShader(shader_vrt);
	program->attachShader(shader_frg);
}

void GenericMaterial::deinitShaders(void)
{
	delete program;
	delete shader_vrt;
	delete shader_frg;
}

}
