#ifndef HPP_GENERICMATERIAL_H
#define HPP_GENERICMATERIAL_H

#include "shaderprogram.h"
#include "rawmaterial.h"
#include "color.h"
#include "material.h"
#include "gldebug.h"
#include "inc_gl.h"
#include "assert.h"
#include "texture.h"
#include "texturemanager.h"

namespace Hpp
{

class GenericMaterial : public Material
{

	friend class Display;

public:

	// Constructor and destructor
	inline GenericMaterial(Rawmaterial const& rawmat, bool twosided = false);
	inline virtual ~GenericMaterial(void);

	// Virtual functions, needed by Material
	inline virtual bool needsLight(Renderable const* renderable) const;

private:

	// Functions for friends
	static void initShaders(void);
	static void deinitShaders(void);

private:

	// The color and alpha of Material
	Color color;
	Color specular;
	float shininess;
	float ambient_multiplier;
	float emittance;

	// Textures
	Texture* colormap;
	Texture* normalmap;
	Texture* specularmap;

	// Some options
	bool twosided;
	bool needs_light;
	bool is_translucent;
	bool needs_uvs;
	bool no_repeat;
	float normalmap_weight;

	// Class-wide shaders
	static Shader* shader_vrt;
	static Shader* shader_frg;
	static Shaderprogram* program;

	// Virtual functions, needed by Material
	inline virtual void initRendering(size_t num_of_lights, Flags flags) const;
	inline virtual void deinitRendering(void) const;
	inline virtual void doRendering(Renderable const* renderable,
	                                Rendbuf< Real > const* poss,
	                                Rendbuf< Real > const* nrms,
	                                std::vector< Rendbuf< Real > const* > const& uvs,
	                                Rendbuf< Real > const* clrs,
	                                Rendbuf< RIdx > const* tris) const;

};

inline GenericMaterial::GenericMaterial(Rawmaterial const& rawmat, bool twosided) :
color(rawmat.color),
specular(rawmat.specular),
shininess(rawmat.shininess),
ambient_multiplier(rawmat.ambient_multiplier),
emittance(rawmat.emittance),
twosided(twosided),
no_repeat(false),
normalmap_weight(rawmat.normalmap_weight)
{
	// Some options
	Real const MIN_TO_NEED_LIGHT = 0.001;
	Real const MAX_TO_BE_TRANSLUCENT = 0.999;

	if (color.getRed() > MIN_TO_NEED_LIGHT ||
	    color.getGreen() > MIN_TO_NEED_LIGHT ||
	    color.getBlue() > MIN_TO_NEED_LIGHT ||
	    specular.getRed() > MIN_TO_NEED_LIGHT ||
	    specular.getGreen() > MIN_TO_NEED_LIGHT ||
	    specular.getBlue() > MIN_TO_NEED_LIGHT) {
	    	needs_light = true;
	} else {
		needs_light = false;
	}

	if (color.getAlpha() <= MAX_TO_BE_TRANSLUCENT) {
		is_translucent = true;
	} else {
		is_translucent = false;
	}

	needs_uvs = false;
	if (rawmat.colormap_tex) {
		colormap = rawmat.colormap_tex;
		needs_uvs = true;
	} else if (!rawmat.colormap.empty()) {
		colormap = Texturemanager::getTexture(rawmat.colormap);
		needs_uvs = true;
	} else {
		colormap = NULL;
	}
	if (rawmat.normalmap_tex) {
		normalmap = rawmat.normalmap_tex;
		needs_uvs = true;
	} else if (!rawmat.normalmap.empty()) {
		normalmap = Texturemanager::getTexture(rawmat.normalmap);
		needs_uvs = true;
	} else {
		normalmap = NULL;
	}
	if (rawmat.specularmap_tex) {
		specularmap = rawmat.specularmap_tex;
		needs_uvs = true;
	} else if (!rawmat.specularmap.empty()) {
		specularmap = Texturemanager::getTexture(rawmat.specularmap);
		needs_uvs = true;
	} else {
		specularmap = NULL;
	}
}

inline GenericMaterial::~GenericMaterial(void)
{
}

inline bool GenericMaterial::needsLight(Renderable const* renderable) const
{
	(void)renderable;
	return needs_light;
}

inline void GenericMaterial::initRendering(size_t num_of_lights, Flags flags) const
{
HppAssert(!(flags & Material::ADDITIVE_RENDERING), "Additive rendering not implemented yet!");
// TODO: Implement additive rendering!

// TODO: Implement using of normalmap_weight!

	HppAssert(num_of_lights <= 1, "Terrainmaterial: Only one or less lights supported!");

	HppCheckGlErrors();

	float difl_buf[4] = { color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() };
	float ambl_buf[4] = { color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() };
	float spec_buf[4] = { 0.0, 0.0, 0.0, color.getAlpha() };
	float emis_buf[4] = { 0.0, 0.0, 0.0, color.getAlpha() };

	if (is_translucent) {
		HppAssert(!glIsEnabled(GL_BLEND), "Blend should not be enabled now!");
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, difl_buf);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambl_buf);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec_buf);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis_buf);

	HppAssert(glIsEnabled(GL_CULL_FACE), "Face culling is not enabled!");
	if (twosided) {
		glDisable(GL_CULL_FACE);
	}

	// Enable arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (needs_uvs) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		GlSystem::ActiveTexture(GL_TEXTURE1);
		GlSystem::ClientActiveTexture(GL_TEXTURE1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		GlSystem::ActiveTexture(GL_TEXTURE2);
		GlSystem::ClientActiveTexture(GL_TEXTURE2);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	Shaderprogram::Flags sflags;

	if (colormap) {
		GlSystem::ActiveTexture(GL_TEXTURE0);
		GlSystem::ClientActiveTexture(GL_TEXTURE0);
		HppCheckGlErrors();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, colormap->getGlTexture());
		HppCheckGlErrors();
		if (no_repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			HppCheckGlErrors();
		}
		sflags.insert("CMAP");
	}
	if (normalmap) {
		GlSystem::ActiveTexture(GL_TEXTURE1);
		GlSystem::ClientActiveTexture(GL_TEXTURE1);
		HppCheckGlErrors();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, normalmap->getGlTexture());
		HppCheckGlErrors();
		if (no_repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			HppCheckGlErrors();
		}
		sflags.insert("NMAP");
	}
	if (specularmap) {
		GlSystem::ActiveTexture(GL_TEXTURE2);
		GlSystem::ClientActiveTexture(GL_TEXTURE2);
		HppCheckGlErrors();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, specularmap->getGlTexture());
		HppCheckGlErrors();
		if (no_repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			HppCheckGlErrors();
		}
		sflags.insert("SMAP");
	}
	GlSystem::ActiveTexture(GL_TEXTURE0);
	GlSystem::ClientActiveTexture(GL_TEXTURE0);

	// Initialize shaders
	if (colormap || normalmap || specularmap) {
		// Set light flags
		GLfloat attenu_c, attenu_l, attenu_q;
		glGetLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, &attenu_c);
		glGetLightfv(GL_LIGHT0, GL_LINEAR_ATTENUATION, &attenu_l);
		glGetLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, &attenu_q);
		if (attenu_c != 0) sflags.insert("ATTENU_C");
		if (attenu_l != 0) sflags.insert("ATTENU_L");
		if (attenu_q != 0) sflags.insert("ATTENU_Q");
		program->enable(sflags);
	}

	if (colormap) {
		GlSystem::Uniform1i(GlSystem::GetUniformLocation(program->getGLSLProgram(), "cmap"), 0);
	}
	if (normalmap) {
		GlSystem::Uniform1i(GlSystem::GetUniformLocation(program->getGLSLProgram(), "nmap"), 1);
	}
	if (specularmap) {
		GlSystem::Uniform1i(GlSystem::GetUniformLocation(program->getGLSLProgram(), "smap"), 2);
	}
}

inline void GenericMaterial::deinitRendering(void) const
{
	if (colormap || normalmap || specularmap) {
		program->disable();
	}

	if (normalmap) {
		GlSystem::ActiveTexture(GL_TEXTURE1);
		GlSystem::ClientActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		HppCheckGlErrors();
		if (no_repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			HppCheckGlErrors();
		}
	}
	if (specularmap) {
		GlSystem::ActiveTexture(GL_TEXTURE2);
		GlSystem::ClientActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		HppCheckGlErrors();
		if (no_repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			HppCheckGlErrors();
		}
	}
	GlSystem::ActiveTexture(GL_TEXTURE0);
	GlSystem::ClientActiveTexture(GL_TEXTURE0);
	if (colormap) {
		glDisable(GL_TEXTURE_2D);
		HppCheckGlErrors();
		if (no_repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			HppCheckGlErrors();
		}
	}
	// Disable arrays
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if (needs_uvs) {
		GlSystem::ActiveTexture(GL_TEXTURE2);
		GlSystem::ClientActiveTexture(GL_TEXTURE2);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		GlSystem::ActiveTexture(GL_TEXTURE1);
		GlSystem::ClientActiveTexture(GL_TEXTURE1);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		GlSystem::ActiveTexture(GL_TEXTURE0);
		GlSystem::ClientActiveTexture(GL_TEXTURE0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	if (is_translucent) {
		HppAssert(glIsEnabled(GL_BLEND), "Blend should be enabled now!");
		glDisable(GL_BLEND);
	}
	HppCheckGlErrors();
	if (twosided) {
		glEnable(GL_CULL_FACE);
	}
	HppCheckGlErrors();
}

inline void GenericMaterial::doRendering(Renderable const* renderable,
                                         Rendbuf< Real > const* poss,
                                         Rendbuf< Real > const* nrms,
                                         std::vector< Rendbuf< Real > const* > const& uvs,
                                         Rendbuf< Real > const* clrs,
                                         Rendbuf< RIdx > const* tris) const
{
	HppAssert(poss, "Null pointer to buffer!");
	HppAssert(nrms, "Null pointer to buffer!");
	HppAssert(uvs.size() < 1 || uvs[0], "Null pointer to buffer!");
	HppAssert(uvs.size() < 2 || uvs[1], "Null pointer to buffer!");
	HppAssert(uvs.size() < 3 || uvs[2], "Null pointer to buffer!");
	HppAssert(tris, "Null pointer to buffer!");

	// Parameters that are not used
	(void)renderable;
// TODO: Code support for colors!
(void)clrs;

	HppCheckGlErrors();

	if (tris->empty()) {
		return;
	}

	// Positions, normals and colors of vertices
	poss->use(RendbufEnums::VERTEX);
	nrms->use(RendbufEnums::NORMAL);
	if (needs_uvs && uvs.size() >= 3) {
		GlSystem::ActiveTexture(GL_TEXTURE2);
		GlSystem::ClientActiveTexture(GL_TEXTURE2);
		uvs[2]->use(RendbufEnums::TEXCOORD, 3);
		GlSystem::ActiveTexture(GL_TEXTURE1);
		GlSystem::ClientActiveTexture(GL_TEXTURE1);
		uvs[1]->use(RendbufEnums::TEXCOORD, 3);
		GlSystem::ActiveTexture(GL_TEXTURE0);
		GlSystem::ClientActiveTexture(GL_TEXTURE0);
		uvs[0]->use(RendbufEnums::TEXCOORD);
	}
	HppCheckGlErrors();

	// Draw triangles
	tris->draw(RendbufEnums::TRIANGLES);
	HppCheckGlErrors();

}

}

#endif
