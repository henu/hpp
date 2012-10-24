#ifndef HPP_MESH_H
#define HPP_MESH_H

#include "skeleton.h"
#include "renderable.h"
#include "rawmesh.h"
#include "submesh.h"
#include "boundingsphere.h"
#include "real.h"
#include "3dutils.h"

#include <vector>

namespace Hpp
{

class OldMesh
{

	friend class Entity;

public:

	inline OldMesh(Rawmesh const& rawmesh, Rawmesh::Halvingstyle hstyle = Rawmesh::SHORTER);
	inline ~OldMesh(void);

	inline size_t getSubmeshesSize(void) const { return submeshes.size(); }
	inline Submesh const* getSubmesh(size_t submesh_id) const { HppAssert(submesh_id < submeshes.size(), "Submesh ID out of range!"); return submeshes[submesh_id]; }
	inline std::string getDefaultMaterial(size_t submesh_id) const { HppAssert(submesh_id < submeshes.size(), "Submesh ID out of range!"); return submeshes[submesh_id]->getDefaultMaterial(); }

	// Returns boundingsphere, that is not
	// distorted for example by skeleton.
	inline Boundingsphere getDefaultBoundingsphere(void) const;

private:

	// Functions for friends
	inline Skeleton const* getSkeleton(void) const { return skel; }
	inline size_t getNumOfVGroups(void) const { return vgroups.size(); }

private:

	typedef std::map< std::string, size_t > VGroups;

	Submeshes submeshes;

	Skeleton* skel;

	VGroups vgroups;

	// These functions are used to find shared normals of smooth faces.
// TODO: Ensure all of these use only triangles!
	inline static void findSharedNormals(Submesh::VrtsSNormals& result, Rawmesh const& rawmesh, std::vector< Rawmesh::Tris > const& tris_n_quads);
	inline static void addNormalToSharedNormals(Submesh::VrtsSNormals& result, uint8_t face_vrts_size, uint32_t const* face_vrts, Vector3 const& normal);
	inline static void addTangentAndBinormalToSharedNormals(Submesh::VrtsSNormals& result, uint8_t face_vrts_size, uint32_t const* face_vrts, size_t submesh_id, Vector2 const* uvs, Vector3 const& tangent, Vector3 const& binormal);

};

inline OldMesh::OldMesh(Rawmesh const& rawmesh, Rawmesh::Halvingstyle hstyle)
{
	// Convert all quads to triangles and merge them with triangles
	std::vector< Rawmesh::Tris > tris_n_quads;
	tris_n_quads.reserve(rawmesh.submeshes.size());
	for (Rawmesh::Submeshes::const_iterator submeshes_it = rawmesh.submeshes.begin();
	     submeshes_it != rawmesh.submeshes.end();
	     ++ submeshes_it) {
	     	Rawmesh::Submesh const& submesh = *submeshes_it;
	     	Rawmesh::Tris submesh_tnq;
	     	submesh_tnq.reserve(submesh.tris.size() + submesh.quads.size() * 2);
	     	submesh_tnq.insert(submesh_tnq.end(), submesh.tris.begin(), submesh.tris.end());
	     	for (Rawmesh::Quads::const_iterator quads_it = submesh.quads.begin();
	     	     quads_it != submesh.quads.end();
	     	     ++ quads_it) {
	     	     	Rawmesh::Tri newtri1;
	     	     	Rawmesh::Tri newtri2;
			Rawmesh::convertQuadToTris(newtri1, newtri2, *quads_it, hstyle, &rawmesh.vrts);
	     	     	submesh_tnq.push_back(newtri1);
	     	     	submesh_tnq.push_back(newtri2);
		}
		tris_n_quads.push_back(submesh_tnq);
	}

	// Go all faces through and find shared normals of smooth faces.
	Submesh::VrtsSNormals vrts_snormal;
	findSharedNormals(vrts_snormal, rawmesh, tris_n_quads);

	submeshes.reserve(rawmesh.submeshes.size());
	for (size_t submesh_id = 0;
	     submesh_id < rawmesh.submeshes.size();
	     submesh_id ++) {
		submeshes.push_back(new Submesh(submesh_id, rawmesh.submeshes[submesh_id], tris_n_quads[submesh_id], rawmesh.vrts, vrts_snormal));
	}

	// Copy possible skeleton
	if (rawmesh.skeleton_exists) {
		skel = new Skeleton(rawmesh.skeleton);
	} else {
		skel = NULL;
	}

	// Init Vertexgroups
	for (Rawmesh::VGroups::const_iterator vgroups_it = rawmesh.vgroups.begin();
	     vgroups_it != rawmesh.vgroups.end();
	     ++ vgroups_it) {
		std::string const& vgroup = *vgroups_it;
		size_t vgroup_id = vgroups.size();
		vgroups[vgroup] = vgroup_id;
	}

}

inline OldMesh::~OldMesh(void)
{
	delete skel;
	for (Submeshes::iterator submeshes_it = submeshes.begin();
	     submeshes_it != submeshes.end();
	     ++ submeshes_it) {
		delete *submeshes_it;
	}
}

inline Boundingsphere OldMesh::getDefaultBoundingsphere(void) const
{
	Boundingsphere result;
	Vector3 vrts_min(99999.9, 99999.9, 99999.9);
	Vector3 vrts_max(-99999.9, -99999.9, -99999.9);;
	for (Submeshes::const_iterator submeshes_it = submeshes.begin();
	     submeshes_it != submeshes.end();
	     ++ submeshes_it) {
		Submesh const* submesh = *submeshes_it;
		submesh->getVertexBoundings(vrts_min, vrts_max);
	}
	Vector3 bsphere_pos = (vrts_min + vrts_max) / 2;
	Real bsphere_radius_to_2 = 0.0;
	for (Submeshes::const_iterator submeshes_it = submeshes.begin();
	     submeshes_it != submeshes.end();
	     ++ submeshes_it) {
		Submesh const* submesh = *submeshes_it;
		submesh->getMaxVertexDistance(bsphere_radius_to_2, bsphere_pos);
	}
	return Boundingsphere(bsphere_pos, sqrt(bsphere_radius_to_2));
}

inline void OldMesh::findSharedNormals(Submesh::VrtsSNormals& result,
                                    Rawmesh const& rawmesh,
                                    std::vector< Rawmesh::Tris > const& tris_n_quads)
{
	HppAssert(result.empty(), "Result must be empty!");
	for (size_t submesh_id = 0;
	     submesh_id < tris_n_quads.size();
	     submesh_id ++) {
	     	Rawmesh::Tris const& submesh_tris_n_quads = tris_n_quads[submesh_id];
		for (Rawmesh::Tris::const_iterator tris_it = submesh_tris_n_quads.begin();
		     tris_it != submesh_tris_n_quads.end();
		     ++ tris_it) {
			Rawmesh::Tri const& tri = *tris_it;
			// Skip solid faces. They cannot make vertices dynamic
			if (!tri.smooth) {
				continue;
			}
			Vector3 vrts_pos[3];
			for (uint8_t face_vrt_id = 0; face_vrt_id < 3; face_vrt_id ++) {
				vrts_pos[face_vrt_id] = rawmesh.vrts[tri.vrts[face_vrt_id]].pos;
			}
			Vector3 nrm = crossProduct(vrts_pos[1] - vrts_pos[0], vrts_pos[2] - vrts_pos[0]).normalized();
			addNormalToSharedNormals(result, 3, tri.vrts, nrm);
			bool uvs_exist = !tri.layers.empty() && tri.layers[0].uvs[0].size() == 2;
			if (uvs_exist) {
				Vector3 tangent, binormal;
				Vector2 uvs[3];
				uvs[0] = Vector2(tri.layers[0].uvs[0][0], tri.layers[0].uvs[0][1]);
				uvs[1] = Vector2(tri.layers[0].uvs[1][0], tri.layers[0].uvs[1][1]);
				uvs[2] = Vector2(tri.layers[0].uvs[2][0], tri.layers[0].uvs[2][1]);
				Submesh::calculateTangentspace(tangent, binormal, vrts_pos, uvs);
				addTangentAndBinormalToSharedNormals(result, 3, tri.vrts, submesh_id, uvs, tangent, binormal);
			}
		}
	}
	// Before returning shared normals, be sure that they (and binormals
	// and tangents) are normalized.
	uint32_t vrt_find = 0;
	Submesh::VrtsSNormals::iterator result_find;
	while ((result_find = result.lower_bound(vrt_find)) != result.end()) {

		Vector3& normal = result_find->second.nrm;

		// Normalize normal
		Real normal_len = normal.length();
		if (normal_len > 0) {
			normal.x /= normal_len;
			normal.y /= normal_len;
			normal.z /= normal_len;
		}
		// Normalize tangents and binormals of all UVs
		for (Submesh::TangentsAndBinormals::iterator ts_n_bs_it = result_find->second.ts_n_bs.begin();
		     ts_n_bs_it != result_find->second.ts_n_bs.end();
		     ++ ts_n_bs_it) {
			Submesh::TangentAndBinormal& t_n_b = ts_n_bs_it->second;

			// Make sure tangent and binormal is 90° to normal.
			t_n_b.tangent = posToPlane(t_n_b.tangent, Vector3::ZERO, normal);
			t_n_b.binormal = posToPlane(t_n_b.binormal, Vector3::ZERO, normal);

			Real tangent_len = t_n_b.tangent.length();
			if (tangent_len > 0) {
				t_n_b.tangent.x /= tangent_len;
				t_n_b.tangent.y /= tangent_len;
				t_n_b.tangent.z /= tangent_len;
			}
			Real binormal_len = t_n_b.binormal.length();
			if (binormal_len > 0) {
				t_n_b.binormal.x /= binormal_len;
				t_n_b.binormal.y /= binormal_len;
				t_n_b.binormal.z /= binormal_len;
				// Also make sure tangent and binormal are
				// perpendicular against each others.
				if (tangent_len > 0) {
					forceVectorsPerpendicular(t_n_b.tangent, t_n_b.binormal);
					t_n_b.tangent.normalize();
					t_n_b.binormal.normalize();
				}
			}
		}

		// Select next vertex
		vrt_find = result_find->first + 1;
	}

}

inline void OldMesh::addNormalToSharedNormals(Submesh::VrtsSNormals& result, uint8_t face_vrts_size, uint32_t const* face_vrts, Vector3 const& normal)
{
	for (uint8_t face_vrt_id = 0; face_vrt_id < face_vrts_size; face_vrt_id ++) {
		uint32_t vrt_id = face_vrts[face_vrt_id];
		Submesh::VrtsSNormals::iterator result_find = result.find(vrt_id);
		if (result_find == result.end()) {
			result[vrt_id].nrm = normal;
		} else {
			result_find->second.nrm += normal;
		}
	}
}

inline void OldMesh::addTangentAndBinormalToSharedNormals(Submesh::VrtsSNormals& result, uint8_t face_vrts_size, uint32_t const* face_vrts, size_t submesh_id, Vector2 const* uvs, Vector3 const& tangent, Vector3 const& binormal)
{
	for (uint8_t face_vrt_id = 0; face_vrt_id < face_vrts_size; face_vrt_id ++) {
		uint32_t vrt_id = face_vrts[face_vrt_id];
		Submesh::VrtsSNormals::iterator result_find = result.find(vrt_id);
		HppAssert(result_find != result.end(), "This should already exist!");
		Submesh::VrtSNormal& vsn = result_find->second;
		Vector2 uv = uvs[face_vrt_id];
		Submesh::UvAndSubmeshId uv_n_si(uv, submesh_id);
		Submesh::TangentsAndBinormals::iterator ts_n_bs_find = vsn.ts_n_bs.find(uv_n_si);
		if (ts_n_bs_find == vsn.ts_n_bs.end()) {
			vsn.ts_n_bs[uv_n_si].tangent = tangent;
			vsn.ts_n_bs[uv_n_si].binormal = binormal;
		} else {
			ts_n_bs_find->second.tangent += tangent;
			ts_n_bs_find->second.binormal += binormal;
		}
	}
}

}

#endif
