#ifndef HPP_RENDERINGENVIRONMENT_H
#define HPP_RENDERINGENVIRONMENT_H

#include "color.h"
#include "lightsource.h"
#include "renderable.h"

namespace Hpp
{

class Renderingenvironment
{

public:

	typedef uint8_t Flags;
	static Flags const MULTIPLE_LIGHTS_PER_PASS = 0x01;

	inline Renderingenvironment(void);
	inline ~Renderingenvironment(void);

	inline void setAmbientLight(Hpp::Color const& ambient);

	inline void render(Renderable* renderable);

	// Lighting control
	inline void initMultipassLighting(Lightsources const& lights, Flags flags = 0);
	inline bool continueMultipassLighting(void);
	inline void finishMultipassLighting(void);

private:

	// Rendering options
	bool multiple_lights_per_pass;

	Hpp::Color ambient;

	// Possible container of lights and beginning of lights that will be
	// enabled next.
	Lightsources const* lights;
	size_t lights_usage;
	size_t lights_enabled;

	// Material flags
	Material::Flags mat_flags;

	// Initialized material
	Material const* mat_init;
	Material::Flags mat_init_flags;

};

inline Renderingenvironment::Renderingenvironment(void) :
multiple_lights_per_pass(false),
ambient(0.0, 0.0, 0.0),
lights(NULL),
lights_usage(0),
lights_enabled(0),
mat_flags(0),
mat_init(NULL),
mat_init_flags(0)
{
}

inline Renderingenvironment::~Renderingenvironment(void)
{
	if (mat_init) {
		mat_init->deinitRendering();
	}
}

inline void Renderingenvironment::setAmbientLight(Hpp::Color const& ambient)
{
	this->ambient = ambient;
}

inline void Renderingenvironment::render(Renderable* renderable)
{
	Material const* mat = renderable->getMaterial();
	if (!mat) {
		return;
	}

	// If different material is currently initialized, then change this
	if (mat != mat_init || mat_init_flags != mat_flags) {
		// Deinitialize old
		if (mat_init) {
			mat_init->deinitRendering();
		}
		// Initialize new
		mat->initRendering(lights_enabled, mat_flags);
		mat_init = mat;
		mat_init_flags = mat_flags;
	}

	// Do rendering. This is done using method of Renderable, because data
	// needs to be extracted to Rendbufs, etc.
	renderable->render(mat);
}

inline void Renderingenvironment::initMultipassLighting(Lightsources const& lights, Flags flags)
{
	this->lights = &lights;
	HppAssert(lights_usage == 0, "Lights usage is not finished from previous rendering session!");
	glEnable(GL_LIGHTING);

	// Extract flags
	bool multiple_lights_per_pass = flags & MULTIPLE_LIGHTS_PER_PASS;

	if (!multiple_lights_per_pass) {
		// Disable all but first light
		glEnable(GL_LIGHT0);
		size_t max_lights = GlSystem::getMaxNumOfLights();
		for (size_t lights_id = 1; lights_id < max_lights; lights_id ++) {
			glDisable(GL_LIGHT0 + lights_id);
		}
		// This will not be changed, so it can be set now
		lights_enabled = 1;
	}
}

inline bool Renderingenvironment::continueMultipassLighting(void)
{
	bool first_pass = (lights_usage == 0);

	// Stop lighting if enough lights have been passed
	if (lights->empty()) {
		if (lights_usage > 0) {
			return false;
		}
	} else {
		if (lights_usage >= lights->size()) {
			return false;
		}
	}

	// Reset material flags
	mat_flags = 0;

	// Set some flags
	if (!first_pass) mat_flags |= Material::ADDITIVE_RENDERING;

	// Set up ambient light
	float ambient_buf[4];
	if (first_pass) {
		ambient_buf[0] = ambient.red;
		ambient_buf[1] = ambient.green;
		ambient_buf[2] = ambient.blue;
		ambient_buf[3] = 1.0;
		mat_flags |= Material::AMBIENT_ENABLED;
	} else {
		ambient_buf[0] = 0.0;
		ambient_buf[1] = 0.0;
		ambient_buf[2] = 0.0;
		ambient_buf[3] = 0.0;
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_buf);

	// If there is really no lights, then leave now
	if (lights->empty()) {
		glDisable(GL_LIGHT0);
		lights_enabled = 0;
		lights_usage ++;
		return true;
	}

	if (multiple_lights_per_pass) {
		// Set up lights
		size_t max_lights = GlSystem::getMaxNumOfLights();
		size_t lights_id = 0;
		lights_enabled = 0;
		while (lights_id < max_lights && lights_usage < lights->size()) {

			(*lights)[lights_usage]->setUpGlLight(GL_LIGHT0 + lights_id);

			// Advance to next light
			lights_id ++;
			lights_usage ++;
			lights_enabled ++;
		}
		// Disable rest of lights
		while (lights_id < max_lights) {
			glDisable(GL_LIGHT0 + lights_id);
			lights_id ++;
		}
	} else {
		// Set up light
		(*lights)[lights_usage]->setUpGlLight(GL_LIGHT0);
		lights_usage ++;
	}

	return true;
}

inline void Renderingenvironment::finishMultipassLighting(void)
{
	lights = NULL;
	lights_usage = 0;
	glDisable(GL_LIGHTING);
}

}

#endif
