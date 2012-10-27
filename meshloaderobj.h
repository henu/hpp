#ifndef HPP_MESHLOADEROBJ
#define HPP_MESHLOADEROBJ

#include "3dutils.h"
#include "mesh.h"
#include "path.h"
#include "misc.h"
#include "exception.h"
#include "cast.h"

#include <fstream>
#include <map>

namespace Hpp
{

namespace MeshloaderObj
{

inline Mesh* load(Path const& path, bool calculate_tangent_and_binormal = false)
{
	std::ifstream file(path.toString().c_str());
	if (!file.is_open()) {
		throw Exception("Unable to open .obj-file \"" + path.toString() + "\"!");

	}

	std::vector< GLfloat > poss_raw;
	std::vector< GLfloat > nrms_raw;
	std::vector< GLfloat > uvs_raw;
	std::vector< std::string > indices_raw;

	size_t line_num = 0;
	while (!file.eof()) {

		std::string line;
		getline (file, line);
		++ line_num;

		if (line.empty()) {
			continue;
		}

		// Strip whitespace from begin and end
		line = trim(line);

		// Skip comments
		if (line[0] == '#') {
			continue;
		}

		std::vector< std::string > words = splitString(line, makeVector(' ')('\t'), true);
		HppAssert(!words.empty(), "Fail!");

		// Position of vertex
		if (words[0] == "v") {
			if (words.size() < 4) {
				throw Exception("Position components missing at line " + sizeToStr(line_num) + " in .obj-file \"" + path.toString() + "\"!");
			}
			poss_raw.push_back(strToFloat(words[1]));
			poss_raw.push_back(strToFloat(words[2]));
			poss_raw.push_back(strToFloat(words[3]));
		}
		// Normal
		else if (words[0] == "vn") {
			if (words.size() < 4) {
				throw Exception("Normal components missing at line " + sizeToStr(line_num) + " in .obj-file \"" + path.toString() + "\"!");
			}
			nrms_raw.push_back(strToFloat(words[1]));
			nrms_raw.push_back(strToFloat(words[2]));
			nrms_raw.push_back(strToFloat(words[3]));
		}
		// UV
		else if (words[0] == "vt") {
			if (words.size() < 3) {
				throw Exception("UV components missing at line " + sizeToStr(line_num) + " in .obj-file \"" + path.toString() + "\"!");
			}
			uvs_raw.push_back(strToFloat(words[1]));
			uvs_raw.push_back(strToFloat(words[2]));
		}
		// Index
		else if (words[0] == "f") {
			if (words.size() < 4) {
				throw Exception("Index components missing at line " + sizeToStr(line_num) + " in .obj-file \"" + path.toString() + "\"!");
			}
// TODO: Support quads!
			if (words.size() > 4) {
				throw Exception("Quads not supported yet!");
			}
			indices_raw.push_back(words[1]);
			indices_raw.push_back(words[2]);
			indices_raw.push_back(words[3]);
		}

	}
	file.close();

	// Form final buffers
	std::vector< GLfloat > poss;
	std::vector< GLfloat > nrms;
	std::vector< GLfloat > uvs;
	std::vector< GLuint > indices;
	std::map< std::string, GLuint > indexmapping;
	for (std::vector< std::string >::const_iterator indices_raw_it = indices_raw.begin();
	     indices_raw_it != indices_raw.end();
	     ++ indices_raw_it) {
		std::string const& index_raw = *indices_raw_it;

		// Check if index is found already
		std::map< std::string, GLuint >::const_iterator indexmapping_find = indexmapping.find(index_raw);
		if (indexmapping_find != indexmapping.end()) {
			indices.push_back(indexmapping_find->second);
			continue;
		}

		// Index is nout found, so create it now
		std::vector< std::string > parts = splitString(index_raw, '/');
		GLuint pos_idx = 0;
		GLuint nrm_idx = 0;
		GLuint uv_idx = 0;
		HppAssert(!parts.empty(), "Invalid raw index!");
		if (parts.size() == 1) {
			pos_idx = strToSize(parts[0]);
			nrm_idx = strToSize(parts[0]);
			uv_idx = strToSize(parts[0]);
		} else if (parts.size() == 2) {
			pos_idx = strToSize(parts[0]);
			uv_idx = strToSize(parts[1]);
		} else if (parts.size() == 3) {
			pos_idx = strToSize(parts[0]);
			if (!parts[1].empty()) {
				uv_idx = strToSize(parts[1]);
			}
			nrm_idx = strToSize(parts[2]);
		} else {
			throw Exception("Invalid index: " + index_raw + "!");
		}

		HppAssert(poss.size() % 3 == 0, "Fail!");
		GLuint vertex_index = poss.size() / 3;

		poss.push_back(poss_raw[(pos_idx-1)*3 + 0]);
		poss.push_back(poss_raw[(pos_idx-1)*3 + 1]);
		poss.push_back(poss_raw[(pos_idx-1)*3 + 2]);
		if (nrm_idx > 0) {
			nrms.push_back(nrms_raw[(nrm_idx-1)*3 + 0]);
			nrms.push_back(nrms_raw[(nrm_idx-1)*3 + 1]);
			nrms.push_back(nrms_raw[(nrm_idx-1)*3 + 2]);
		}
		if (uv_idx > 0) {
			uvs.push_back(uvs_raw[(uv_idx-1)*2 + 0]);
			uvs.push_back(uvs_raw[(uv_idx-1)*2 + 1]);
		}

		indexmapping[index_raw] = vertex_index;

		indices.push_back(vertex_index);
	}

	if (!nrms.empty() && poss.size() != nrms.size()) {
		throw Exception("Invalid number of normals in buffer!");
	}
	if (!uvs.empty() && poss.size() / 3 != uvs.size() / 2) {
		throw Exception("Invalid number of UVs in buffer!");
	}

	Mesh* result = new Mesh;
	result->setBuffer("pos", GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 3, &poss[0], sizeof(GLfloat) * poss.size());
	if (!nrms.empty()) {
		result->setBuffer("normal", GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 3, &nrms[0], sizeof(GLfloat) * nrms.size());
	}
	if (!uvs.empty()) {
		result->setBuffer("uv", GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 2, &uvs[0], sizeof(GLfloat) * uvs.size());
	}
// TODO: In future, convert to GLushort ot GLubyte if there is only small number of vertices!
	result->setBuffer("index", GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, GL_STATIC_DRAW, 3, &indices[0], sizeof(GLfloat) * indices.size());

	// Calculate tangent and binormal if they are requested
	if (!nrms.empty() && !uvs.empty() && calculate_tangent_and_binormal) {
		std::vector< GLfloat > tangents(nrms.size(), 0);
		std::vector< GLfloat > binormals(nrms.size(), 0);

		for (size_t tri_id = 0; tri_id < indices.size()/3; ++ tri_id) {
			GLuint v0_id = indices[tri_id*3 + 0];
			GLuint v1_id = indices[tri_id*3 + 1];
			GLuint v2_id = indices[tri_id*3 + 2];

			Vector3 v0_pos(poss[v0_id*3 + 0], poss[v0_id*3 + 1], poss[v0_id*3 + 2]);
			Vector3 v1_pos(poss[v1_id*3 + 0], poss[v1_id*3 + 1], poss[v1_id*3 + 2]);
			Vector3 v2_pos(poss[v2_id*3 + 0], poss[v2_id*3 + 1], poss[v2_id*3 + 2]);
			Vector3 v0_to_v1_pos = v1_pos - v0_pos;
			Vector3 v0_to_v2_pos = v2_pos - v0_pos;

			Vector2 v0_uv(uvs[v0_id*2 + 0], uvs[v0_id*2 + 1]);
			Vector2 v1_uv(uvs[v1_id*2 + 0], uvs[v1_id*2 + 1]);
			Vector2 v2_uv(uvs[v2_id*2 + 0], uvs[v2_id*2 + 1]);
			Vector2 v0_to_v1_uv = v1_uv - v0_uv;
			Vector2 v0_to_v2_uv = v2_uv - v0_uv;

			Vector3 tangent = v0_to_v2_pos * v0_to_v2_uv.y - v0_to_v1_pos * v0_to_v2_uv.x;
			Vector3 binormal = v0_to_v1_pos * v0_to_v1_uv.x - v0_to_v2_pos * v0_to_v1_uv.y;
			tangent.normalize();
			binormal.normalize();

			tangents[v0_id*3 + 0] += tangent.x;
			tangents[v0_id*3 + 1] += tangent.y;
			tangents[v0_id*3 + 2] += tangent.z;
			tangents[v1_id*3 + 0] += tangent.x;
			tangents[v1_id*3 + 1] += tangent.y;
			tangents[v1_id*3 + 2] += tangent.z;
			tangents[v2_id*3 + 0] += tangent.x;
			tangents[v2_id*3 + 1] += tangent.y;
			tangents[v2_id*3 + 2] += tangent.z;
			binormals[v0_id*3 + 0] += binormal.x;
			binormals[v0_id*3 + 1] += binormal.y;
			binormals[v0_id*3 + 2] += binormal.z;
			binormals[v1_id*3 + 0] += binormal.x;
			binormals[v1_id*3 + 1] += binormal.y;
			binormals[v1_id*3 + 2] += binormal.z;
			binormals[v2_id*3 + 0] += binormal.x;
			binormals[v2_id*3 + 1] += binormal.y;
			binormals[v2_id*3 + 2] += binormal.z;

		}

		for (size_t v_id = 0; v_id < nrms.size()/3; ++ v_id) {
			Vector3 const normal(nrms[v_id*3 + 0], nrms[v_id*3 + 1], nrms[v_id*3 + 2]);
			Vector3 tangent(tangents[v_id*3 + 0], tangents[v_id*3 + 1], tangents[v_id*3 + 2]);
			Vector3 binormal(binormals[v_id*3 + 0], binormals[v_id*3 + 1], binormals[v_id*3 + 2]);
			// Ensure tangent and binormal are at the plane that is formed by the normal
			tangent = posToPlane(tangent, Vector3::ZERO, normal);
			binormal = posToPlane(binormal, Vector3::ZERO, normal);
			tangent.normalize();
			binormal.normalize();
			// Ensure angle between these is 90 °
			forceVectorsPerpendicular(tangent, binormal);
			tangent.normalize();
			binormal.normalize();
			// Store modified vectors
			tangents[v_id*3 + 0] = tangent.x;
			tangents[v_id*3 + 1] = tangent.y;
			tangents[v_id*3 + 2] = tangent.z;
			binormals[v_id*3 + 0] = binormal.x;
			binormals[v_id*3 + 1] = binormal.y;
			binormals[v_id*3 + 2] = binormal.z;
		}

		result->setBuffer("tangent", GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 3, &tangents[0], sizeof(GLfloat) * tangents.size());
		result->setBuffer("binormal", GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 3, &binormals[0], sizeof(GLfloat) * binormals.size());
	}

	return result;
}

}

}

#endif
