#ifndef HPP_MATERIAL_H
#define HPP_MATERIAL_H

#include "light.h"
#include "matrix4.h"
#include "transform.h"
#include "glsystem.h"
#include "gldebug.h"
#include "rendbuf.h"
#include "renderindex.h"

#include <iterator>
#include <vector>

namespace Hpp
{

// Classes that doesn't need their headers included for now
class Renderable;

class Material
{

	// Friends
	friend class Renderingenvironment;
	friend class Renderable;

public:

	// Flags for rendering state
	typedef uint8_t Flags;
	static Flags const ADDITIVE_RENDERING	= 0x01;
	static Flags const AMBIENT_ENABLED	= 0x02;

	// Different types of alpha rendering. Types are numbered so they can
	// be sorted based on the complexity of alpha renderin type.
// TODO: Use these in future!
	enum Alphatype { NO_ALPHA      = 0,
	                 NO_DEPTH_SORT = 1,
	                 DEPTH_SORT    = 2,
	                 BSP_TREE      = 3};

	// Constructor and destructor
	inline Material(void);
	inline virtual ~Material(void);

	// Begins/ends manual rendering
	virtual void beginRendering(Matrix4 const& viewmatrix, Color const& ambient_light = Color(0, 0, 0), Light const* light = NULL, bool additive_rendering = false) const = 0;
	virtual void endRendering(void) const = 0;

	virtual bool needsLight(Renderable const* renderable) const { (void)renderable; return true; }

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	inline void render(Renderable const* renderable,
	                   Rendbuf< Real > const* poss,
	                   Rendbuf< Real > const* nrms,
	                   std::vector< Rendbuf< Real > const* > const& uvs,
	                   Rendbuf< Real > const* clrs,
	                   Rendbuf< RIdx > const* tris,
	                   Transform const& transf) const;

private:

	virtual void doRendering(Renderable const* renderable,
	                         Rendbuf< Real > const* poss,
	                         Rendbuf< Real > const* nrms,
	                         std::vector< Rendbuf< Real > const* > const& uvs,
	                         Rendbuf< Real > const* clrs,
	                         Rendbuf< RIdx > const* tris) const = 0;

};


inline Material::Material(void)
{
}

inline Material::~Material(void)
{
}

inline void Material::render(Renderable const* renderable,
                             Rendbuf< Real > const* poss,
                             Rendbuf< Real > const* nrms,
                             std::vector< Rendbuf< Real > const* > const& uvs,
                             Rendbuf< Real > const* clrs,
                             Rendbuf< RIdx > const* tris,
                             Transform const& transf) const
{
	// Apply transformation
	glPushMatrix();
// TODO: Check if we are using doubles instead of floats!
	glMultMatrixf(transpose(transf.getMatrix()).getCells());
	HppCheckGlErrors();

	// Actual rendering
	doRendering(renderable, poss, nrms, uvs, clrs, tris);

	glPopMatrix();
	HppCheckGlErrors();
}

}

#endif
