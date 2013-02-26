#ifndef HPP_MATERIAL_H
#define HPP_MATERIAL_H

#include "camera.h"
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
class Mesh;
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

	// Functions to set view and projection matrices
	virtual void setViewmatrix(Matrix4 const& viewmatrix) = 0;
	virtual void setProjectionmatrix(Matrix4 const& projectionmatrix) = 0;
	inline void setViewAndProjectionmatricesFromCamera(Camera const* camera);

	virtual void renderMesh(Mesh const* mesh, Transform const& transf) = 0;

	// Begins/ends manual rendering
	virtual void beginRendering(Color const& ambient_light = Color(0, 0, 0), Light const* light = NULL, bool additive_rendering = false) const = 0;
	virtual void endRendering(void) const = 0;

	virtual bool needsLight(Renderable const* renderable) const { (void)renderable; return true; }

private:

};


inline Material::Material(void)
{
}

inline Material::~Material(void)
{
}

inline void Material::setViewAndProjectionmatricesFromCamera(Camera const* camera)
{
	setViewmatrix(camera->getViewmatrix());
	setProjectionmatrix(camera->getProjectionmatrix());
}

}

#endif
