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

	std::vector< GLfloat > poss;
	std::vector< GLfloat > uvs;

	static std::string const SHADER_VRT;
	static std::string const SHADER_FRG;

};

inline Renderqueue2d::Renderqueue2d(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
x(x), y(y),
width(width), height(height),
rendering_started(false)
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
	glEnable(GL_TEXTURE_2D);

	// Tune blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	program.enable();
	program.setUniform1i("tex", 0);

	// Prepare projection
	HppCheckGlErrors();

	// Form viewmat
	Matrix3 viewmat;
	viewmat = Matrix3::translMatrix(Hpp::Vector2(-1, -1));
	viewmat *= Matrix3::scaleMatrix(Hpp::Vector2(2.0 / width, 2.0 / height));
	program.setUniform("viewmat", viewmat, true);

	HppCheckGlErrors();

	Display::pushScissor(x, y, width, height);

	HppCheckGlErrors();

}

inline void Renderqueue2d::end()
{
	if (!rendering_started) {
		throw Exception("Rendering has not been started!");
	}
	flush();
	rendering_started = false;
	// Restore Opengl stuff
	program.disable();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
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
	if (tex != active_texture) {
		flush();
		active_texture = tex;
		// Bind texture to OpenGL
		HppCheckGlErrors();
		glBindTexture(GL_TEXTURE_2D, tex->getGlTexture());
		HppCheckGlErrors();
	}

	HppCheckForCorrectThread();

	Transform2D transf;
	transf.scale(size);
	if (rotation != Angle::fromDegrees(0)) {
		transf.translate(-rot_pos);
		transf.rotate(rotation);
		transf.translate(rot_pos);
	}
	transf.translate(pos);

	Vector2 v0 = transf.applyToPosition(Vector2(0, 0));
	Vector2 v1 = transf.applyToPosition(Vector2(1, 0));
	Vector2 v2 = transf.applyToPosition(Vector2(1, 1));
	Vector2 v3 = transf.applyToPosition(Vector2(0, 1));

	// Vertex #0
	poss.push_back(v0.x);
	poss.push_back(v0.y);
	uvs.push_back(tex_pos.x);
	uvs.push_back(tex_pos.y);

	// Vertex #1
	poss.push_back(v1.x);
	poss.push_back(v1.y);
	uvs.push_back(tex_pos.x + tex_size.x);
	uvs.push_back(tex_pos.y);

	// Vertex #3
	poss.push_back(v3.x);
	poss.push_back(v3.y);
	uvs.push_back(tex_pos.x);
	uvs.push_back(tex_pos.y + tex_size.y);

	// Vertex #1
	poss.push_back(v1.x);
	poss.push_back(v1.y);
	uvs.push_back(tex_pos.x + tex_size.x);
	uvs.push_back(tex_pos.y);

	// Vertex #2
	poss.push_back(v2.x);
	poss.push_back(v2.y);
	uvs.push_back(tex_pos.x + tex_size.x);
	uvs.push_back(tex_pos.y + tex_size.y);

	// Vertex #3
	poss.push_back(v3.x);
	poss.push_back(v3.y);
	uvs.push_back(tex_pos.x);
	uvs.push_back(tex_pos.y + tex_size.y);

}

inline void Renderqueue2d::flush(void)
{
	HppAssert(poss.size() == uvs.size(), "Buffers are not sync!");
	if (poss.empty()) {
		return;
	}

	// Make bufferobjects
	Bufferobject poss_bo(GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 2, &poss[0], poss.size());
	Bufferobject uvs_bo(GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 2, &uvs[0], uvs.size());

	program.setBufferobject("pos", &poss_bo);
	program.setBufferobject("uv", &uvs_bo);

	glDrawArrays(GL_TRIANGLES, 0, poss.size() / 2);

	poss.clear();
	uvs.clear();
}

}

#endif
