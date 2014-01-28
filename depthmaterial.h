#ifndef HPP_DEPTHMATERIAL_H
#define HPP_DEPTHMATERIAL_H

#include "texture.h"
#include "path.h"
#include "json.h"
#include "material.h"
#include "mesh.h"
#include "shaderprogram.h"
#include "shaderprogramhandle.h"

namespace Hpp
{

class Depthmaterial : public Material
{

friend class Display;

public:

	typedef uint8_t Flags;
	static Flags const RADIAL_DEPTH = 0x01;
	static Flags const USE_COLOR = 0x02;
	static Flags const PACK_COLOR = 0x04;
	static Flags const TWOSIDED = 0x08;

	inline Depthmaterial(Real nearplane, Real farplane, Flags flags = 0, Texture* alphamask = NULL);
	inline Depthmaterial(Path const& path, Texture* alphamask);

	// This can be used to get name of possible texture, that this
	// material needs. Returns true, if material needs texture.
	inline static bool getNeededTexture(std::string& result, Path const& path);

	inline void setNearAndFar(Real nearplane, Real farplane);

	inline virtual void setViewmatrix(Matrix4 const& viewmatrix);
	inline virtual void setProjectionmatrix(Matrix4 const& projectionmatrix);
	inline virtual void renderMesh(Mesh const* mesh, Transform const& transf);
	inline virtual void beginRendering(Color const& ambient_light = Color(0, 0, 0), Light const* light = NULL, bool additive_rendering = false) const;
	inline virtual void endRendering(void) const;

// Functions for friends
private:

	static void initShaders(void);
	static void deinitShaders(void);

private:

	// Locations of all uniforms and vertex attributes
	static uint16_t const UNIF_PMAT = 0;
	static uint16_t const UNIF_MVMAT = 1;
	static uint16_t const UNIF_NEAR = 2;
	static uint16_t const UNIF_FAR = 3;
	static uint16_t const UNIF_ALPHAMASK = 4;

	static uint16_t const VATR_POS = 0;
	static uint16_t const VATR_UV = 1;

	// Options
	Real nearplane;
	Real farplane;
	bool radial_depth;
	bool use_color;
	bool pack_color;
	bool twosided;
	Texture* alphamask;

	// Rendering state
	mutable Shaderprogramhandle* rendering_programhandle;
	mutable Matrix4 rendering_viewmatrix;

	// Class-wide shaders
	static Shaderprogram* program;

};

inline Depthmaterial::Depthmaterial(Real near, Real far, Flags flags, Texture* alphamask) :
nearplane(near),
farplane(far),
radial_depth(flags & RADIAL_DEPTH),
use_color(flags & USE_COLOR),
pack_color(flags & PACK_COLOR),
twosided(flags & TWOSIDED),
alphamask(alphamask),
rendering_programhandle(NULL)
{
	if (pack_color && !use_color) {
		throw Exception("You cannot pack colors if you are not using them!");
	}
}

inline Depthmaterial::Depthmaterial(Path const& path, Texture* alphamask) :
nearplane(0),
farplane(0),
radial_depth(false),
use_color(false),
pack_color(false),
twosided(false),
alphamask(NULL),
rendering_programhandle(NULL)
{
	Json json(path.readString());

	if (json.keyExists("radial depth")) {
		radial_depth = json.getMember("radial depth").getBoolean();
	}

	if (json.keyExists("use color")) {
		use_color = json.getMember("use color").getBoolean();
	}

	if (json.keyExists("pack color")) {
		pack_color = json.getMember("pack color").getBoolean();
	}

	if (json.keyExists("twosided")) {
		twosided = json.getMember("twosided").getBoolean();
	}

	// Mask
	if (json.keyExists("alphamask")) {
		if (!alphamask) {
			throw Exception("Depthmaterial needs alphamask texture, but it is not provided!");
		}
		this->alphamask = alphamask;
	} else if (alphamask) {
		throw Exception("Depthmaterial does not need alphamask texture, but it is still provided!");
	}

}

bool Depthmaterial::getNeededTexture(std::string& result, Path const& path)
{
	Json json(path.readString());
	if (json.keyExists("alphamask")) {
		result = json.getMember("alphamask").getString();
		return true;
	}
	return false;
}

inline void Depthmaterial::setNearAndFar(Real near, Real far)
{
	nearplane = near;
	farplane = far;
}

inline void Depthmaterial::setViewmatrix(Matrix4 const& viewmatrix)
{
	rendering_viewmatrix = viewmatrix;
}

inline void Depthmaterial::setProjectionmatrix(Matrix4 const& projectionmatrix)
{
	rendering_programhandle->setUniform(UNIF_PMAT, projectionmatrix, true);
}

inline void Depthmaterial::renderMesh(Mesh const* mesh, Transform const& transf)
{
	// Position buffer is needed always.
	rendering_programhandle->setBufferobject(VATR_POS, mesh->getBuffer(Mesh::POS));
	HppCheckGlErrors();

	// UV buffer is needed only, if there is an alphamask.
	if (alphamask) {
		rendering_programhandle->setBufferobject(VATR_UV, mesh->getBuffer(Mesh::UV));
		HppCheckGlErrors();
	}

	// Calculate and set ModelViewMatrix
	Matrix4 mvmat = rendering_viewmatrix * transf.getMatrix();
	rendering_programhandle->setUniform(UNIF_MVMAT, mvmat, true);
	rendering_programhandle->setUniform1f(UNIF_NEAR, nearplane);
	rendering_programhandle->setUniform1f(UNIF_FAR, farplane);

	// Render
	mesh->getBuffer(Mesh::INDEX)->drawElements(GL_TRIANGLES);
}

inline void Depthmaterial::beginRendering(Color const& ambient_light, Light const* light, bool additive_rendering) const
{
	(void)ambient_light;
	(void)light;
	(void)additive_rendering;
	HppAssert(!additive_rendering, "Additive rendering not supported!");

	HppCheckGlErrors();

	HppAssert(glIsEnabled(GL_CULL_FACE), "Face culling is not enabled!");
	if (twosided) {
		glDisable(GL_CULL_FACE);
	}

	Shaderprogram::Flags sflags;

	// If there is alpha mask, then bind it and enable appropriate shaderflag.
	if (alphamask) {
		alphamask->bind();
		HppCheckGlErrors();
		sflags.insert("ALPHAMASK");
	}

	if (radial_depth) sflags.insert("RADIAL");
	if (use_color) sflags.insert("COLOR");
	if (pack_color) sflags.insert("PACK");
	rendering_programhandle = program->createHandle(sflags);
// TODO: Optimize this!
	rendering_programhandle->enable();

	// Set alphamask as uniform, if it exists
	if (alphamask) {
		rendering_programhandle->setUniform1i(UNIF_ALPHAMASK, alphamask->getBoundTextureunit());
	}
}

inline void Depthmaterial::endRendering(void) const
{
	rendering_programhandle->disable();
	delete rendering_programhandle;
	rendering_programhandle = NULL;
	HppCheckGlErrors();

	// If there is alpha mask, then unbind it
	if (alphamask) {
		alphamask->unbind();
		HppCheckGlErrors();
	}

	if (twosided) {
		glEnable(GL_CULL_FACE);
		HppCheckGlErrors();
	}
}

}

#endif
