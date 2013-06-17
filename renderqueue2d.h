#ifndef HPP_RENDERQUEUE2D
#define HPP_RENDERQUEUE2D

#include "transform2d.h"
#include "shaderprogram.h"
#include "noncopyable.h"
#include "vector2.h"
#include "color.h"
#include "texture.h"
#include "display.h"
#include "exception.h"

#include <stdint.h>
#include <vector>

namespace Hpp
{

class Renderqueue2d : public NonCopyable
{

public:

	inline Renderqueue2d(uint32_t x = 0, uint32_t y = 0, uint32_t width = 0, uint32_t height = 0);
	inline ~Renderqueue2d(void);

	inline void begin(void);
	inline void end();

	inline uint32_t getWidth(void) const { return width; }
	inline uint32_t getHeight(void) const { return height; }
	inline uint32_t getPositionX(void) const { return x; }
	inline uint32_t getPositionY(void) const { return y; }

	inline void renderSprite(Texture const* tex,
	                         Vector2 const& pos,
	                         Vector2 const& size,
	                         Vector2 const& tex_pos = Vector2(0, 0),
	                         Vector2 const& tex_size = Vector2(1, 1),
	                         Color const& color = Color(1, 1, 1),
	                         Angle const& rotation = Angle::fromRadians(0),
	                         Vector2 const& rot_pos = Vector2(0, 0));

	inline void flush(void);

private:

	uint32_t x, y;
	uint32_t width, height;

	Shaderprogram program;

	bool rendering_started;
	Texture const* active_texture;
	bool rgb_forced_to_one;

	std::vector< GLfloat > poss;
	std::vector< GLfloat > uvs;
	std::vector< GLfloat > clrs;

	static std::string const SHADER_VRT;
	static std::string const SHADER_FRG;

	// This enables program and sets appropriate flags, based on "rgb_forced_to_one".
	inline void enableProgram(void);

};

inline Renderqueue2d::Renderqueue2d(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
x(x), y(y),
width(width), height(height),
rendering_started(false),
rgb_forced_to_one(false)
{
	// Initialize shaderprogram
	Shader shader_vrt;
	shader_vrt.load(SHADER_VRT, Shader::VERTEX_SHADER);
	Shader shader_frg;
	shader_frg.load(SHADER_FRG, Shader::FRAGMENT_SHADER);
	program.attachShader(shader_vrt);
	program.attachShader(shader_frg);
	// Set size
	if (this->width == 0 && Display::getWidth() > x) {
		this->width = Display::getWidth() - x;
	}
	if (this->height == 0 && Display::getHeight() > y) {
		this->height = Display::getHeight() - y;
	}
}

inline Renderqueue2d::~Renderqueue2d(void)
{
	if (rendering_started) {
		end();
	}
}

inline void Renderqueue2d::begin(void)
{
	if (rendering_started) {
		throw Exception("Rendering has already been started!");
	}
	// Set some values
	rendering_started = true;
	active_texture = NULL;
	// Initialize OpenGL for 2D rendering
	glViewport(x, y, width, height);
	// Set some GL things
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Tune blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Display::pushScissor(x, y, width, height);

	HppCheckGlErrors();

	enableProgram();

	// Prepare projection
	HppCheckGlErrors();

	// Form viewmat
	Matrix3 viewmat;
	viewmat = Matrix3::translMatrix(Vector2(-1, -1));
	viewmat *= Matrix3::scaleMatrix(Vector2(2.0 / width, 2.0 / height));
	program.setUniform("viewmat", viewmat, true);

	HppCheckGlErrors();

}

inline void Renderqueue2d::end()
{
	if (!rendering_started) {
		throw Exception("Rendering has not been started!");
	}
	flush();
	if (active_texture) {
		active_texture->unbind();
	}
	rendering_started = false;
	// Restore Opengl stuff
	program.disable();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glViewport(0, 0, Display::getWidth(), Display::getHeight());

	Display::popScissor();
}

inline void Renderqueue2d::renderSprite(Texture const* tex,
                                        Vector2 const& pos,
                                        Vector2 const& size,
                                        Vector2 const& tex_pos,
                                        Vector2 const& tex_size,
                                        Color const& color,
                                        Angle const& rotation,
                                        Vector2 const& rot_pos)
{
// TODO: Use "color"!
(void)color;
	if (tex != active_texture) {
		flush();
		if (active_texture) {
			active_texture->unbind();
		}
		active_texture = tex;
		// Bind texture to OpenGL
		HppCheckGlErrors();
		tex->bind();
		program.setUniform1i("tex", tex->getBoundTextureunit());
		HppCheckGlErrors();
	}

	// If this texture is in GL_ALPHA format, then
	// ensure RGB values are forced to one
	if (tex->getGlFormat() == GL_ALPHA) {
		if (!rgb_forced_to_one) {
			flush();
			program.disable();
			rgb_forced_to_one = true;
			enableProgram();
		}
	}
	// If this texture is not in GL_ALPHA format, then
	// ensure RGB values are not forced to one
	else {
		if (rgb_forced_to_one) {
			flush();
			program.disable();
			rgb_forced_to_one = false;
			enableProgram();
		}
	}

	HppCheckForCorrectThread();

	Transform2D transf;
	transf.scale(size);
	if (rotation != Angle(0)) {
		transf.translate(-rot_pos);
		transf.rotate(rotation);
		transf.translate(rot_pos);
	}
	transf.translate(pos);

	Vector2 v0 = transf.applyToPosition(Vector2(0, 0));
	Vector2 v1 = transf.applyToPosition(Vector2(1, 0));
	Vector2 v2 = transf.applyToPosition(Vector2(1, 1));
	Vector2 v3 = transf.applyToPosition(Vector2(0, 1));

	GLfloat r = color.getRed();
	GLfloat g = color.getGreen();
	GLfloat b = color.getBlue();
	GLfloat a = color.getAlpha();

	// Vertex #0
	poss.push_back(v0.x);
	poss.push_back(v0.y);
	uvs.push_back(tex_pos.x);
	uvs.push_back(tex_pos.y);
	clrs.push_back(r);
	clrs.push_back(g);
	clrs.push_back(b);
	clrs.push_back(a);

	// Vertex #1
	poss.push_back(v1.x);
	poss.push_back(v1.y);
	uvs.push_back(tex_pos.x + tex_size.x);
	uvs.push_back(tex_pos.y);
	clrs.push_back(r);
	clrs.push_back(g);
	clrs.push_back(b);
	clrs.push_back(a);

	// Vertex #3
	poss.push_back(v3.x);
	poss.push_back(v3.y);
	uvs.push_back(tex_pos.x);
	uvs.push_back(tex_pos.y + tex_size.y);
	clrs.push_back(r);
	clrs.push_back(g);
	clrs.push_back(b);
	clrs.push_back(a);

	// Vertex #1
	poss.push_back(v1.x);
	poss.push_back(v1.y);
	uvs.push_back(tex_pos.x + tex_size.x);
	uvs.push_back(tex_pos.y);
	clrs.push_back(r);
	clrs.push_back(g);
	clrs.push_back(b);
	clrs.push_back(a);

	// Vertex #2
	poss.push_back(v2.x);
	poss.push_back(v2.y);
	uvs.push_back(tex_pos.x + tex_size.x);
	uvs.push_back(tex_pos.y + tex_size.y);
	clrs.push_back(r);
	clrs.push_back(g);
	clrs.push_back(b);
	clrs.push_back(a);

	// Vertex #3
	poss.push_back(v3.x);
	poss.push_back(v3.y);
	uvs.push_back(tex_pos.x);
	uvs.push_back(tex_pos.y + tex_size.y);
	clrs.push_back(r);
	clrs.push_back(g);
	clrs.push_back(b);
	clrs.push_back(a);

}

inline void Renderqueue2d::flush(void)
{
	HppAssert(poss.size() == uvs.size(), "Buffers are not sync!");
	HppAssert(poss.size() * 2 == clrs.size(), "Buffers are not sync!");
	if (poss.empty()) {
		return;
	}

	// Make bufferobjects
	Bufferobject poss_bo(GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 2, &poss[0], poss.size());
	Bufferobject uvs_bo(GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 2, &uvs[0], uvs.size());
	Bufferobject clrs_bo(GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 4, &clrs[0], clrs.size());

	program.setBufferobject("pos", &poss_bo);
	program.setBufferobject("uv", &uvs_bo);
	program.setBufferobject("clr", &clrs_bo);

	glDrawArrays(GL_TRIANGLES, 0, poss.size() / 2);

	poss.clear();
	uvs.clear();
	clrs.clear();
}

inline void Renderqueue2d::enableProgram(void)
{
	Hpp::Shaderprogram::Flags flags;
	if (rgb_forced_to_one) {
		flags.insert("FORCE_RGB_TO_ONE");
	}
	program.enable(flags);
}

}

#endif
