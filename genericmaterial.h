#ifndef HPP_GENERICMATERIAL_H
#define HPP_GENERICMATERIAL_H

#include "3dconversions.h"
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
	inline GenericMaterial(void);
	inline virtual ~GenericMaterial(void);

	inline void setColor(Color const& color);
	inline void setColormap(Texture* cmap);
	inline void setNormalmap(Texture* nmap);
	inline void setSpecularmap(Texture* smap);
	inline void setShadeless(bool shadeless);

	inline Color getColor(void) const { return color; }
	inline Texture* getColormap(void) const { return colormap; }
	inline Texture* getNormalmap(void) const { return normalmap; }
	inline Texture* getSpecularmap(void) const { return specularmap; }
	inline bool getShadeless(void) const { return shadeless; }

	inline virtual void beginRendering(Matrix4 const& viewmatrix, Color const& ambient_light = Color(0, 0, 0), Light* light = NULL, bool additive_rendering = false) const;
	inline virtual void endRendering(void) const;

	// Virtual functions, needed by Material
	inline virtual bool needsLight(Renderable const* renderable) const;

// TODO: Remove this temporary solution!
inline Shaderprogram* getProgram(void) { return program; }

private:

	// Functions for friends
	static void initShaders(void);
	static void deinitShaders(void);

private:

	// Some options
	static float const AMBIENT_LIGHT_ON_THRESHOLD;
	static float const NEEDS_LIGHT_THRESHOLD;
	static float const TRANSLUCENT_THRESHOLD;

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

	// Options
	bool twosided;
	bool shadeless;
	bool no_repeat;
	float normalmap_weight;

	// State
	bool needs_light;
	bool is_translucent;
	bool needs_uvs;

	// Class-wide shaders
	static Shader* shader_vrt;
	static Shader* shader_frg;
	static Shaderprogram* program;

	// Virtual functions, needed by Material
	inline virtual void doRendering(Renderable const* renderable,
	                                Rendbuf< Real > const* poss,
	                                Rendbuf< Real > const* nrms,
	                                std::vector< Rendbuf< Real > const* > const& uvs,
	                                Rendbuf< Real > const* clrs,
	                                Rendbuf< RIdx > const* tris) const;

	inline void updateNeedsLight(void);
	inline void updateIsTranslucent(void);

};

inline GenericMaterial::GenericMaterial(Rawmaterial const& rawmat, bool twosided) :
color(rawmat.color),
specular(rawmat.specular),
shininess(rawmat.shininess),
ambient_multiplier(rawmat.ambient_multiplier),
emittance(rawmat.emittance),
twosided(twosided),
shadeless(false),
no_repeat(false),
normalmap_weight(rawmat.normalmap_weight)
{
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

	updateNeedsLight();
	updateIsTranslucent();
}

inline GenericMaterial::GenericMaterial(void) :
color(Color(1, 1, 1)),
specular(Color(0, 0, 0)),
shininess(0),
ambient_multiplier(1),
emittance(0),
colormap(NULL),
normalmap(NULL),
specularmap(NULL),
twosided(true),
shadeless(false),
no_repeat(false),
normalmap_weight(1),
needs_uvs(false)
{
	updateNeedsLight();
	updateIsTranslucent();
}

inline GenericMaterial::~GenericMaterial(void)
{
}

inline void GenericMaterial::setColor(Color const& color)
{
	this->color = color;
	updateNeedsLight();
	updateIsTranslucent();
}

inline void GenericMaterial::setColormap(Texture* cmap)
{
	colormap = cmap;
}

inline void GenericMaterial::setNormalmap(Texture* nmap)
{
	normalmap = nmap;
}

inline void GenericMaterial::setSpecularmap(Texture* smap)
{
	specularmap = smap;
}

inline void GenericMaterial::setShadeless(bool shadeless)
{
	this->shadeless = shadeless;
	updateNeedsLight();
}

inline void GenericMaterial::beginRendering(Matrix4 const& viewmatrix, Color const& ambient_light, Light* light, bool additive_rendering) const
{
// TODO: Implement using of normalmap_weight!
HppAssert(!additive_rendering, "Additive rendering not implemented yet!");
// TODO: Implement additive rendering!
	bool ambient_light_enabled = ambient_light.getRed() > AMBIENT_LIGHT_ON_THRESHOLD || ambient_light.getGreen() > AMBIENT_LIGHT_ON_THRESHOLD || ambient_light.getBlue() > AMBIENT_LIGHT_ON_THRESHOLD;

	HppCheckGlErrors();

	if (is_translucent) {
		HppAssert(!glIsEnabled(GL_BLEND), "Blend should not be enabled now!");
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	HppAssert(glIsEnabled(GL_CULL_FACE), "Face culling is not enabled!");
	if (twosided) {
		glDisable(GL_CULL_FACE);
	}

	Shaderprogram::Flags sflags;

	if (colormap) {
		GlSystem::ActiveTexture(GL_TEXTURE0);
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

	// Initialize shaders
	if (shadeless) {
		sflags.insert("SHADELESS");
	} else {
		if (ambient_light_enabled) {
			sflags.insert("AMBIENT_LIGHT");
		}
		if (light) {
			sflags.insert("LIGHT");
			// Set attenuation light flags
			if (light->getType() == Light::POINT) {
				float attenu_c = light->getConstantAttenuation();
				float attenu_l = light->getLinearAttenuation();
				float attenu_q = light->getQuadraticAttenuation();
				if (attenu_c > 0) sflags.insert("ATTENU_C");
				if (attenu_l > 0) sflags.insert("ATTENU_L");
				if (attenu_q > 0) sflags.insert("ATTENU_Q");
			}
		}
	}

	if (color.getRed() > NEEDS_LIGHT_THRESHOLD || color.getGreen() > NEEDS_LIGHT_THRESHOLD || color.getBlue() > NEEDS_LIGHT_THRESHOLD) {
		sflags.insert("COLOR");
	}
// TODO: Set specular and other values!

	program->enable(sflags);

	if (!shadeless) {
		if (ambient_light_enabled) {
			program->setUniform("ambient_light", ambient_light);
		}
		if (light) {
			if (light->getType() == Light::POINT) {
				float attenu_c = light->getConstantAttenuation();
				float attenu_l = light->getLinearAttenuation();
				float attenu_q = light->getQuadraticAttenuation();
				if (attenu_c > 0) program->setUniform1f("light_constant_attenuation", attenu_c);
				if (attenu_l > 0) program->setUniform1f("light_linear_attenuation", attenu_c);
				if (attenu_q > 0) program->setUniform1f("light_quadratic_attenuation", attenu_c);
				program->setUniform("light_pos_viewspace", viewmatrix * light->getPosition(), 1);
			} else {
				program->setUniform("light_pos_viewspace", matrix4ToMatrix3(viewmatrix) * -light->getDirection(), 0);
			}
			program->setUniform("light_color", light->getColor(), RGB);
		}
	}
	if (color.getRed() > NEEDS_LIGHT_THRESHOLD || color.getGreen() > NEEDS_LIGHT_THRESHOLD || color.getBlue() > NEEDS_LIGHT_THRESHOLD) {
		program->setUniform("material_color", color);
	}
	if (colormap) {
		program->setUniform1i("cmap", 0);
	}
	if (normalmap) {
		program->setUniform1i("nmap", 1);
	}
	if (specularmap) {
		program->setUniform1i("smap", 2);
	}
}

inline void GenericMaterial::endRendering(void) const
{
	program->disable();

	if (normalmap) {
		GlSystem::ActiveTexture(GL_TEXTURE1);
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
		glDisable(GL_TEXTURE_2D);
		HppCheckGlErrors();
		if (no_repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			HppCheckGlErrors();
		}
	}
	GlSystem::ActiveTexture(GL_TEXTURE0);
	if (colormap) {
		glDisable(GL_TEXTURE_2D);
		HppCheckGlErrors();
		if (no_repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			HppCheckGlErrors();
		}
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

inline bool GenericMaterial::needsLight(Renderable const* renderable) const
{
	(void)renderable;
	return needs_light;
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
		GlSystem::ClientActiveTexture(GL_TEXTURE2); //DEPRECATED
		uvs[2]->use(RendbufEnums::TEXCOORD, 3);
		GlSystem::ActiveTexture(GL_TEXTURE1);
		GlSystem::ClientActiveTexture(GL_TEXTURE1); //DEPRECATED
		uvs[1]->use(RendbufEnums::TEXCOORD, 3);
		GlSystem::ActiveTexture(GL_TEXTURE0);
		GlSystem::ClientActiveTexture(GL_TEXTURE0); //DEPRECATED
		uvs[0]->use(RendbufEnums::TEXCOORD);
	}
	HppCheckGlErrors();

	// Draw triangles
	tris->draw(RendbufEnums::TRIANGLES);
	HppCheckGlErrors();

}

inline void GenericMaterial::updateNeedsLight(void)
{
	if (color.getRed() > NEEDS_LIGHT_THRESHOLD ||
	    color.getGreen() > NEEDS_LIGHT_THRESHOLD ||
	    color.getBlue() > NEEDS_LIGHT_THRESHOLD ||
	    specular.getRed() > NEEDS_LIGHT_THRESHOLD ||
	    specular.getGreen() > NEEDS_LIGHT_THRESHOLD ||
	    specular.getBlue() > NEEDS_LIGHT_THRESHOLD) {
	    	needs_light = true;
	} else {
		needs_light = false;
	}
}

inline void GenericMaterial::updateIsTranslucent(void)
{
	if (color.getAlpha() <= TRANSLUCENT_THRESHOLD) {
		is_translucent = true;
	} else {
		is_translucent = false;
	}
}

}

#endif
