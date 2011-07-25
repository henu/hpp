#ifndef HPP_MOVABLE_H
#define HPP_MOVABLE_H

#include "viewfrustum.h"
#include "transform.h"
#include "boundingsphere.h"
#include "visibles.h"

#include <set>

namespace Hpp
{

class Movable
{

	friend class Camera;

public:

	inline Movable(void);
	inline virtual ~Movable(void);

	// Parent handling
	inline void setParent(Movable* parent);
	inline Movable* getParent(void) { return parent; }
	inline Movable const* getParent(void) const { return parent; }

	// Transform modifiers
	inline void resetTransform(void) { transf.reset(); transformChanged(); }
	inline void setTransform(Transform const& transf) { this->transf = transf; transformChanged(); }
	inline void setPosition(Vector3 const& pos) { transf.setPosition(pos); transformChanged(); }
	inline void translate(Vector3 const& v) { transf.translate(v); transformChanged(); }
	inline void rotateX(Angle const& angle) { transf.rotateX(angle); transformChanged(); }
	inline void rotateY(Angle const& angle) { transf.rotateY(angle); transformChanged(); }
	inline void rotateZ(Angle const& angle) { transf.rotateZ(angle); transformChanged(); }
	inline void scale(Vector3 const& v) { transf.scale(v); transformChanged(); }
	inline Transform getAbsoluteTransform(void) const;
	// Transform getters
	inline Transform getTransform(void) const { return transf; }

	// Updates absolute transform. This may be only called if this Movable
	// does not have parent or its parent has up-to-date absolute
	// transform. This also updates absolute transforms of all children
	// that need updating.
	inline void updateAbsoluteTransform(void);

	// Updates total boundingsphere. Children are updated too. This may be
	// only called if this Movable does not have parent.
	inline void updateTotalBoundingsphere(void);

	// Boundingsphere must be visible when this is called
	inline Boundingsphere getTotalBoundingsphere(void) const;

	inline void setVisible(bool visible = true);
	inline bool getVisible(void) const { return visible; }

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	// Gets visibles from this Movable and from all of it's children. ofrusts is optional. It means occluding viewfrustums.
	inline void getAllVisibles(Visibles& result, Viewfrustums const& vfrusts, Viewfrustums const& ofrusts = Viewfrustums()) const;

protected:

	Transform transf;

	// Functions to access renderables
	inline void setRenderable(Renderable* renderable) { renderables.assign(1, renderable); }
	inline void setRenderables(Renderables& renderables) { this->renderables = renderables; }
	inline void addRenderable(Renderable* renderable) { renderables.push_back(renderable); }
	inline void addRenderables(Renderables& renderables) { this->renderables.insert(this->renderables.begin(), renderables.begin(), renderables.end()); }
	inline void clearRenderables(void) { renderables.clear(); }

	inline void transformChanged(void);

	inline void setBoundingsphere(Boundingsphere const& bs);

private:

	typedef std::set< Movable* > Children;

	enum TransformUpToDate { YES, NO, NO_FOR_CHILDREN };

	Movable* parent;
	Children children;

	// Absolute transform and state of it
	Transform transf_abs;
	TransformUpToDate transf_abs_uptodate;

	// Total boundingsphere. This contains boundingsphere of this Movable
	// and those from it's children. Total boundingsphere is relative to
	// this Movable.
	Boundingsphere totalbs;
	bool totalbs_uptodate;

	// Is Movable visible or not. If its not visible, then it wont appear
	// in recursive boundingvolume and its transform will not be updated.
	bool visible;

	// Regular boundingsphere
	Boundingsphere bs;

	// Renderables. These are set by subclass
	Renderables renderables;

	// Children registering and unregistering. This is called by children
	// who set this Movable as parent.
	inline void registerChild(Movable* child);
	inline void unregisterChild(Movable* child);

	// Internal absolute transform functions
	inline void checkIfAllChildrenHasAbsoluteTransformsUpToDate(void);
	inline void updateAbsoluteTransform(Transform const& parent_transf_abs);
	inline void transformChangedRecursive(void);

	inline void updateTotalBoundingsphereRecursive(void);

	// Marks total boundingspheres of this and all its parents to need
	// recalculation.
	inline void markTotalBoundingsphereToNeedRecalculation(void);

	// Informs renderable about updated transform
	inline virtual void absoluteTransformUpdated(void) { }

	inline static bool boundingsphereVisible(Boundingsphere const& bs, Transform const& bs_transf, Viewfrustums const& vfrusts, Viewfrustums const& ofrusts);

};

inline Movable::Movable(void) :
parent(NULL),
transf_abs_uptodate(NO),
totalbs_uptodate(false),
visible(true),
bs(Boundingsphere::nothing())
{
}

inline Movable::~Movable(void)
{
	HppAssert(children.empty(), "Unable to destroy Movable that has children!");
	if (parent) {
		parent->unregisterChild(this);
	}
}

void Movable::setParent(Movable* parent)
{
	if (this->parent) {
		this->parent->unregisterChild(this);
		if (!this->parent->transf_abs_uptodate == NO_FOR_CHILDREN) {
			this->parent->checkIfAllChildrenHasAbsoluteTransformsUpToDate();
		}
	}
	this->parent = parent;
	if (this->parent) {
		this->parent->registerChild(this);
	}
	// Make a little hack, and pretend that transform was fine. If this is
	// not set, then the next function will do nothing.
	transf_abs_uptodate = YES;
	transformChanged();
}

inline Transform Movable::getAbsoluteTransform(void) const
{
	HppAssert(transf_abs_uptodate != NO, "Unable to get absolute transform because it is not up to date!");
	return transf_abs;
}

inline void Movable::updateAbsoluteTransform(void)
{
	HppAssert(!parent || parent->transf_abs_uptodate != NO, "Parent transform must be up-to-date!");

	// Check if this call is useless
	if (transf_abs_uptodate == YES) {
		return;
	}

	if (!parent) {
		updateAbsoluteTransform(Transform());
	} else {
		updateAbsoluteTransform(parent->transf_abs);
	}

	// Check if this was the only child of parent that needed updating
	if (parent) {
		parent->checkIfAllChildrenHasAbsoluteTransformsUpToDate();
	}

}

inline void Movable::updateTotalBoundingsphere(void)
{
	// If hidden, then do nothing
	if (!visible) {
		return;
	}
	HppAssert(!parent, "No parent is allowed!");
	if (totalbs_uptodate) {
		return;
	}
	updateTotalBoundingsphereRecursive();
}

inline Boundingsphere Movable::getTotalBoundingsphere(void) const
{
	HppAssert(visible, "If total boundingsphere is wanted, then Movable must be visible!");
	HppAssert(totalbs_uptodate, "Total boundingsphere is not up-to-date!");
	return totalbs;
}

inline void Movable::setVisible(bool visible)
{
	// If there is no change, then do nothing
	if (this->visible == visible) {
		return;
	}
	this->visible = visible;
	// After this and all its children has been hidden/revealed,
	// bounding sphere needs recalculation.
	markTotalBoundingsphereToNeedRecalculation();
	// If revealed, then it is good idea to recalculate transformation
	if (visible) {
		transformChanged();
	}
}

inline void Movable::transformChanged(void)
{
	// If hidden, then do nothing
	if (!visible) {
		return;
	}

	// If already obsolete, then do nothing
	if (transf_abs_uptodate == NO) return;

	// Mark all children to need recalculation too
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	Movable* child = *children_it;
	     	if (child->visible && child->transf_abs_uptodate != NO) {
	     		child->transformChangedRecursive();
	     	}
	}

	transf_abs_uptodate = NO;

	// Inform all parents also
	Movable* infoparent = parent;
	while (infoparent && infoparent->transf_abs_uptodate == YES) {
		infoparent->transf_abs_uptodate = NO_FOR_CHILDREN;
		infoparent = infoparent->parent;
	}

	markTotalBoundingsphereToNeedRecalculation();
}

inline void Movable::setBoundingsphere(Boundingsphere const& bs)
{
	this->bs = bs;
	// If hidden, then do nothing more
	if (!visible) {
		return;
	}
	markTotalBoundingsphereToNeedRecalculation();
}

inline void Movable::registerChild(Movable* child)
{
	HppAssert(child, "Cannot be NULL!");
	HppAssert(children.find(child) == children.end(), "That child is already found!");
	// Ensure this will not make loop of parents
	#ifndef NDEBUG
	Movable* check_parent = this;
	while (check_parent) {
		HppAssert(child != check_parent, "Some Movables form parent loop!");
		check_parent = check_parent->parent;
	}
	#endif
	children.insert(child);
	// If hidden, then do nothing more
	if (!visible) {
		return;
	}
	markTotalBoundingsphereToNeedRecalculation();
}

inline void Movable::unregisterChild(Movable* child)
{
	HppAssert(child, "Cannot be NULL!");
	HppAssert(children.find(child) != children.end(), "That child does not exist!");
	children.erase(child);
	// If hidden, then do nothing more
	if (!visible) {
		return;
	}
	markTotalBoundingsphereToNeedRecalculation();
}

inline void Movable::getAllVisibles(Visibles& result, Viewfrustums const& vfrusts, Viewfrustums const& ofrusts) const
{
	// If hidden, then do nothing
	if (!visible) {
		return;
	}

	HppAssert(totalbs_uptodate, "Total boundingsphere is not up-to-date!");

	// If total boundingsphere is zero sized, then give up
	if (totalbs.isNothing()) {
		return;
	}

	Transform my_absolute_transf = getAbsoluteTransform();

	// Check which view and occlusionfrustums are needed with this Movable
	Viewfrustums children_vfrusts;
	Viewfrustums children_ofrusts;
	if (totalbs.isInfinite()) {
		children_vfrusts = vfrusts;
		children_ofrusts = ofrusts;
	} else {
		// Apply transformation to total boundingsphere
		Boundingsphere totalbs_t(my_absolute_transf.applyToPosition(totalbs.getPosition()), my_absolute_transf.getMaximumScaling() * totalbs.getRadius());
		// Check occlusionfrustums first, in case some of them totally
		// hides this boundingsphere.
		for (Viewfrustums::const_iterator ofrusts_it = ofrusts.begin();
		     ofrusts_it != ofrusts.end();
		     ofrusts_it ++) {
			Viewfrustum const& ofrust = *ofrusts_it;
			Viewfrustum::VFResult inside = ofrust.testBoundingsphere(totalbs_t);
			// If fully inside, then this scenenode and
			// everything inside it is fully occluded.
			if (inside == Viewfrustum::INSIDE) {
				return;
			}
			// If partially inside, then this occlusionfrustum
			// is needed when checking children
			else if (inside == Viewfrustum::PARTIALLY_INSIDE) {
				children_ofrusts.push_back(ofrust);
			}
			// If outside, then nothing is done.
		}
		// Now check viewfrustums. Do this only if there is viewfrustums. If there are no viewfrustums, then it means that everything will be visible
		if (!vfrusts.empty()) {
			bool fully_outside = true;
			for (Viewfrustums::const_iterator vfrusts_it = vfrusts.begin();
			     vfrusts_it != vfrusts.end();
			     vfrusts_it ++) {
				Viewfrustum const& vfrust = *vfrusts_it;
				Viewfrustum::VFResult inside = vfrust.testBoundingsphere(totalbs_t);
				// If fully inside, then no view frustums are
				// needed, because this one makes everything
				// inside this viewfrustum visible. However, it
				// is needed to mark that boundingsphere is
				// inside some viewfrustum.
				if (inside == Viewfrustum::INSIDE) {
					children_vfrusts.clear();
					fully_outside = false;
					break;
				}
				// If partially inside, then this viewfrustum
				// is needed when checking children
				else if (inside == Viewfrustum::PARTIALLY_INSIDE) {
					children_vfrusts.push_back(vfrust);
					fully_outside = false;
				}
				// If outside, then nothing is done.
			}
			// Check if there was no viewfrustums
			// that this boundingsphere is inside
			if (fully_outside) {
				return;
			}
		}
	}

	// Check local boundingsphere
	if (boundingsphereVisible(bs, my_absolute_transf, children_vfrusts, children_ofrusts)) {
		for (Renderables::const_iterator renderables_it = renderables.begin();
		     renderables_it != renderables.end();
		     renderables_it ++) {
		     	Renderable* renderable = *renderables_it;
		     	if (renderable->needsLight()) {
				result.normal.push_back(renderable);
		     	} else {
				result.no_light.push_back(renderable);
		     	}
		}
	}

	// Get visibles from children
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	Movable* child = *children_it;
	     	child->getAllVisibles(result, children_vfrusts, children_ofrusts);
	}
}

inline void Movable::checkIfAllChildrenHasAbsoluteTransformsUpToDate(void)
{
	HppAssert(transf_abs_uptodate == NO_FOR_CHILDREN, "Unexpected 100% up-to-date absolute transform!");
	bool all_up_to_date = true;
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	Movable* child = *children_it;
	     	// Skip children that are hidden
	     	if (!child->visible) {
			continue;
	     	}
	     	if (child->transf_abs_uptodate != YES) {
	     		all_up_to_date = false;
	     		break;
	     	}
	}
	if (!all_up_to_date) {
		return;
	}
	transf_abs_uptodate = YES;
	if (parent) {
		parent->checkIfAllChildrenHasAbsoluteTransformsUpToDate();
	}
}

inline void Movable::updateAbsoluteTransform(Transform const& parent_transf_abs)
{
	// If hidden, then do nothing
	if (!visible) {
		return;
	}

	HppAssert(!parent || parent->transf_abs_uptodate != NO, "Parent transform must be up-to-date!");
	HppAssert(transf_abs_uptodate != YES, "Must not be YES!");

	// Update absolute transform if it's really needed
	if (transf_abs_uptodate == NO) {
		transf_abs = transf.addAnotherTransform(parent_transf_abs);
	}

	// Mark transform updated
	transf_abs_uptodate = YES;

	// Inform subclass
	absoluteTransformUpdated();

	// Go children through and make required updates
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Movable* child = *children_it;
		if (child->transf_abs_uptodate != YES) {
			child->updateAbsoluteTransform(transf_abs);
		}
	}
}

inline void Movable::transformChangedRecursive(void)
{
	HppAssert(transf_abs_uptodate != NO, "Must not be NO!");
	HppAssert(visible, "Movable must be visible!");
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
	     	Movable* child = *children_it;
	     	if (child->transf_abs_uptodate != NO) {
	     		child->transformChangedRecursive();
	     	}
	}
	transf_abs_uptodate = NO;
}

inline void Movable::updateTotalBoundingsphereRecursive(void)
{
	// If hidden, then do nothing
	if (!visible) {
		return;
	}
	// First ensure children have their total boundingspheres up-to-date
	for (Children::iterator children_it = children.begin();
	     children_it != children.end();
	     children_it ++) {
		Movable* child = *children_it;
		if (!child->totalbs_uptodate) {
			child->updateTotalBoundingsphereRecursive();
		}
	}

	// Mark total boundingsphere up-to-date
	totalbs_uptodate = true;

	// Do actual calculations
	bool totalbs_infinite = bs.isInfinite();
	Vector3 totalbs_pos;
	Boundingspheres children_bss;
	children_bss.reserve(children.size() + 1);
	if (!totalbs_infinite) {

		// Add main boundinsphere only if it is not zero sized.
		if (!bs.isNothing()) {
			totalbs_pos = bs.getPosition();
			children_bss.push_back(bs);
		} else {
			totalbs_pos = Vector3::ZERO;
		}

		for (Children::iterator children_it = children.begin();
		     children_it != children.end();
		     children_it ++) {
			Movable* child = *children_it;
			// Skip children that are hidden
			if (!child->visible) {
				continue;
			}
			Boundingsphere child_bs = child->getTotalBoundingsphere();
			if (child_bs.isInfinite()) {
				totalbs_infinite = true;
				break;
			}
			// Skip children that have boundingsphere of size zero
			if (child_bs.isNothing()) {
				continue;
			}

			// Apply transform of child to boundingsphere
			child_bs.applyTransform(child->getTransform());

			children_bss.push_back(child_bs);
			totalbs_pos += child_bs.getPosition();
		}
	}
	if (totalbs_infinite) {
		totalbs = Boundingsphere::infinite();
		return;
	}

	// If there was no boundingspheres, then set size to zero
	if (children_bss.empty()) {
		totalbs.setNothing();
		return;
	}

	// Calculate boundingsphere radius
	totalbs_pos /= children_bss.size();
	Real totalbs_radius = (totalbs_pos - children_bss[0].getPosition()).length() + children_bss[0].getRadius();
	for (size_t child_bs_id = 1; child_bs_id < children_bss.size(); child_bs_id ++) {
	     	Boundingsphere const& child_bs = children_bss[child_bs_id];
	     	Real radius_candidate = (totalbs_pos - child_bs.getPosition()).length() + child_bs.getRadius();
	     	if (radius_candidate > totalbs_radius) {
			totalbs_radius = radius_candidate;
	     	}
	}
	totalbs.set(totalbs_pos, totalbs_radius);
}

inline void Movable::markTotalBoundingsphereToNeedRecalculation(void)
{
	Movable* totalbs_owner = this;
	while (totalbs_owner->totalbs_uptodate) {
		totalbs_owner->totalbs_uptodate = false;
		totalbs_owner = totalbs_owner->parent;
		if (!totalbs_owner) {
			break;
		}
	}
}

inline bool Movable::boundingsphereVisible(Boundingsphere const& bs, Transform const& bs_transf, Viewfrustums const& vfrusts, Viewfrustums const& ofrusts)
{
	Boundingsphere bs_fixed(bs_transf.applyToPosition(bs.getPosition()), bs_transf.getMaximumScaling() * bs.getRadius());
	if (!vfrusts.empty()) {
		bool visible = false;
		for (Viewfrustums::const_iterator vfrusts_it = vfrusts.begin();
		     vfrusts_it != vfrusts.end();
		     vfrusts_it ++) {
			Viewfrustum const& vfrust = *vfrusts_it;
			if (vfrust.testBoundingsphere(bs_fixed) != Viewfrustum::OUTSIDE) {
				visible = true;
				break;
			}
		}
		if (!visible) {
			return false;
		}
	}
	for (Viewfrustums::const_iterator ofrusts_it = ofrusts.begin();
	     ofrusts_it != ofrusts.end();
	     ofrusts_it ++) {
		Viewfrustum const& ofrust = *ofrusts_it;
		if (ofrust.testBoundingsphere(bs_fixed) != Viewfrustum::INSIDE) {
			return false;
		}
	}
	return true;
}

}

#endif
