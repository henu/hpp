#ifndef HPP_SUBMESH_H
#define HPP_SUBMESH_H

#include "noncopyable.h"
#include "renderable.h"
#include "vertexgroupinfluences.h"
#include "real.h"
#include "renderindex.h"
#include "rendbuf.h"
#include "rawmesh.h"
#include "3dutils.h"

#include <vector>
#include <set>
#include <map>

namespace Hpp
{

class Submesh : public NonCopyable
{

	friend class Subentity;

public:

	// Type and container for shared normals (and tangents and binormals)
	// of smooth faces. Tangents and binormals are indexed by UV
	// coordinates and submesh IDs.
	struct TangentAndBinormal
	{
		Vector3 tangent;
		Vector3 binormal;
	};
	struct UvAndSubmeshId
	{
		Vector2 uv;
		size_t submesh_id;
		inline UvAndSubmeshId(Vector2 const& uv, size_t submesh_id) : uv(uv), submesh_id(submesh_id) { }
		inline bool operator<(UvAndSubmeshId const& i) const
		{
			if (uv < i.uv) return true;
			if (i.uv < uv) return false;
			return submesh_id < i.submesh_id;
		}
	};
	typedef std::map< UvAndSubmeshId, TangentAndBinormal > TangentsAndBinormals;
	struct VrtSNormal
	{
		Vector3 nrm;
		TangentsAndBinormals ts_n_bs;
	};
	typedef std::map< uint32_t, VrtSNormal > VrtsSNormals;

	// Quads are not supported, so they need to be
	// merged with triangles to one big container.
	inline Submesh(size_t submesh_id,
	               Rawmesh::Submesh const& rawsubmesh,
	               Rawmesh::Tris const& rawsubmesh_tris,
	               Rawmesh::Vertices const& vrts,
	               VrtsSNormals const& vrts_snormal);
	inline ~Submesh(void);

	inline std::string getDefaultMaterial(void) const { return default_mat; }

	// Calculates tangent and binormal of tangentspace, based on positions
	// and UV coordinates.
	inline static void calculateTangentspace(Vector3& tangent,
	                                         Vector3& binormal,
	                                         Vector3 const* poss,
	                                         Vector2 const* uvs);

	// Functions to get some statistics of the volume that vertices use.
	// This is mainly used for calculation of default boundingsphere.
	inline void getVertexBoundings(Vector3& vrts_min, Vector3& vrts_max) const;
	inline void getMaxVertexDistance(Real& result_to_2, Vector3 const& pos) const;

private:

	// ----------------------------------------
	// Types for friends
	// ----------------------------------------

	// Basic vertex stuff
	Rendbuf< Real > poss;
	Rendbuf< Real > nrms;
	std::vector< Rendbuf< Real >* > uvs;
	Rendbuf< Real > clrs;

	// Faces
	Rendbuf< RIdx > tris;

	// Vertex group influences
	Rendbuf< RIdx > vg_ids;
	Rendbuf< Real > vg_weights;

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	inline Renderable* createRenderable(void) const;

private:

	// ----------------------------------------
	// Data
	// ----------------------------------------

	std::string default_mat;


	// ----------------------------------------
	// Private functions
	// ----------------------------------------

	// These functions are used when loading
	inline void addFaceToStaticparts(size_t submesh_id,
	                                 Rawmesh::Tri const& tri,
	                                 Rawmesh::Vertices const& vrts,
	                                 VrtsSNormals const& vrts_snormal,
	                                 bool uvs_exist,
	                                 bool vgis_exist);

	inline static TangentsAndBinormals::const_iterator findBestTangentAndBinormal(TangentsAndBinormals const& ts_n_bs, UvAndSubmeshId const& uv_n_si);

};
typedef std::vector< Submesh* > Submeshes;

inline Submesh::Submesh(size_t submesh_id,
                        Rawmesh::Submesh const& rawsubmesh,
                        Rawmesh::Tris const& rawsubmesh_tris,
                        Rawmesh::Vertices const& vrts,
                        VrtsSNormals const& vrts_snormal) :
default_mat(rawsubmesh.mat)
{
	bool uvs_exist = !rawsubmesh.uvcompcounts.empty() && rawsubmesh.uvcompcounts[0] == 2;
	bool vgis_exist = rawsubmesh.vgis_exist;

	for (Rawmesh::Tris::const_iterator tris_it = rawsubmesh_tris.begin();
	     tris_it != rawsubmesh_tris.end();
	     ++ tris_it) {
	     	Rawmesh::Tri const& tri = *tris_it;
		addFaceToStaticparts(submesh_id, tri, vrts, vrts_snormal, uvs_exist, vgis_exist);
	}

	poss.convertToVBO(RendbufEnums::ARRAY);
	nrms.convertToVBO(RendbufEnums::ARRAY);
	if (uvs_exist) {
		HppAssert(uvs.size() == 3, "Fail!");
		uvs[0]->convertToVBO(RendbufEnums::ARRAY);
		uvs[1]->convertToVBO(RendbufEnums::ARRAY);
		uvs[2]->convertToVBO(RendbufEnums::ARRAY);
// TODO: Current solution produces too many vertices! Code system that makes vertices one, even if their tangent space values differ little bit...
	}
	clrs.convertToVBO(RendbufEnums::ARRAY);
	tris.convertToVBO(RendbufEnums::ELEMENTS);
	vg_ids.convertToVBO(RendbufEnums::ARRAY);
	vg_weights.convertToVBO(RendbufEnums::ARRAY);

}

inline Submesh::~Submesh(void)
{
	for (std::vector< Rendbuf< Real >* >::iterator uvs_it = uvs.begin();
	     uvs_it != uvs.end();
	     ++ uvs_it) {
		delete *uvs_it;
	}
}

inline void Submesh::calculateTangentspace(Vector3& tangent,
                                           Vector3& binormal,
                                           Vector3 const* poss,
                                           Vector2 const* uvs)
{
	// Precalculate some difference values
	Vector3 v1v0 = poss[1] - poss[0];
	Vector3 v2v0 = poss[2] - poss[0];
	Vector2 c1c0 = uvs[1] - uvs[0];
	Vector2 c2c0 = uvs[2] - uvs[0];

	// Calculate determinant
	Real d = c1c0.x * c2c0.y - c2c0.x * c1c0.y;
	HppAssert(d != 0.0, "Divizion by zero!");
	Real one_by_d = 1.0 / d;
	// Calculate vectors
	tangent = one_by_d * (c2c0.y * v1v0 - c1c0.y * v2v0);
	binormal = one_by_d * (c2c0.x * v1v0 - c1c0.x * v2v0);

// TODO: If these fail, then replace assert with if!
	HppAssert(tangent.lengthTo2() > 0.0, "Divizion by zero!");
	HppAssert(binormal.lengthTo2() > 0.0, "Divizion by zero!");

	// Force angle between vectors to be 90°
	forceVectorsPerpendicular(tangent, binormal);

	// Do normalizations
	tangent.normalize();
	binormal.normalize();

}

inline void Submesh::getVertexBoundings(Vector3& vrts_min, Vector3& vrts_max) const
{
	for (size_t poss_ofs = 0; poss_ofs < poss.size(); poss_ofs += 3) {
		Vector3 vrt_pos(poss[poss_ofs + 0], poss[poss_ofs + 1], poss[poss_ofs + 2]);
		vrts_min.x = std::min(vrts_min.x, vrt_pos.x);
		vrts_min.y = std::min(vrts_min.y, vrt_pos.y);
		vrts_min.z = std::min(vrts_min.z, vrt_pos.z);
		vrts_max.x = std::max(vrts_max.x, vrt_pos.x);
		vrts_max.y = std::max(vrts_max.y, vrt_pos.y);
		vrts_max.z = std::max(vrts_max.z, vrt_pos.z);
	}
}

inline void Submesh::getMaxVertexDistance(Real& result_to_2, Vector3 const& pos) const
{
	for (size_t poss_ofs = 0; poss_ofs < poss.size(); poss_ofs += 3) {
		Vector3 vrt_pos(poss[poss_ofs + 0], poss[poss_ofs + 1], poss[poss_ofs + 2]);
		Real dst_to_2 = (vrt_pos - pos).lengthTo2();
		result_to_2 = std::max(result_to_2, dst_to_2);
	}
}

inline Renderable* Submesh::createRenderable(void) const
{
	Renderable* new_renderable = new Renderable(&poss, &nrms, uvs, &clrs, &tris);
// TODO: Uncomment these
/*
	if (!vg_ids.empty()) {
		HppAssert(vg_ids.size() == vg_weights.size(), "Invalid sizes!");
		new_renderable->setGenericBuf("vg_ids", vg_ids);
		new_renderable->setGenericBuf("vg_weights", vg_weights);
	}
*/
	return new_renderable;
}

inline void Submesh::addFaceToStaticparts(size_t submesh_id,
                                          Rawmesh::Tri const& tri,
                                          Rawmesh::Vertices const& vrts,
                                          VrtsSNormals const& vrts_snormal,
                                          bool uvs_exist,
                                          bool vgis_exist)
{
//	VGInfls const& vgi = vrts[tri.vrts[0]].vginfls;

	if (uvs_exist && uvs.empty()) {
		uvs.reserve(3);
		for (uint8_t i = 0; i < 3; i ++) {
// TODO: Should these rendbufs be autoreleasing ones?
			uvs.push_back(new Rendbuf< Real >());
		}
	}

/*
	// First find correct staticpart
	Staticpart* spart = NULL;
	for (Staticparts::iterator sparts_it = sparts.begin();
	     sparts_it != sparts.end();
	     ++ sparts_it) {
	     	if (sparts_it->vginfls == vgi) {
	     		spart = &*sparts_it;
	     		break;
	     	}
	}
	if (!spart) {
		Staticpart new_spart;
		new_spart.vginfls = vgi;
		if (uvs_exist) {
			new_spart.uvs.assign(3, Rendbuf< Real >());
		}
		sparts.push_back(new_spart);
		spart = &sparts.back();
	}
*/

	// Find vertices for this face. Because faces can be smooth or solid,
	// we cannot just try to begin find vertices.
	// First we need to calculate real normals of this face.
	Vector3 vrts_pos[3];
	Vector3 vrts_nrm[3];
	Vector2 vrts_uv[3];
	Vector3 vrts_tangent[3];
	Vector3 vrts_binormal[3];
	VGInfls vrts_vgi[3];
	for (uint8_t face_vrt_id = 0; face_vrt_id < 3; face_vrt_id ++) {
		uint32_t vrt_id = tri.vrts[face_vrt_id];
		vrts_pos[face_vrt_id] = vrts[vrt_id].pos;
		if (vgis_exist) {
			vrts_vgi[face_vrt_id] = calculateFixedVertexgroupinfluence(vrts[vrt_id].vginfls);
		}
		if (tri.smooth) {
			VrtsSNormals::const_iterator vrts_snormal_find = vrts_snormal.find(vrt_id);
			HppAssert(vrts_snormal_find != vrts_snormal.end(), "Static normal was not found!");
			HppAssert(vrts_snormal_find->second.nrm.lengthTo2() > 0.999 && vrts_snormal_find->second.nrm.lengthTo2() < 1.001, "Normal is not normalized!");
			vrts_nrm[face_vrt_id] = vrts_snormal_find->second.nrm;
			if (uvs_exist) {
				HppAssert(!tri.layers.empty(), "No layers!");
				HppAssert(tri.layers[0].uvs[0].size() == 2, "Two component UVs was expected!");
				std::vector< Real > const& uv_raw = tri.layers[0].uvs[face_vrt_id];
				Vector2 uv(uv_raw[0], uv_raw[1]);
				vrts_uv[face_vrt_id] = uv;
				UvAndSubmeshId uv_n_si(uv, submesh_id);
				// Find nearest tangent and binormal
				TangentsAndBinormals::const_iterator ts_n_bs_it = findBestTangentAndBinormal(vrts_snormal_find->second.ts_n_bs, uv_n_si);
				vrts_tangent[face_vrt_id] = ts_n_bs_it->second.tangent;
				vrts_binormal[face_vrt_id] = ts_n_bs_it->second.binormal;
				HppAssert(fabs(dotProduct(ts_n_bs_it->second.tangent, vrts_snormal_find->second.nrm)) < 0.01, "Needs 90° diff!");
				HppAssert(fabs(dotProduct(ts_n_bs_it->second.binormal, vrts_snormal_find->second.nrm)) < 0.01, "Needs 90° diff!");
			} else {
				vrts_uv[face_vrt_id] = Vector2::ZERO;
				vrts_tangent[face_vrt_id] = Vector3::ZERO;
				vrts_binormal[face_vrt_id] = Vector3::ZERO;
			}
		}
	}
	if (!tri.smooth) {
		Vector3 normal = crossProduct(vrts_pos[1] - vrts_pos[0], vrts_pos[2] - vrts_pos[0]).normalized();
		vrts_nrm[0] = normal;
		vrts_nrm[1] = normal;
		vrts_nrm[2] = normal;
		if (uvs_exist) {
			HppAssert(!tri.layers.empty(), "No layers!");
			HppAssert(tri.layers[0].uvs[0].size() == 2, "Two component UVs was expected!");
			vrts_uv[0] = Vector2(tri.layers[0].uvs[0][0], tri.layers[0].uvs[0][1]);
			vrts_uv[1] = Vector2(tri.layers[0].uvs[1][0], tri.layers[0].uvs[1][1]);
			vrts_uv[2] = Vector2(tri.layers[0].uvs[2][0], tri.layers[0].uvs[2][1]);
			Vector3 tangent;
			Vector3 binormal;
			calculateTangentspace(tangent, binormal, vrts_pos, vrts_uv);
			vrts_tangent[0] = tangent;
			vrts_tangent[1] = tangent;
			vrts_tangent[2] = tangent;
			vrts_binormal[0] = binormal;
			vrts_binormal[1] = binormal;
			vrts_binormal[2] = binormal;
		} else {
			vrts_uv[0] = Vector2::ZERO;
			vrts_uv[1] = Vector2::ZERO;
			vrts_uv[2] = Vector2::ZERO;
			vrts_tangent[0] = Vector3::ZERO;
			vrts_tangent[1] = Vector3::ZERO;
			vrts_tangent[2] = Vector3::ZERO;
			vrts_binormal[0] = Vector3::ZERO;
			vrts_binormal[1] = Vector3::ZERO;
			vrts_binormal[2] = Vector3::ZERO;
		}
	}
	for (uint8_t face_vrt_id = 0; face_vrt_id < 3; face_vrt_id ++) {

		// Form identification information
		Vector3 vrt_pos = vrts_pos[face_vrt_id];
		Vector3 vrt_nrm = vrts_nrm[face_vrt_id];
		Vector2 vrt_uv = vrts_uv[face_vrt_id];
		Vector3 vrt_tangent = vrts_tangent[face_vrt_id];
		Vector3 vrt_binormal = vrts_binormal[face_vrt_id];
		VGInfls const& vrt_vgi = vrts_vgi[face_vrt_id];

		// Now try to find vertex that would have these same values.
		HppAssert(poss.size() % 3 == 0, "Not multiple of nine!");
		HppAssert(nrms.size() % 3 == 0, "Not multiple of nine!");
		uint32_t vrts_size = poss.size() / 3;
		int32_t matching_vrt_id = -1;
		Real const DIFF_THRESHOLD = 0.001 * 0.001;
		Real const DIFF_THRESHOLD_TO_2 = DIFF_THRESHOLD * DIFF_THRESHOLD;
		for (uint32_t vrt_id = 0; vrt_id < vrts_size; vrt_id ++) {
			uint32_t offset4 = vrt_id * 4;
			uint32_t offset3 = vrt_id * 3;
			uint32_t offset2 = vrt_id * 2;
			// Skip those vertices that have different position or
			// normal.
			if ((Vector3(poss[offset3 + 0], poss[offset3 + 1], poss[offset3 + 2]) - vrt_pos).lengthTo2() > DIFF_THRESHOLD_TO_2 ||
			    (Vector3(nrms[offset3 + 0], nrms[offset3 + 1], nrms[offset3 + 2]) - vrt_nrm).lengthTo2() > DIFF_THRESHOLD_TO_2) {
			    	continue;
			}
			// Also skip those with different UV coordinates.
			if (uvs_exist &&
			    ((Vector2((*uvs[0])[offset2 + 0], (*uvs[0])[offset2 + 1]) - vrt_uv).lengthTo2() > DIFF_THRESHOLD_TO_2 ||
			     (Vector3((*uvs[1])[offset3 + 0], (*uvs[1])[offset3 + 1], (*uvs[1])[offset3 + 2]) - vrt_tangent).lengthTo2() > DIFF_THRESHOLD_TO_2 ||
			     (Vector3((*uvs[2])[offset3 + 0], (*uvs[2])[offset3 + 1], (*uvs[2])[offset3 + 2]) - vrt_binormal).lengthTo2() > DIFF_THRESHOLD_TO_2)) {
				continue;
			}
			// And finally check vertexgroup influences
			if (vgis_exist && !vgiNearEnough(vg_ids, vg_weights, offset4, vrt_vgi, DIFF_THRESHOLD_TO_2)) {
				continue;
			}
			matching_vrt_id = vrt_id;
			break;
		}
		// If matching vertex was not found
		if (matching_vrt_id < 0) {
			poss.push_back_v3(vrt_pos);
			nrms.push_back_v3(vrt_nrm);
			if (uvs_exist) {
				HppAssert(uvs.size() == 3, "Fail!");
				uvs[0]->push_back_v2(vrt_uv);
				uvs[1]->push_back_v3(vrt_tangent);
				uvs[2]->push_back_v3(vrt_binormal);
			}
			if (vgis_exist) {
				uint8_t extrafills = 4 - vrt_vgi.size();
				for (VGInfls::const_iterator vrt_vgi_it = vrt_vgi.begin();
				     vrt_vgi_it != vrt_vgi.end();
				     ++ vrt_vgi_it) {
				     	vg_ids.push_back(vrt_vgi_it->first);
				     	vg_weights.push_back(vrt_vgi_it->second);
				}
				for (uint8_t fill = 0; fill < extrafills; fill ++) {
				     	vg_ids.push_back(0);
				     	vg_weights.push_back(-1.0);
				}
			}
			matching_vrt_id = vrts_size;
		}
		tris.push_back(matching_vrt_id);

	}
}

inline Submesh::TangentsAndBinormals::const_iterator Submesh::findBestTangentAndBinormal(TangentsAndBinormals const& ts_n_bs, UvAndSubmeshId const& uv_n_si)
{
	TangentsAndBinormals::const_iterator result = ts_n_bs.end();
	Real nearest_to_2 = 99999999;
	for (TangentsAndBinormals::const_iterator ts_n_bs_it = ts_n_bs.begin();
	     ts_n_bs_it != ts_n_bs.end();
	     ++ ts_n_bs_it) {
		UvAndSubmeshId const& key = ts_n_bs_it->first;
		if (key.submesh_id == uv_n_si.submesh_id) {
			Real diff = (key.uv - uv_n_si.uv).lengthTo2();
			if (diff < nearest_to_2) {
				nearest_to_2 = diff;
				result = ts_n_bs_it;
			}
		}
	}
	HppAssert(result != ts_n_bs.end(), "Nothing was found, but at least one was expected!");
	return result;
}

}

#endif
