#ifndef HPP_ENTITY_H
#define HPP_ENTITY_H

#include "actionhandle.h"
#include "materialmanager.h"
#include "subentity.h"
#include "mesh.h"
#include "movable.h"
#include "matrix4.h"

#include <set>
#include <vector>

namespace Hpp
{

class Entity : public Movable
{

	friend class Actionhandle;

public:

	inline Entity(Mesh const* mesh);
	inline ~Entity(void);

	inline void setMaterial(size_t submesh_id, Material* mat);
	inline void setMaterial(Material* mat);
	inline void setDefaultMaterials(void);

	// Activates new action and returns handle to it
	inline Actionhandle* getActionHandle(std::string const& act_name);

private:

	// Functions for friends
	inline void unregisterActionhandle(Actionhandle* ahandle);
	inline void markSkeletonOutOfDate(void) { skel_uptodate = false; }

private:

	typedef std::set< Actionhandle* > Actionhandles;
	typedef std::vector< Matrix4 > BoneTransforms;

	Mesh const* mesh;
	Subentities subents;

	// Possible skeleton in use, actionhandles to its actions and
	// transformation of bones, ordered by vertexgroups.
	Skeleton const* skel;
	bool skel_uptodate;
	Actionhandles ahandles;
	BoneTransforms btransfs;

	inline virtual void absoluteTransformUpdated(void);
	inline virtual void prepareForRendering(void);

};

inline Entity::Entity(Mesh const* mesh) :
mesh(mesh),
// TODO: Make it possible to select other skeletons!
skel(mesh->getSkeleton()),
skel_uptodate(false)
{
	subents.reserve(mesh->getSubmeshesSize());
	for (size_t submesh_id = 0;
	     submesh_id < mesh->getSubmeshesSize();
	     submesh_id ++) {
	     	Submesh const* submesh = mesh->getSubmesh(submesh_id);
	     	Subentity* new_subent;
	     	try {
	     		new_subent = new Subentity(submesh);
	     	}
	     	catch ( ... ) {
			for (Subentities::iterator subents_it = subents.begin();
			     subents_it != subents.end();
			     subents_it ++) {
				delete *subents_it;
			}
			throw;
	     	}
	     	Renderables submesh_renderables;
	     	new_subent->getRenderables(submesh_renderables);
	     	addRenderables(submesh_renderables);
	     	subents.push_back(new_subent);
	}
	// Allocate memory for bonetransformations
	btransfs.assign(mesh->getNumOfVGroups(), Matrix4::IDENTITY);

	// By default, use boundingsphere of undistorted mesh
	setBoundingsphere(mesh->getDefaultBoundingsphere());
}

inline Entity::~Entity(void)
{
	HppAssert(ahandles.empty(), "Found unreleased actionhandles!");
	for (Subentities::iterator subents_it = subents.begin();
	     subents_it != subents.end();
	     subents_it ++) {
	     	delete *subents_it;
	}
}

inline void Entity::setMaterial(size_t submesh_id, Material* mat)
{
	HppAssert(submesh_id < subents.size(), "Submesh ID out of range!");
	subents[submesh_id]->setMaterial(mat);
}

inline void Entity::setMaterial(Material* mat)
{
	for (Subentities::iterator subents_it = subents.begin();
	     subents_it != subents.end();
	     subents_it ++) {
	     	Subentity* subent = *subents_it;
	     	subent->setMaterial(mat);
	}
}

inline void Entity::setDefaultMaterials(void)
{
	for (size_t submesh_id = 0; submesh_id < mesh->getSubmeshesSize(); submesh_id ++) {
		setMaterial(submesh_id, Materialmanager::getMaterial(mesh->getDefaultMaterial(submesh_id)));
	}
}

inline Actionhandle* Entity::getActionHandle(std::string const& act_name)
{
	Action const* act = skel->getAction(act_name);
	Actionhandle* new_acthandle = new Actionhandle(this, act);
	HppAssert(ahandles.find(new_acthandle) == ahandles.end(), "Already found!");
	ahandles.insert(new_acthandle);
	return new_acthandle;
}

inline void Entity::unregisterActionhandle(Actionhandle* ahandle)
{
	HppAssert(ahandles.find(ahandle) != ahandles.end(), "Unknown actionhandle!");
	ahandles.erase(ahandle);
}

inline void Entity::absoluteTransformUpdated(void)
{
	Transform const transf_abs = getAbsoluteTransform();
	for (Subentities::iterator subents_it = subents.begin();
	     subents_it != subents.end();
	     subents_it ++) {
		Subentity* subent = *subents_it;
		subent->updateAbsoluteTransform(transf_abs);
	}
}

inline void Entity::prepareForRendering(void)
{
	// If there is no skeleton or skeleton is not changed, then do nothing
	if (!skel || skel_uptodate) {
		return;
	}



	skel_uptodate = true;
}

}

#endif
