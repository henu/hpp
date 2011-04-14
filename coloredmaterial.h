#ifndef HPP_COLOREDMATERIAL_H
#define HPP_COLOREDMATERIAL_H

#include "rawmaterial.h"
#include "color.h"
#include "material.h"
#include "gldebug.h"
#include "inc_gl.h"
#include "assert.h"

namespace Hpp
{

class ColoredMaterial : public Material
{

public:

	// Constructor and destructor
	inline ColoredMaterial(Rawmaterial const& rawmat, bool twosided = false);
	inline virtual ~ColoredMaterial(void);

	// Virtual functions, needed by Material
	inline virtual bool needsLight(Renderable const* renderable) const;

private:

	// The color and alpha of Material
	Color color;
	Color specular;
	float shininess;
	float ambient_multiplier;
	float emittance;

	bool twosided;
	bool needs_light;
	bool is_translucent;

	// Virtual functions, needed by Material
	inline virtual void initRendering(size_t num_of_lights, Flags flags) const;
	inline virtual void deinitRendering(void) const;
	inline virtual void doRendering(Renderable const* renderable,
	                                Rendbuf< Real > const& poss,
	                                Rendbuf< Real > const& nrms,
	                                std::vector< Rendbuf< Real > > const& uvs,
	                                Rendbuf< Real > const& clrs,
	                                Rendbuf< RIdx > const& tris) const;

};

inline ColoredMaterial::ColoredMaterial(Rawmaterial const& rawmat, bool twosided) :
color(rawmat.color),
specular(rawmat.specular),
shininess(rawmat.shininess),
ambient_multiplier(rawmat.ambient_multiplier),
emittance(rawmat.emittance),
twosided(twosided)
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
}

inline ColoredMaterial::~ColoredMaterial(void)
{
}

inline bool ColoredMaterial::needsLight(Renderable const* renderable) const
{
	(void)renderable;
	return needs_light;
}

inline void ColoredMaterial::initRendering(size_t num_of_lights, Flags flags) const
{
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
}

inline void ColoredMaterial::deinitRendering(void) const
{
	// Disable arrays
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
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

inline void ColoredMaterial::doRendering(Renderable const* renderable,
                                         Rendbuf< Real > const* poss,
                                         Rendbuf< Real > const* nrms,
                                         std::vector< Rendbuf< Real > > const* uvs,
                                         Rendbuf< Real > const* clrs,
                                         Rendbuf< RIdx > const* tris) const
{

	// Parameters that are not used
	(void)renderable;
	(void)uvs;
// TODO: Code support for colors!
(void)clrs;

	HppCheckGlErrors();

	if (tris.empty()) {
		return;
	}

	// Positions, normals and colors of vertices
	poss->use(RendbufEnums::VERTEX);
	nrms->use(RendbufEnums::NORMAL);
	HppCheckGlErrors();

	// Draw triangles
	tris->draw(RendbufEnums::TRIANGLES);
	HppCheckGlErrors();

}

}

#endif
