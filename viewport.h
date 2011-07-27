#ifndef HPP_VIEWPORT_H
#define HPP_VIEWPORT_H

#include "texture.h"
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

	inline uint32_t getPositionX(void) const { return x; }
	inline uint32_t getPositionY(void) const { return y; }
	inline uint32_t getWidth(void) const { return width; }
	inline uint32_t getHeight(void) const { return height; }
	inline Color getBgcolor(void) const { return bg_color; }
	inline bool getTransparent(void) const { return bg_transparent; }

	inline void clearBackground(void);

	// Renders some visibles with some lights using specific camera
	inline void renderVisibles(Camera* cam, Visibles& visibles, Lightsources const& lights = Lightsources(), Color const& ambient_light = Color(0.2, 0.2, 0.2)) const;

	// 2D rendering stuff
	inline void init2DRendering(void) const;
	inline void deinit2DRendering(void) const;
	inline void renderSprite(Texture const& tex,
	                         Vector2 const& pos,
	                         Vector2 const& size,
	                         Vector2 const& tex_pos = Vector2(0, 0),
	                         Vector2 const& tex_size = Vector2(1, 1),
	                         Angle const& rotation = Angle::fromRadians(0),
	                         Vector2 const& rot_pos = Vector2(0, 0)) const;

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

inline void Viewport::clearBackground(void)
{
	// Set viewport
	HppCheckForCorrectThread();
	glViewport(x, y, width, height);
	// Clear
	if (!bg_transparent) {
		glClearColor(bg_color.getRed(), bg_color.getGreen(), bg_color.getBlue(), 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

inline void Viewport::renderVisibles(Camera* cam, Visibles& visibles, Lightsources const& lights, Color const& ambient_light) const
{
	// Set viewport
	HppCheckForCorrectThread();
	glViewport(x, y, width, height);

	// Set some GL things
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);

	// Clear z buffer
	HppCheckForCorrectThread();
	glClear(GL_DEPTH_BUFFER_BIT);

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

inline void Viewport::init2DRendering(void) const
{
	// Set viewport
	HppCheckForCorrectThread();
	glViewport(x, y, width, height);

	// Set some GL things
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	// Tune blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Prepare projection
	HppCheckGlErrors();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(x, x + width, y, y + height, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	HppCheckGlErrors();

}

inline void Viewport::deinit2DRendering(void) const
{
	// Set some GL things
	glDisable(GL_TEXTURE_2D);
}

inline void Viewport::renderSprite(Texture const& tex,
                                   Vector2 const& pos,
                                   Vector2 const& size,
                                   Vector2 const& tex_pos,
                                   Vector2 const& tex_size,
                                   Angle const& rotation,
                                   Vector2 const& rot_pos) const
{
// TODO: Use rotation!
(void)rotation;
(void)rot_pos;
	HppCheckForCorrectThread();

	// Bind texture
	glBindTexture(GL_TEXTURE_2D, tex.getGlTexture());

	HppCheckGlErrors();
	glBegin(GL_QUADS);

	glTexCoord2f(tex_pos.x, tex_pos.y);
	glVertex2f(pos.x, pos.y);

	glTexCoord2f(tex_pos.x + tex_size.x, tex_pos.y);
	glVertex2f(pos.x + size.x, pos.y);

	glTexCoord2f(tex_pos.x + tex_size.x, tex_pos.y + tex_size.y);
	glVertex2f(pos.x + size.x, pos.y + size.y);

	glTexCoord2f(tex_pos.x, tex_pos.y + tex_size.y);
	glVertex2f(pos.x, pos.y + size.y);

	glEnd();
	HppCheckGlErrors();
}

}

#endif
