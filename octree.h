#ifndef HPP_OCTREE_H
#define HPP_OCTREE_H

#include "boundingbox.h"
#include "vector3.h"
#include "assert.h"

#include <vector>
#include <set>

namespace Hpp
{

class Octree
{

public:

	class Region;

	class Handle
	{
	public:
		inline Handle(void);
		inline Handle(Region* region);
		inline ~Handle(void);

		inline Region* getRegion(void) { return region; }

		// Copy constructor and assignment operator
		inline Handle(Handle const& handle);
		inline Handle operator=(Handle const& handle);
		// Functions for user data
		inline void setData(void* data) { this->data = data; }
		inline void* getData(void) { return data; }

		// Although is it safe to call this, this
		// should only be called by Region.
		inline void setNoRegion(void);
	private:
		Region* region;
		void* data;
	};
	typedef std::vector< Handle* > Handles;
	typedef std::set< Handle* > HandleSet;

	class Region
	{

	public:

		inline Region(Vector3 const& min_corner, Vector3 const& max_corner, Octree* octree, Region* parent, uint8_t type);
		inline ~Region(void);

		inline Handle spawnHandle(Boundingbox const& handle_bb);

		// This returns all hangles in this Region and in its ancestors
		// and children. This will clear the result.
		inline void getAllHandles(Handles& result);

		// "result" is not cleared
		inline void getCollidingHandles(Handles& result, Boundingvolume const* bv);

		inline Boundingbox getBoundingbox(void) const { return bb; }

		// Called by Handles
		inline void registerHandle(Handle* handle);
		inline void unregisterHandle(Handle* handle);

	private:
		Octree* octree;
		Region* parent;
		Boundingbox bb;
		Vector3 center;
		Region* children[8];
		// Type of region. 0 - 7 are different corners. 8 = root.
		uint8_t type;

		HandleSet handles;

		// "result" is not cleared
		inline void doGetAllHandles(Handles& result, bool descent_to_children_recursively);
	};

	inline Octree(void);
	inline Octree(Vector3 const& min_corner, Vector3 const& max_corner);

	inline Handle spawnHandle(Boundingbox const& handle_bb);

	// Get all handles that are at the regions that touch
	// to given Boundingvolume. "result" is not cleared.
	inline void getCollidingHandles(Handles& result, Boundingvolume const* bv);

private:

	Vector3 cmin, cmax;

	Region root;

};

inline Octree::Octree(void) :
cmin(Vector3::ZERO),
cmax(Vector3::ZERO),
root(Vector3::ZERO, Vector3::ZERO, this, NULL, 8)
{
}

inline Octree::Octree(Vector3 const& min_corner, Vector3 const& max_corner) :
cmin(min_corner),
cmax(max_corner),
root(min_corner, max_corner, this, NULL, 8)
{
}

inline Octree::Handle Octree::spawnHandle(Boundingbox const& handle_bb)
{
	return root.spawnHandle(handle_bb);
}

inline Octree::Handle::Handle(void) :
region(NULL),
data(NULL)
{
}

inline void Octree::getCollidingHandles(Handles& result, Boundingvolume const* bv)
{
	root.getCollidingHandles(result, bv);
}

inline Octree::Handle::Handle(Region* region) :
region(region),
data(NULL)
{
	HppAssert(region, "No region given!");
	region->registerHandle(this);
}

inline Octree::Handle::Handle(Handle const& handle) :
region(handle.region),
data(handle.data)
{
	if (region) {
		region->registerHandle(this);
	}
}

inline Octree::Handle::~Handle(void)
{
	if (region) {
		region->unregisterHandle(this);
	}
}

inline Octree::Handle Octree::Handle::operator=(Handle const& handle)
{
	if (region) {
		region->unregisterHandle(this);
	}
	region = handle.region;
	data = handle.data;
	if (region) {
		region->registerHandle(this);
	}
	return *this;
}

inline void Octree::Handle::setNoRegion(void)
{
	if (region) {
		region->unregisterHandle(this);
	}
	region = NULL;
}

inline Octree::Region::Region(Vector3 const& min_corner, Vector3 const& max_corner, Octree* octree, Region* parent, uint8_t type) :
octree(octree),
parent(parent),
bb(min_corner, max_corner),
center(bb.getCenter()),
type(type)
{
	for (uint8_t child_id = 0; child_id < 8; ++ child_id) {
		children[child_id] = NULL;
	}
}

inline Octree::Region::~Region(void)
{
	while (!handles.empty()) {
		(*handles.begin())->setNoRegion();
	}
	for (uint8_t child_id = 0; child_id < 8; ++ child_id) {
		delete children[child_id];
	}
}

inline Octree::Handle Octree::Region::spawnHandle(Boundingbox const& handle_bb)
{
	// If this does not fit to the children, then spawn it to this Region.
	if ((handle_bb.getMinX() < center.x && handle_bb.getMaxX() > center.x) ||
	    (handle_bb.getMinY() < center.y && handle_bb.getMaxY() > center.y) ||
	    (handle_bb.getMinZ() < center.z && handle_bb.getMaxZ() > center.z)) {
		return Handle(this);
	}

	uint8_t child_id;
	if (handle_bb.getMaxZ() <= center.z) {
		if (handle_bb.getMaxY() <= center.y) {
			if (handle_bb.getMaxX() <= center.x) {
				child_id = 0;
				if (!children[child_id]) {
					Vector3 child_cmin(bb.getMinX(), bb.getMinY(), bb.getMinZ());
					Vector3 child_cmax(center.x, center.y, center.z);
					children[child_id] = new Region(child_cmin, child_cmax, octree, this, child_id);
				}
			} else {
				child_id = 1;
				if (!children[child_id]) {
					Vector3 child_cmin(center.x, bb.getMinY(), bb.getMinZ());
					Vector3 child_cmax(bb.getMaxX(), center.y, center.z);
					children[child_id] = new Region(child_cmin, child_cmax, octree, this, child_id);
				}
			}
		} else {
			if (handle_bb.getMaxX() <= center.x) {
				child_id = 2;
				if (!children[child_id]) {
					Vector3 child_cmin(bb.getMinX(), center.y, bb.getMinZ());
					Vector3 child_cmax(center.x, bb.getMaxY(), center.z);
					children[child_id] = new Region(child_cmin, child_cmax, octree, this, child_id);
				}
			} else {
				child_id = 3;
				if (!children[child_id]) {
					Vector3 child_cmin(center.x, center.y, bb.getMinZ());
					Vector3 child_cmax(bb.getMaxX(), bb.getMaxY(), center.z);
					children[child_id] = new Region(child_cmin, child_cmax, octree, this, child_id);
				}
			}
		}
	} else {
		if (handle_bb.getMaxY() <= center.y) {
			if (handle_bb.getMaxX() <= center.x) {
				child_id = 4;
				if (!children[child_id]) {
					Vector3 child_cmin(bb.getMinX(), bb.getMinY(), center.z);
					Vector3 child_cmax(center.x, center.y, bb.getMaxZ());
					children[child_id] = new Region(child_cmin, child_cmax, octree, this, child_id);
				}
			} else {
				child_id = 5;
				if (!children[child_id]) {
					Vector3 child_cmin(center.x, bb.getMinY(), center.z);
					Vector3 child_cmax(bb.getMaxX(), center.y, bb.getMaxZ());
					children[child_id] = new Region(child_cmin, child_cmax, octree, this, child_id);
				}
			}
		} else {
			if (handle_bb.getMaxX() <= center.x) {
				child_id = 6;
				if (!children[child_id]) {
					Vector3 child_cmin(bb.getMinX(), center.y, center.z);
					Vector3 child_cmax(center.x, bb.getMaxY(), bb.getMaxZ());
					children[child_id] = new Region(child_cmin, child_cmax, octree, this, child_id);
				}
			} else {
				child_id = 7;
				if (!children[child_id]) {
					Vector3 child_cmin(center.x, center.y, center.z);
					Vector3 child_cmax(bb.getMaxX(), bb.getMaxY(), bb.getMaxZ());
					children[child_id] = new Region(child_cmin, child_cmax, octree, this, child_id);
				}
			}
		}
	}
	return children[child_id]->spawnHandle(handle_bb);
}

inline void Octree::Region::getAllHandles(Handles& result)
{
	result.clear();
	Region* region = this;
	while (region) {
		region->doGetAllHandles(result, region == this);
		region = region->parent;
	}
}

inline void Octree::Region::doGetAllHandles(Handles& result, bool descent_to_children_recursively)
{
	// First return Handles from this Region
	result.insert(result.end(), handles.begin(), handles.end());
	// If children are wanted, then return their Handles
	if (descent_to_children_recursively) {
		for (uint8_t child_id = 0; child_id < 8; ++ child_id) {
			Region* child = children[child_id];
			if (child) {
				child->doGetAllHandles(result, true);
			}
		}
	}
}

inline void Octree::Region::getCollidingHandles(Handles& result, Boundingvolume const* bv)
{
	// If given boundingvolume does not hit to this Region, then do nothing.
	if (bb.testAnotherBoundingvolume(bv) == Boundingvolume::OUTSIDE) {
		return;
	}

	// Add all Handles from this Region
	result.insert(result.end(), handles.begin(), handles.end());

	// Check children too
	for (uint8_t child_id = 0; child_id < 8; ++ child_id) {
		Region* child = children[child_id];
		if (child) {
			child->getCollidingHandles(result, bv);
		}
	}
}

inline void Octree::Region::registerHandle(Handle* handle)
{
	HppAssert(handles.find(handle) == handles.end(), "Handle already registered!");
	handles.insert(handle);
}

inline void Octree::Region::unregisterHandle(Handle* handle)
{
	HppAssert(handles.find(handle) != handles.end(), "Handle not registered!");
	handles.erase(handle);
}

}

#endif

