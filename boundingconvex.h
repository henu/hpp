#ifndef HPP_BOUNDINGCONVEX_H
#define HPP_BOUNDINGCONVEX_H

#include "boundingvolume.h"
#include "boundingbox.h"
#include "boundingsphere.h"
#include "plane.h"
#include "meshloader.h"

#include <string>

namespace Hpp
{

class Boundingconvex : public Boundingvolume
{

public:

	inline Boundingconvex(void);
	inline Boundingconvex(Plane::Vec const& cutplanes);

	// Normals of cutplanes must point inside Buildingconvex
	inline void cut(Plane const& cutplane);
	inline void setCutplanes(Plane::Vec const& cutplanes);

	inline virtual Testresult testAnotherBoundingvolume(Boundingvolume const* bv) const;
	inline virtual bool isPositionInside(Vector3 const& pos) const;

	inline bool isNothing(void) const { return nothing; }

	inline Boundingconvex intersection(Boundingconvex const& bconvex) const;

	inline std::string toString(void) const;

	// This is not very fast operation. Also cutplanes that are endless
	// do not appear properly or might be completely hidden.
	inline Mesh* createMesh(void) const;

	inline void translate(Vector3 const& transl);

private:

	// Edges of visible parts of cutplanes
	struct Edge
	{
		Vector3 begin;
		Vector3 end;
		bool begin_endless;
		bool end_endless;
	};
	typedef std::vector< Edge > Edges;

	struct Cutplane
	{
		Plane plane;
		Edges edges;
		inline Cutplane(Plane const& plane) : plane(plane) { }
	};
	typedef std::vector< Cutplane > Cutplanes;

	Cutplanes cutplanes;

	// This tells if cutplanes cut everything
	// so Boundingconvex converts nothing.
	bool nothing;

	inline void discardUselessCutplanes(void);

};

inline Boundingconvex::Boundingconvex(void) :
nothing(false)
{
}

inline Boundingconvex::Boundingconvex(Plane::Vec const& cutplanes)
{
	setCutplanes(cutplanes);
}

inline void Boundingconvex::cut(Plane const& cutplane)
{
	cutplanes.push_back(Cutplane(cutplane));
	discardUselessCutplanes();
}

inline void Boundingconvex::setCutplanes(Plane::Vec const& cutplanes)
{
	this->cutplanes.clear();
	for (Plane::Vec::const_iterator cutplanes_it = cutplanes.begin();
	     cutplanes_it != cutplanes.end();
	     ++ cutplanes_it) {
		Plane const& cutplane = *cutplanes_it;
		this->cutplanes.push_back(Cutplane(cutplane));
	}
	discardUselessCutplanes();
}

inline Boundingvolume::Testresult Boundingconvex::testAnotherBoundingvolume(Boundingvolume const* bv) const
{
	if (nothing) {
		return OUTSIDE;
	}

	// If another Boundingvolume is Boundingsphere
	Boundingsphere const* bs = dynamic_cast< Boundingsphere const* >(bv);
	if (bs) {
		if (bs->isInfinite()) {
			return PARTIALLY_INSIDE;
		}
		bool fully_inside = true;
		for (Cutplanes::const_iterator cutplanes_it = cutplanes.begin();
		     cutplanes_it != cutplanes.end();
		     ++ cutplanes_it) {
			Cutplane const& cutplane = *cutplanes_it;
			Real distance_to_plane = cutplane.plane.distanceToPosition(bs->getPosition());
			if (distance_to_plane < -bs->getRadius()) {
				return OUTSIDE;
			}
			if (distance_to_plane < bs->getRadius()) {
				fully_inside = false;
			}
		}
		if (fully_inside) return INSIDE;
		return PARTIALLY_INSIDE;
	}

	// If another Boundingvolume is Boundingbox
	Boundingbox const* bb = dynamic_cast< Boundingbox const* >(bv);
	if (bb) {
		// Convert box into sphere and test it
		Vector3 bs_pos = bb->getCenter();
		Real bs_radius = bb->getSpacediagonalLength() / 2;
		Boundingsphere bs(bs_pos, bs_radius);

		Boundingvolume::Testresult bs_result = testAnotherBoundingvolume(&bs);
		if (bs_result == OUTSIDE) return OUTSIDE;
		if (bs_result == INSIDE) return INSIDE;

		// To prevent false negative (box is completely inside, but
		// partly inside is returned), try with smaller sphere.
		bs_radius = std::min(std::min(bb->getWidthX(), bb->getDepthY()), bb->getHeightZ());
		Boundingsphere bs2(bs_pos, bs_radius);

		bs_result = testAnotherBoundingvolume(&bs2);
		if (bs_result == INSIDE) return INSIDE;
		return PARTIALLY_INSIDE;
	}

	// If another Boundingvolume is Boundingconvex
	Boundingconvex const* bc = dynamic_cast< Boundingconvex const* >(bv);
	if (bc) {
HppAssert(false, "Not implemented yet!");
// TODO: Code this!
return INSIDE;
	}

	// This Boundingvolume cannot be handled, so try the other way
	return bv->testAnotherBoundingvolume(this);
}

inline bool Boundingconvex::isPositionInside(Vector3 const& pos) const
{
	if (nothing) return false;

	for (Cutplanes::const_iterator cutplanes_it = cutplanes.begin();
	     cutplanes_it != cutplanes.end();
	     ++ cutplanes_it) {
		Cutplane const& cutplane = *cutplanes_it;
		if (!cutplane.plane.isPositionAtFront(pos)) {
			return false;
		}
	}
	return true;
}

inline Boundingconvex Boundingconvex::intersection(Boundingconvex const& bconvex) const
{
	Boundingconvex result;
	result.cutplanes.reserve(cutplanes.size() + bconvex.cutplanes.size());
	result.cutplanes.insert(result.cutplanes.end(), cutplanes.begin(), cutplanes.end());
	result.cutplanes.insert(result.cutplanes.end(), bconvex.cutplanes.begin(), bconvex.cutplanes.end());
	result.discardUselessCutplanes();
	return result;
}

inline std::string Boundingconvex::toString(void) const
{
	std::string result;
	for (Cutplanes::const_iterator cutplanes_it = cutplanes.begin();
	     cutplanes_it != cutplanes.end();
	     ++ cutplanes_it) {
		Cutplane const& cutplane = *cutplanes_it;
		result += cutplane.plane.toString();
		if (cutplanes.end() - cutplanes_it > 1) {
			result += " ";
		}
	}
	return result;
}

inline Mesh* Boundingconvex::createMesh(void) const
{
	Meshloader mloader;

	for (Cutplanes::const_iterator cutplanes_it = cutplanes.begin();
	     cutplanes_it != cutplanes.end();
	     ++ cutplanes_it) {
		Cutplane const& cutplane = *cutplanes_it;

		// Calculate point that is inside ("center") of cutpolygon
		Vector3 center = Vector3::ZERO;
		size_t samples = 0;
		for (Edges::const_iterator edges_it = cutplane.edges.begin();
		     edges_it != cutplane.edges.end();
		     ++ edges_it) {
			Edge const& edge = *edges_it;
			if (!edge.begin_endless) {
				center += edge.begin;
				++ samples;
			}
			if (!edge.end_endless) {
				center += edge.end;
				++ samples;
			}
		}
		if (samples <= 2) {
			continue;
		}
		center /= samples;

		// Now go all edges through again
		// and generate triangles from them.
		for (Edges::const_iterator edges_it = cutplane.edges.begin();
		     edges_it != cutplane.edges.end();
		     ++ edges_it) {
			Edge const& edge = *edges_it;
			Vector3 begin = edge.begin;
			Vector3 end = edge.end;
			if (edge.begin_endless) {
				begin += (edge.begin - edge.end).normalized() * 1000.0;
			}
			if (edge.end_endless) {
				end += (edge.end - edge.begin).normalized() * 1000.0;
			}
			Vector3 tri_normal = crossProduct(begin - center, end - center);
			// If normal is too short, then it means that triangle
			// would be just a line. It can be ignored.
			Real tri_normal_len = tri_normal.length();
			if (tri_normal_len < 0.00001) {
				continue;
			}
			tri_normal /= tri_normal_len;
			// Check if edge should be swapped
			if (dotProduct(cutplane.plane.getNormal(), tri_normal) > 0) {
				std::swap(begin, end);
				tri_normal = -tri_normal;
			}
			// Generate triangle
			mloader.addTriangle(center, begin, end,
			                    tri_normal, tri_normal, tri_normal);
		}

	}

	return mloader.createMesh();
}

inline void Boundingconvex::translate(Vector3 const& transl)
{
	for (Cutplanes::iterator cutplanes_it = cutplanes.begin();
	     cutplanes_it != cutplanes.end();
	     ++ cutplanes_it) {
		Cutplane& cutplane = *cutplanes_it;
		cutplane.plane = Plane(cutplane.plane.getNormal(), cutplane.plane.getPosition() + transl);
		for (Edges::iterator edges_it = cutplane.edges.begin();
		     edges_it != cutplane.edges.end();
		     ++ edges_it) {
			Edge& edge = *edges_it;
			edge.begin += transl;
			edge.end += transl;
		}
	}
}

inline void Boundingconvex::discardUselessCutplanes(void)
{
	nothing = false;
	if (cutplanes.empty()) {
		return;
	} else if (cutplanes.size() == 1) {
		cutplanes[0].edges.clear();
		return;
	}

	std::vector< size_t > cutplanes_to_destroy;

	// Go all cutplanes through, form their edges and
	// check if the cutplanes should be destroyed.
	for (size_t cutplane1_id = 0;
	     cutplane1_id < cutplanes.size();
	     ++ cutplane1_id) {

		Cutplane& cutplane1 = cutplanes[cutplane1_id];
		bool destroy_cutplane1 = false;

		// First form all Edges between
		// this and other current cutplanes.
		cutplane1.edges.clear();
		Edge new_edge;
		new_edge.begin_endless = true;
		new_edge.end_endless = true;
		Vector3 new_edge_dir;
		for (size_t cutplane2_id = 0;
		     cutplane2_id < cutplanes.size();
		     ++ cutplane2_id) {
			if (cutplane1_id == cutplane2_id) continue;
			Cutplane const& cutplane2 = cutplanes[cutplane2_id];

			if (cutplane1.plane.hitsPlane(cutplane2.plane, &new_edge.begin, &new_edge_dir)) {
				new_edge.end = new_edge.begin + new_edge_dir;
				HppAssert(new_edge_dir.length() > 0.000001, "Edge direction is too short!");
				cutplane1.edges.push_back(new_edge);
			}
		}

		// There was no edges. This means that all other cutplanes are
		// parallel to this one. So now go all cutplanes through and
		// check if this cutplane is at the back side of any of them.
		if (cutplane1.edges.empty()) {

			Vector3 pos_at_plane = cutplane1.plane.getPosition();

			for (size_t cutplane2_id = 0;
			     cutplane2_id < cutplanes.size();
			     ++ cutplane2_id) {
				if (cutplane1_id == cutplane2_id) continue;
				Cutplane const& cutplane2 = cutplanes[cutplane2_id];

				if (!cutplane2.plane.isPositionAtFront(pos_at_plane)) {
					destroy_cutplane1 = true;
					break;
				}
			}

		}
		// There was edges, now use other cutplanes to
		// shorten them. If all of them finally
		// disappear, then this Plane can be destroyed
		else {

			for (size_t cutplane2_id = 0;
			     cutplane2_id < cutplanes.size();
			     ++ cutplane2_id) {
				if (cutplane1_id == cutplane2_id) continue;
				Cutplane const& cutplane2 = cutplanes[cutplane2_id];

				size_t edge_id = 0;
				while (edge_id < cutplane1.edges.size()) {
					Edge& edge = cutplane1.edges[edge_id];
					// Get direction of edge and normalize it.
					Vector3 edge_dir = edge.end - edge.begin;
					Real edge_dir_len = edge_dir.length();
					HppAssert(edge_dir_len > 0.000001, "Edge direction is too short!");

					// Check how cutplane hits this edge
					bool destroy_edge = false;
					Vector3 hitpos;
					Real edge_dir_mult;
					if (cutplane2.plane.hitsLine(edge.begin, edge_dir, &hitpos, &edge_dir_mult)) {

						// Edge is cut from behind (i.e. nearer to begin)
						if (dotProduct(cutplane2.plane.getNormal(), edge_dir) > 0) {
							// Cut position does not reach edge at all
							if (!edge.begin_endless && edge_dir_mult < 0) {
								// Do nothing
							}
							// Edge continues endlessly after end
							else if (edge.end_endless) {
								edge.begin = hitpos;
								edge.end = edge.begin + edge_dir;
								edge.begin_endless = false;
							}
							// Cut position cuts edge, but does not reach end
							else if (edge_dir_mult < 1) {
								edge.begin = hitpos;
								if (edge.end_endless) {
									edge.end = edge.begin + edge_dir;
								}
								edge.begin_endless = false;
							}
							// Edge is cut completely
							else {
								destroy_edge = true;
							}
						}
						// Edge is cut from front (i.e. nearer to end)
						else {
							// Cut position does not reach edge at all
							if (!edge.end_endless && edge_dir_mult > 1) {
								// Do nothing
							}
							// Edge continues endlessly before begin
							else if (edge.begin_endless) {
								edge.end = hitpos;
								edge.begin = edge.end - edge_dir;
								edge.end_endless = false;
							}
							// Cut position cuts edge, but does not reach begin
							else if (edge_dir_mult > 0) {
								edge.end = hitpos;
								if (edge.begin_endless) {
									edge.begin = edge.end - edge_dir;
								}
								edge.end_endless = false;
							}
							// Edge is cut completely
							else {
								destroy_edge = true;
							}
						}

						// If edge is too short, then discard it
						if (!destroy_edge) {
							if ((edge.end - edge.begin).length() < 0.0001) {
								destroy_edge = true;
							}
						}

					}

					if (destroy_edge) {
						edge = cutplane1.edges.back();
						cutplane1.edges.pop_back();
					} else {
						++ edge_id;
					}

				}

			}

			destroy_cutplane1 = cutplane1.edges.empty();
		}

		if (destroy_cutplane1) {
			cutplanes_to_destroy.push_back(cutplane1_id);
		}

	}

	// Finally do actual destroying
	for (std::vector< size_t >::reverse_iterator cutplanes_to_destroy_rit = cutplanes_to_destroy.rbegin();
	     cutplanes_to_destroy_rit != cutplanes_to_destroy.rend();
	     ++ cutplanes_to_destroy_rit) {
		size_t cutplane_id = *cutplanes_to_destroy_rit;
		cutplanes[cutplane_id] = cutplanes.back();
		cutplanes.pop_back();
	}

	// If all cutplanes were destroyed, then mark
	// that this Boundingconvex covers nothing.
	if (cutplanes.empty()) {
		nothing = true;
	}
}

}

#endif
