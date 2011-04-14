#ifndef HPP_RAWMESH_H
#define HPP_RAWMESH_H

#include "vector3.h"
#include "serialize.h"
#include "path.h"
#include "exception.h"
#include "vertexgroupinfluences.h"
#include "rawmaterial.h"
#include "skeleton.h"

#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>

namespace Hpp
{

class Rawmesh
{

public:

	struct Vertex
	{
		Vector3 pos;
		VGInfls vginfls;
	};
	typedef std::vector< Vertex > Vertices;

	typedef std::vector< std::string > VGroups;

	typedef std::vector< std::vector< Real > > FaceUVs;
	struct FaceLayer
	{
		// The size of outer vector is same as the number of vertices
		// (for example three for triangles or four for quads).
		FaceUVs uvs;
	};
	typedef std::vector< FaceLayer > FaceLayers;

	struct Tri
	{
		uint32_t vrts[3];
		FaceLayers layers;
		bool smooth;
	};
	typedef std::vector< Tri > Tris;

	struct Quad
	{
		uint32_t vrts[4];
		FaceLayers layers;
		bool smooth;
	};
	typedef std::vector< Quad > Quads;

	typedef std::vector< uint8_t > UVComponentCounts;
	struct Submesh
	{
		UVComponentCounts uvcompcounts;
		Tris tris;
		Quads quads;
		std::string mat;
		bool vgis_exist;
	};
	typedef std::vector< Submesh > Submeshes;

	enum Halvingstyle { V0_TO_V2,
	                    V1_TO_V3,
	                    SHORTER,
	                    LONGER };

	inline Rawmesh(void);
	inline Rawmesh(Path const& p);
	inline Rawmesh(std::istream& strm);

	// Read data which is in specific format
	inline void readVersion100(std::istream& strm);

	// Converts quad to triangles
	inline static void convertQuadToTris(Tri& tri1, Tri& tri2, Quad const& quad, Halvingstyle hstyle = V0_TO_V2, Vertices const* vrts = NULL);

	// Public members;
	std::string name;
	Vertices vrts;
	VGroups vgroups;
	Submeshes submeshes;
	Rawmaterials mats;
	bool skeleton_exists;
	Skeleton skeleton;

private:

	inline void detectVersionAndRead(std::istream& strm);

};

inline Rawmesh::Rawmesh(void)
{
}

inline Rawmesh::Rawmesh(Path const& p)
{
	std::ifstream file(p.toString().c_str(), std::ios_base::binary);
	if (!file.is_open()) {
		throw Exception("Unable to open file \"" + p.toString() + "\"!");
	}

	detectVersionAndRead(file);

	file.close();
}

inline Rawmesh::Rawmesh(std::istream& strm)
{
	detectVersionAndRead(strm);
}

inline void Rawmesh::readVersion100(std::istream& strm)
{
	// Clear possible old data
	vrts.clear();
	vgroups.clear();
	submeshes.clear();
	mats.clear();

	// Name
	name = deserializeString(strm, 4);

	// Vertices
	uint32_t vrts_size = deserializeUInt32(strm);
	vrts.reserve(vrts_size);
	while (vrts.size() < vrts_size) {
		Vertex new_vrt;
		new_vrt.pos = deserializeVector3(strm);
		uint32_t vginfls_size = deserializeUInt32(strm);
		while (new_vrt.vginfls.size() < vginfls_size) {
			uint32_t vg_id = deserializeUInt32(strm);
			if (new_vrt.vginfls.find(vg_id) != new_vrt.vginfls.end()) {
				throw Exception("Found vertex that has vertexgroup #" + sizeToStr(vg_id) + " defined twice!");
			}
			new_vrt.vginfls[vg_id] = deserializeFloat(strm);
		}
		vrts.push_back(new_vrt);
	}

	// Vertexgroups
	uint32_t vgroups_size = deserializeUInt32(strm);
	vgroups.reserve(vgroups_size);
	while (vgroups.size() < vgroups_size) {
		std::string vgroup_name = deserializeString(strm, 4);
		if (!vgroup_name.empty() && std::find(vgroups.begin(), vgroups.end(), vgroup_name) != vgroups.end()) {
			throw Exception("Vertexgroup " + vgroup_name + " defined twice!");
		}
		vgroups.push_back(vgroup_name);
	}

	// Submeshes
	uint32_t submeshes_size = deserializeUInt32(strm);
	submeshes.reserve(submeshes_size);
	while (submeshes.size() < submeshes_size) {
		Submesh new_submesh;
		new_submesh.vgis_exist = false;

		// UV Component counts
		uint32_t uvcompcounts_size = deserializeUInt32(strm);
		new_submesh.uvcompcounts.reserve(uvcompcounts_size);
		while (new_submesh.uvcompcounts.size() < uvcompcounts_size) {
			new_submesh.uvcompcounts.push_back(deserializeUInt8(strm));
		}

		// Triangles
		uint32_t tris_size = deserializeUInt32(strm);
		new_submesh.tris.reserve(tris_size);
		while (new_submesh.tris.size() < tris_size) {
			Tri new_tri;
			// Vertices
			new_tri.vrts[0] = deserializeUInt32(strm);
			new_tri.vrts[1] = deserializeUInt32(strm);
			new_tri.vrts[2] = deserializeUInt32(strm);
			if (new_tri.vrts[0] >= vrts.size() ||
			    new_tri.vrts[1] >= vrts.size() ||
			    new_tri.vrts[2] >= vrts.size()) {
				throw Exception("Found triangle that has reference to vertex that does not exist!");
			}
			// Layers
			uint32_t layers_size = new_submesh.uvcompcounts.size();
			new_tri.layers.reserve(layers_size);
			while (new_tri.layers.size() < layers_size) {
				FaceLayer new_layer;
				uint8_t uvcomps = new_submesh.uvcompcounts[new_tri.layers.size()];
				for (uint8_t vrts_id = 0; vrts_id < 3; vrts_id ++) {
					std::vector< float > uv;
					uv.reserve(uvcomps);
					while (uv.size() < uvcomps) {
						uv.push_back(deserializeFloat(strm));
					}
					new_layer.uvs.push_back(uv);
				}
				new_tri.layers.push_back(new_layer);
			}
			new_tri.smooth = deserializeUInt8(strm);
			new_submesh.tris.push_back(new_tri);
			// Check if vertexgroup influences are used
			if (!new_submesh.vgis_exist) {
				for (uint8_t vrt_id = 0; vrt_id < 3; vrt_id ++) {
					Vertex const& vrt = vrts[new_tri.vrts[vrt_id]];
					if (!vrt.vginfls.empty()) {
						new_submesh.vgis_exist = true;
					}
				}
			}
		}

		// Quads
		uint32_t quads_size = deserializeUInt32(strm);
		new_submesh.quads.reserve(quads_size);
		while (new_submesh.quads.size() < quads_size) {
			Quad new_quad;
			// Vertices
			new_quad.vrts[0] = deserializeUInt32(strm);
			new_quad.vrts[1] = deserializeUInt32(strm);
			new_quad.vrts[2] = deserializeUInt32(strm);
			new_quad.vrts[3] = deserializeUInt32(strm);
			if (new_quad.vrts[0] >= vrts.size() ||
			    new_quad.vrts[1] >= vrts.size() ||
			    new_quad.vrts[2] >= vrts.size() ||
			    new_quad.vrts[3] >= vrts.size()) {
				throw Exception("Found quad that has reference to vertex that does not exist!");
			}
			// Layers
			uint32_t layers_size = new_submesh.uvcompcounts.size();
			new_quad.layers.reserve(layers_size);
			while (new_quad.layers.size() < layers_size) {
				FaceLayer new_layer;
				uint8_t uvcomps = new_submesh.uvcompcounts[new_quad.layers.size()];
				for (uint8_t vrts_id = 0; vrts_id < 4; vrts_id ++) {
					std::vector< float > uv;
					uv.reserve(uvcomps);
					while (uv.size() < uvcomps) {
						uv.push_back(deserializeFloat(strm));
					}
					new_layer.uvs.push_back(uv);
				}
				new_quad.layers.push_back(new_layer);
			}
			new_quad.smooth = deserializeUInt8(strm);
			new_submesh.quads.push_back(new_quad);
			// Check if vertexgroup influences are used
			if (!new_submesh.vgis_exist) {
				for (uint8_t vrt_id = 0; vrt_id < 4; vrt_id ++) {
					Vertex const& vrt = vrts[new_quad.vrts[vrt_id]];
					if (!vrt.vginfls.empty()) {
						new_submesh.vgis_exist = true;
					}
				}
			}
		}

		// Material
		new_submesh.mat = deserializeString(strm, 4);

		submeshes.push_back(new_submesh);
	}

	// Skeleton
	skeleton_exists = deserializeUInt8(strm);
	if (skeleton_exists) {
		skeleton.readVersion100(strm);
	}

	// Materials
	uint32_t mats_size = deserializeUInt32(strm);
	while (mats.size() < mats_size) {
		mats.push_back(Rawmaterial(strm));
	}

}

inline void Rawmesh::convertQuadToTris(Tri& tri1, Tri& tri2, Quad const& quad, Halvingstyle hstyle, Vertices const* vrts)
{
	HppAssert(tri1.layers.empty(), "Layers of triangles must be empty!");
	HppAssert(tri2.layers.empty(), "Layers of triangles must be empty!");
	// Decide final halving style
	bool from_v0_to_v2;
	if (hstyle == V0_TO_V2) {
		from_v0_to_v2 = true;
	} else if (hstyle == V1_TO_V3) {
		from_v0_to_v2 = false;
	} else {
		// Ensure positions of vertices are provided
		HppAssert(vrts, "Vertices are not provided! Hence automatical halving style can not be done!");
		Vector3 diff_v0_to_v2 = (*vrts)[quad.vrts[0]].pos - (*vrts)[quad.vrts[2]].pos;
		Vector3 diff_v1_to_v3 = (*vrts)[quad.vrts[1]].pos - (*vrts)[quad.vrts[3]].pos;
		if (diff_v0_to_v2.lengthTo2() < diff_v1_to_v3.lengthTo2()) {
			from_v0_to_v2 = (hstyle == SHORTER);
		} else {
			from_v0_to_v2 = (hstyle == LONGER);
		}
	}

	// Other options
	tri1.smooth = quad.smooth;
	tri2.smooth = quad.smooth;
	// Vertices
	tri1.vrts[0] = quad.vrts[0];
	tri1.vrts[1] = quad.vrts[1];
	tri2.vrts[0] = quad.vrts[2];
	tri2.vrts[1] = quad.vrts[3];
	if (from_v0_to_v2) {
		tri1.vrts[2] = quad.vrts[2];
		tri2.vrts[2] = quad.vrts[0];
	} else {
		tri1.vrts[2] = quad.vrts[3];
		tri2.vrts[2] = quad.vrts[1];
	}
	// UV layers
	tri1.layers.reserve(quad.layers.size());
	tri2.layers.reserve(quad.layers.size());
	for (FaceLayers::const_iterator quad_layers_it = quad.layers.begin();
	     quad_layers_it != quad.layers.end();
	     quad_layers_it ++) {
	     	FaceLayer const& quad_layer = *quad_layers_it;
	     	FaceLayer tri1_layer;
	     	FaceLayer tri2_layer;
	     	tri1_layer.uvs.reserve(3);
	     	tri2_layer.uvs.reserve(3);
	     	tri1_layer.uvs.push_back(quad_layer.uvs[0]);
	     	tri1_layer.uvs.push_back(quad_layer.uvs[1]);
	     	tri2_layer.uvs.push_back(quad_layer.uvs[2]);
	     	tri2_layer.uvs.push_back(quad_layer.uvs[3]);
		if (from_v0_to_v2) {
			tri1_layer.uvs.push_back(quad_layer.uvs[2]);
			tri2_layer.uvs.push_back(quad_layer.uvs[0]);
		} else {
			tri1_layer.uvs.push_back(quad_layer.uvs[3]);
			tri2_layer.uvs.push_back(quad_layer.uvs[1]);
		}
		tri1.layers.push_back(tri1_layer);
		tri2.layers.push_back(tri2_layer);
	}
}

inline void Rawmesh::detectVersionAndRead(std::istream& strm)
{
	// Read version
	char buf[32];
	strm.read(buf, 32);
	if (strm.eof()) {
		throw Exception("Hpp mesh file does not contain valid header!");
	}
	if (strncmp(buf, "HPP_MESH_1.00                   ", 32) == 0) {
		readVersion100(strm);
	}
}

}

#endif
