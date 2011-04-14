#ifndef HPP_VIEWPORT_H
#define HPP_VIEWPORT_H

#include "renderingenvironment.h"
#include "camera.h"
#include "lightsource.h"
#include "visibles.h"
#include "color.h"

namespace Hpp
{

class Viewport
{

public:

	inline Viewport(void);
	inline Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	inline void setPosition(uint32_t x, uint32_t y) { this->x = x; this->y = y; }
	inline void setSize(uint32_t width, uint32_t height) { this->width = width; this->height = height; }
	inline void setBgcolor(Color const& color) { bg_color = color; }
	inline void setTransparent(bool transparent = true) { bg_transparent = transparent; }

	// Renders some visibles with some lights using specific camera
	inline void renderVisibles(Camera* cam, Visibles& visibles, Lightsources const& lights = Lightsources(), Color const& ambient_light = Color(0.2, 0.2, 0.2)) const;

private:

	uint32_t x, y;
	uint32_t width, height;

	Color bg_color;
	bool bg_transparent;

};

inline Viewport::Viewport(void) :
bg_color(0, 0, 0),
bg_transparent(false)
{
}

inline Viewport::Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
x(x),
y(y),
width(width),
height(height),
bg_color(0, 0, 0),
bg_transparent(false)
{
}

inline void Viewport::renderVisibles(Camera* cam, Visibles& visibles, Lightsources const& lights, Color const& ambient_light) const
{
	// Set viewport
	HppCheckForCorrectThread();
	glViewport(x, y, width, height);

	// Clear z buffer
	HppCheckForCorrectThread();
	if (bg_transparent) {
		glClear(GL_DEPTH_BUFFER_BIT);
	} else {
		glClearColor(bg_color.getRed(), bg_color.getGreen(), bg_color.getBlue(), 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}
	HppCheckForCorrectThread();

	cam->prepareProjection(width, height);

	Renderingenvironment renv;
	renv.setAmbientLight(ambient_light);

	// Renderables without need for lighting.
	for (Renderables::iterator renderables_it = visibles.no_light.begin();
	     renderables_it != visibles.no_light.end();
	     renderables_it ++) {
		Renderable* renderable = *renderables_it;
		renv.render(renderable);
	}

	// Renderables with need for lighting
	renv.initMultipassLighting(lights);
	while (renv.continueMultipassLighting()) {
		for (Renderables::iterator renderables_it = visibles.normal.begin();
		     renderables_it != visibles.normal.end();
		     renderables_it ++) {
			Renderable* renderable = *renderables_it;
			renv.render(renderable);
		}
	}
	renv.finishMultipassLighting();

}

}

#endif
