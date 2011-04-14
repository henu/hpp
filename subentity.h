#ifndef HPP_SUBENTITY_H
#define HPP_SUBENTITY_H

#include "submesh.h"
#include "renderable.h"

#include <vector>

namespace Hpp
{

class Material;

class Subentity
{

	friend class Entity;

public:

	inline Subentity(Submesh const* submesh);
	inline ~Subentity(void);

	inline void setMaterial(Material* mat);

private:

	// Functions for friends
	inline void getRenderables(Renderables& result);
	inline void updateAbsoluteTransform(Transform const& entity_transf);

private:

	Submesh const* submesh;

	Renderable* renderable;

};

typedef std::vector< Subentity* > Subentities;

inline Subentity::Subentity(Submesh const* submesh) :
submesh(submesh)
{
	renderable = submesh->createRenderable();
}

inline Subentity::~Subentity(void)
{
	delete renderable;
}

inline void Subentity::setMaterial(Material* mat)
{
	renderable->setMaterial(mat);
}

inline void Subentity::getRenderables(Renderables& result)
{
	result.push_back(renderable);
}

inline void Subentity::updateAbsoluteTransform(Transform const& entity_transf)
{
	renderable->updateAbsoluteTransform(entity_transf);
}

}

#endif
