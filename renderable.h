#ifndef HPP_RENDERABLE_H
#define HPP_RENDERABLE_H

#include "material.h"
#include "rendbuf.h"
#include "renderindex.h"
#include "transform.h"
#include "real.h"

#include <vector>

namespace Hpp
{

class Renderable
{

	friend class Renderingenvironment;

public:

	inline Renderable(void);
	inline Renderable(Rendbuf< Real > const* poss,
	                  Rendbuf< Real > const* nrms,
	                  std::vector< Rendbuf< Real >* > const& uvs,
	                  Rendbuf< Real > const* clrs,
	                  Rendbuf< RIdx > const* tris);
	inline Renderable(Rendbuf< Real > const* poss,
	                  Rendbuf< Real > const* nrms,
	                  std::vector< Rendbuf< Real > const* > const& uvs,
	                  Rendbuf< Real > const* clrs,
	                  Rendbuf< RIdx > const* tris);

	inline virtual ~Renderable(void);

	inline void setPositionsbuf(Rendbuf< Real > const* poss) { this->poss = poss; }
	inline void setNormalsbuf(Rendbuf< Real > const* nrms) { this->nrms = nrms; }
	inline void setUVsbuf(std::vector< Rendbuf< Real >* > const& uvs) { this->uvs.assign(uvs.begin(), uvs.end()); }
	inline void setUVsbuf(std::vector< Rendbuf< Real > const* > const& uvs) { this->uvs = uvs; }
	inline void setColorsbuf(Rendbuf< Real > const* clrs) { this->clrs = clrs; }
	inline void setTrianglesbuf(Rendbuf< RIdx > const* tris) { this->tris = tris; }

	inline Transform getTransform(void) const { return transf; }
	inline Transform getAbsoluteTransform(void) const { return transf_abs; }

	inline void setMaterial(Material* mat) { this->mat = mat; }
	inline Material* getMaterial(void) { return mat; }
	inline Material const* getMaterial(void) const { return mat; }

	inline bool needsLight(void) const;

	// Updates absolute transform using absolute transform of owning
	// Movable.
	inline void updateAbsoluteTransform(Transform const& owner_transf);

private:

	// Renders using specific material. Material needs to be given because
	// in some situations, default material of renderable may have been
	// changed.
	inline void render(Material const* mat);

private:

	Rendbuf< Real > const* poss;
	Rendbuf< Real > const* nrms;
	std::vector< Rendbuf< Real > const* > uvs;
	Rendbuf< Real > const* clrs;
	Rendbuf< RIdx > const* tris;

	Material* mat;

	// Transforms of this renderable. For absolute transform to work,
	// Movable that owns this Renderable must manually update it before
	// rendering process every time its transform is changed.
	Transform transf;
	Transform transf_abs;

	// Gives subclasses possibility to prepare for rendering
	inline virtual void prepareForRendering(void) { }

};
typedef std::vector< Renderable* > Renderables;

inline Renderable::Renderable(void) :
poss(NULL),
nrms(NULL),
uvs(NULL),
clrs(NULL),
tris(NULL),
mat(NULL)
{
}

inline Renderable::Renderable(Rendbuf< Real > const* poss,
                              Rendbuf< Real > const* nrms,
                              std::vector< Rendbuf< Real > const* > const& uvs,
                              Rendbuf< Real > const* clrs,
                              Rendbuf< RIdx > const* tris) :
poss(poss),
nrms(nrms),
uvs(uvs.begin(), uvs.end()),
clrs(clrs),
tris(tris),
mat(NULL)
{
}

inline Renderable::Renderable(Rendbuf< Real > const* poss,
                              Rendbuf< Real > const* nrms,
                              std::vector< Rendbuf< Real >* > const& uvs,
                              Rendbuf< Real > const* clrs,
                              Rendbuf< RIdx > const* tris) :
poss(poss),
nrms(nrms),
uvs(uvs.begin(), uvs.end()),
clrs(clrs),
tris(tris),
mat(NULL)
{
}

inline Renderable::~Renderable(void)
{
}

inline bool Renderable::needsLight(void) const
{
	if (!mat || !mat->needsLight(this)) {
		return false;
	}
	return true;
}

inline void Renderable::updateAbsoluteTransform(Transform const& owner_transf)
{
	transf_abs = transf.addAnotherTransform(owner_transf);
}

inline void Renderable::render(Material const* mat)
{
	prepareForRendering();
	mat->render(this, poss, nrms, uvs, clrs, tris, transf_abs);
}

}

#endif
