#ifndef HPP_MESHLOADER
#define HPP_MESHLOADER

#include "3dutils.h"
#include "mesh.h"
#include "exception.h"
#include "cast.h"

#include <GL/gl.h>
#include <vector>
#include <map>
#include <string>

namespace Hpp
{

class Meshloader
{

public:

	inline Meshloader(void) { }

	inline Mesh* createMesh(bool calculate_tangent_and_binormal = false);

protected:

	inline void readArrays(std::vector< GLfloat > const& poss,
	                       std::vector< GLfloat > const& nrms,
	                       std::vector< GLfloat > const& uvs,
	                       std::vector< GLuint > const& indices_poss,
	                       std::vector< GLuint > const& indices_nrms,
	                       std::vector< GLuint > const& indices_uvs);

private:

	std::vector< GLfloat > poss;
	std::vector< GLfloat > nrms;
	std::vector< GLfloat > uvs;
	std::vector< GLuint > indices;

};

inline Mesh* Meshloader::createMesh(bool calculate_tangent_and_binormal)
{
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
// TODO: Make this more logical!
			tangent = binormal;
			tangent.normalize();
			//binormal.normalize();
			binormal = crossProduct(normal, tangent);
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

inline void Meshloader::readArrays(std::vector< GLfloat > const& poss,
                                   std::vector< GLfloat > const& nrms,
                                   std::vector< GLfloat > const& uvs,
                                   std::vector< GLuint > const& indices_poss,
                                   std::vector< GLuint > const& indices_nrms,
                                   std::vector< GLuint > const& indices_uvs)
{
	this->poss.clear();
	this->nrms.clear();
	this->uvs.clear();
	this->indices.clear();

	if (poss.size() % 3 != 0) {
		throw Exception("Position data should have three components per vertex!");
	}

	if (!indices_nrms.empty()) {
		if (indices_nrms.size() != indices_poss.size()) {
			throw Exception("Amount of position and normal indices is not equal!");
		}
		if (nrms.size() % 3 != 0) {
			throw Exception("Normal data should have three components per vertex!");
		}
	}
	if (!indices_uvs.empty()) {
		if (indices_uvs.size() != indices_uvs.size()) {
			throw Exception("Amount of position and UV indices is not equal!");
		}
		if (uvs.size() % 2 != 0) {
			throw Exception("UV data should have two components per vertex!");
		}
	}

	// Combine all indices into one
	std::map< std::string, GLuint > indexmapping;
	for (size_t index_id = 0; index_id < indices_poss.size(); ++ index_id) {
		std::string index_combined = sizeToStr(indices_poss[index_id]);
		index_combined += '/';
		if (!indices_nrms.empty()) {
			index_combined += sizeToStr(indices_nrms[index_id]);
		}
		index_combined += '/';
		if (!indices_uvs.empty()) {
			index_combined += sizeToStr(indices_uvs[index_id]);
		}

		// Check if index is found already
		std::map< std::string, GLuint >::const_iterator indexmapping_find = indexmapping.find(index_combined);
		if (indexmapping_find != indexmapping.end()) {
			this->indices.push_back(indexmapping_find->second);
			continue;
		}

		GLuint vertex_index = this->poss.size() / 3;

		GLuint pos_idx = indices_poss[index_id];
		this->poss.push_back(poss[(pos_idx-1)*3 + 0]);
		this->poss.push_back(poss[(pos_idx-1)*3 + 1]);
		this->poss.push_back(poss[(pos_idx-1)*3 + 2]);
		if (!indices_nrms.empty()) {
			GLuint nrm_idx = indices_nrms[index_id];
			Hpp::Vector3 normal(nrms[(nrm_idx-1)*3 + 0], nrms[(nrm_idx-1)*3 + 1], nrms[(nrm_idx-1)*3 + 2]);
			normal.normalize();
			this->nrms.push_back(normal.x);
			this->nrms.push_back(normal.y);
			this->nrms.push_back(normal.z);
		}
		if (!indices_uvs.empty()) {
			GLuint uv_idx = indices_uvs[index_id];
			this->uvs.push_back(uvs[(uv_idx-1)*2 + 0]);
			this->uvs.push_back(uvs[(uv_idx-1)*2 + 1]);
		}

		indexmapping[index_combined] = vertex_index;

		this->indices.push_back(vertex_index);
	}

	if (!this->nrms.empty() && this->poss.size() != this->nrms.size()) {
		throw Exception("Invalid number of normals in buffer!");
	}
	if (!this->uvs.empty() && this->poss.size() / 3 != this->uvs.size() / 2) {
		throw Exception("Invalid number of UVs in buffer!");
	}

}

}

#endif