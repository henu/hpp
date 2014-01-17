#ifndef HPP_MESHLOADER
#define HPP_MESHLOADER

#include "3dutils.h"
#include "mesh.h"
#include "exception.h"
#include "cast.h"
#include "vector2.h"
#include "vector3.h"

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

	inline size_t getNumOfTriangles(void) const { return indices.size() / 3; }
	inline size_t getNumOfVertices(void) const { return poss.size() / 3; }

	inline GLuint getTriangleCorner(GLuint triangle, uint8_t corner) const;

	inline Vector3 getVertexPosition(GLuint vertex) const;
	inline Vector3 getVertexNormal(GLuint vertex) const;
	inline Vector2 getVertexUV(GLuint vertex) const;

	inline void addTriangle(Vector3 const& pos0, Vector3 const& pos1, Vector3 const& pos2);
	inline void addTriangle(Vector3 const& pos0, Vector3 const& pos1, Vector3 const& pos2,
	                        Vector3 const& nrm0, Vector3 const& nrm1, Vector3 const& nrm2);
	inline void addTriangleWithoutNormals(Vector3 const& pos0, Vector3 const& pos1, Vector3 const& pos2,
	                                      Vector2 const& uv0, Vector2 const& uv1, Vector2 const& uv2);
	inline void addTriangle(Vector3 const& pos0, Vector3 const& pos1, Vector3 const& pos2,
	                        Vector3 const& nrm0, Vector3 const& nrm1, Vector3 const& nrm2,
	                        Vector2 const& uv0, Vector2 const& uv1, Vector2 const& uv2);

	// Recalculates normals using positions of Triangles. If
	// some triangle share same vertex, then the normal will
	// have average direction from both triangles. If there
	// are already old normals, they will be overwritten.
	inline void calculateNormals(void);

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

	// Finds index of vertex that has required properties
	// or creates new one, if nothing is found.
	inline GLuint findIndex(Vector3 const& pos,
	                        Vector3 const& nrm,
	                        Vector2 const& uv,
	                        bool use_nrm,
	                        bool use_uv);

};

inline Mesh* Meshloader::createMesh(bool calculate_tangent_and_binormal)
{
	Mesh* result = new Mesh;
	result->setBuffer(Mesh::POS, GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 3, &poss[0], poss.size());
	if (!nrms.empty()) {
		result->setBuffer(Mesh::NORMAL, GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 3, &nrms[0], nrms.size());
	}
	if (!uvs.empty()) {
		result->setBuffer(Mesh::UV, GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 2, &uvs[0], uvs.size());
	}
// TODO: In future, convert to GLushort ot GLubyte if there is only small number of vertices!
	result->setBuffer(Mesh::INDEX, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, GL_STATIC_DRAW, 3, &indices[0], indices.size());

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
			if (tangent.length() != 0) tangent.normalize();
			if (binormal.length() != 0) binormal.normalize();

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
			if (tangent.length() != 0 && binormal.length() != 0) {
				tangent.normalize();
				binormal.normalize();
				// Ensure angle between these is 90 °
				try {
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
				catch (Exception)
				{
				}
			}
		}

		result->setBuffer(Mesh::TANGENT, GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 3, &tangents[0], tangents.size());
		result->setBuffer(Mesh::BINORMAL, GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW, 3, &binormals[0], binormals.size());
	}

	return result;
}

inline GLuint Meshloader::getTriangleCorner(GLuint triangle, uint8_t corner) const
{
	return indices[triangle*3 + corner];
}

inline Vector3 Meshloader::getVertexPosition(GLuint vertex) const
{
	GLuint vrt_ofs = vertex * 3;
	return Vector3(poss[vrt_ofs + 0], poss[vrt_ofs + 1], poss[vrt_ofs + 2]);
}

inline Vector3 Meshloader::getVertexNormal(GLuint vertex) const
{
	GLuint vrt_ofs = vertex * 3;
	return Vector3(nrms[vrt_ofs + 0], nrms[vrt_ofs + 1], nrms[vrt_ofs + 2]);
}

inline Vector2 Meshloader::getVertexUV(GLuint vertex) const
{
	GLuint vrt_ofs = vertex * 2;
	return Vector2(uvs[vrt_ofs + 0], uvs[vrt_ofs + 1]);
}

inline void Meshloader::addTriangle(Vector3 const& pos0, Vector3 const& pos1, Vector3 const& pos2)
{
	HppAssert(nrms.empty(), "Unable to add triangle without normals if already added triangles have them!");
	HppAssert(uvs.empty(), "Unable to add triangle without UVs if already added triangles have them!");

	GLuint idx0 = findIndex(pos0, Vector3::ZERO, Vector2::ZERO, false, false);
	GLuint idx1 = findIndex(pos1, Vector3::ZERO, Vector2::ZERO, false, false);
	GLuint idx2 = findIndex(pos2, Vector3::ZERO, Vector2::ZERO, false, false);

	indices.push_back(idx0);
	indices.push_back(idx1);
	indices.push_back(idx2);
}

inline void Meshloader::addTriangle(Vector3 const& pos0, Vector3 const& pos1, Vector3 const& pos2,
                                    Vector3 const& nrm0, Vector3 const& nrm1, Vector3 const& nrm2)
{
	HppAssert(poss.empty() || !nrms.empty(), "Unable to add triangle with normals if already added triangles do not have them!");
	HppAssert(uvs.empty(), "Unable to add triangle without UVs if already added triangles have them!");

	GLuint idx0 = findIndex(pos0, nrm0, Vector2::ZERO, true, false);
	GLuint idx1 = findIndex(pos1, nrm1, Vector2::ZERO, true, false);
	GLuint idx2 = findIndex(pos2, nrm2, Vector2::ZERO, true, false);

	indices.push_back(idx0);
	indices.push_back(idx1);
	indices.push_back(idx2);
}

inline void Meshloader::addTriangleWithoutNormals(Vector3 const& pos0, Vector3 const& pos1, Vector3 const& pos2,
                                                  Vector2 const& uv0, Vector2 const& uv1, Vector2 const& uv2)
{
	HppAssert(nrms.empty(), "Unable to add triangle without normals if already added triangles have them!");
	HppAssert(poss.empty() || !uvs.empty(), "Unable to add triangle with UVs if already added triangles do not have them!");

	GLuint idx0 = findIndex(pos0, Vector3::ZERO, uv0, false, true);
	GLuint idx1 = findIndex(pos1, Vector3::ZERO, uv1, false, true);
	GLuint idx2 = findIndex(pos2, Vector3::ZERO, uv2, false, true);

	indices.push_back(idx0);
	indices.push_back(idx1);
	indices.push_back(idx2);
}

inline void Meshloader::addTriangle(Vector3 const& pos0, Vector3 const& pos1, Vector3 const& pos2,
                                    Vector3 const& nrm0, Vector3 const& nrm1, Vector3 const& nrm2,
                                    Vector2 const& uv0, Vector2 const& uv1, Vector2 const& uv2)
{
	HppAssert(poss.empty() || !nrms.empty(), "Unable to add triangle with normals if already added triangles do not have them!");
	HppAssert(poss.empty() || !uvs.empty(), "Unable to add triangle with UVs if already added triangles do not have them!");

	GLuint idx0 = findIndex(pos0, nrm0, uv0, true, true);
	GLuint idx1 = findIndex(pos1, nrm1, uv1, true, true);
	GLuint idx2 = findIndex(pos2, nrm2, uv2, true, true);

	indices.push_back(idx0);
	indices.push_back(idx1);
	indices.push_back(idx2);
}

void Meshloader::calculateNormals()
{
	// Reset all normals to have zero length
	nrms.assign(poss.size(), 0);

	// Go all triangles through
	for (size_t tri_id = 0; tri_id < indices.size()/3; ++ tri_id) {
		GLuint idx0 = indices[tri_id*3 + 0];
		GLuint idx1 = indices[tri_id*3 + 1];
		GLuint idx2 = indices[tri_id*3 + 2];

		Vector3 c0(poss[idx0*3 + 0], poss[idx0*3 + 1], poss[idx0*3 + 2]);
		Vector3 c1(poss[idx1*3 + 0], poss[idx1*3 + 1], poss[idx1*3 + 2]);
		Vector3 c2(poss[idx2*3 + 0], poss[idx2*3 + 1], poss[idx2*3 + 2]);

		// Calculate normal and normalize it
		Vector3 normal = crossProduct(c1 - c0, c2 - c0);
		Hpp::Real normal_len = normal.length();
		if (normal_len > 0) {
			normal /= normal_len;
		}

		// Add normals to buffer
		nrms[idx0*3 + 0] = normal.x;
		nrms[idx0*3 + 1] = normal.y;
		nrms[idx0*3 + 2] = normal.z;
		nrms[idx1*3 + 0] = normal.x;
		nrms[idx1*3 + 1] = normal.y;
		nrms[idx1*3 + 2] = normal.z;
		nrms[idx2*3 + 0] = normal.x;
		nrms[idx2*3 + 1] = normal.y;
		nrms[idx2*3 + 2] = normal.z;
	}

	// Finally go all normals through and normalize them.
	for (size_t vertex_id = 0; vertex_id < nrms.size() / 3; ++ vertex_id) {
		Hpp::Vector3 normal(nrms[vertex_id*3 + 0], nrms[vertex_id*3 + 1], nrms[vertex_id*3 + 2]);
		Hpp::Real normal_len = normal.length();
		if (normal_len > 0) {
			normal /= normal_len;
		}
		nrms[vertex_id*3 + 0] = normal.x;
		nrms[vertex_id*3 + 1] = normal.y;
		nrms[vertex_id*3 + 2] = normal.z;
	}
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
		if (indices_uvs.size() != indices_poss.size()) {
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
		this->poss.push_back(poss[pos_idx*3 + 0]);
		this->poss.push_back(poss[pos_idx*3 + 1]);
		this->poss.push_back(poss[pos_idx*3 + 2]);
		if (!indices_nrms.empty()) {
			GLuint nrm_idx = indices_nrms[index_id];
			Vector3 normal(nrms[nrm_idx*3 + 0], nrms[nrm_idx*3 + 1], nrms[nrm_idx*3 + 2]);
			normal.normalize();
			this->nrms.push_back(normal.x);
			this->nrms.push_back(normal.y);
			this->nrms.push_back(normal.z);
		}
		if (!indices_uvs.empty()) {
			GLuint uv_idx = indices_uvs[index_id];
			this->uvs.push_back(uvs[uv_idx*2 + 0]);
			this->uvs.push_back(uvs[uv_idx*2 + 1]);
		}

		indexmapping[index_combined] = vertex_index;

		this->indices.push_back(vertex_index);
	}

	if (!this->nrms.empty() && this->poss.size() != this->nrms.size()) {
		throw Exception("Invalid number of normals in buffer!");
	}
	if (!this->uvs.empty() && this->poss.size() * 2 != this->uvs.size() * 3) {
		throw Exception("Invalid number of UVs in buffer!");
	}

}

inline GLuint Meshloader::findIndex(Vector3 const& pos,
                                    Vector3 const& nrm,
                                    Vector2 const& uv,
                                    bool use_nrm,
                                    bool use_uv)
{
	Real SAME_DIFF_THRESHOLD = 0.0001;
	Real SAME_DIFF_THRESHOLD_TO_2 = SAME_DIFF_THRESHOLD * SAME_DIFF_THRESHOLD;

	for (GLuint vrt_idx = 0; vrt_idx < poss.size() / 3; ++ vrt_idx) {
		size_t ofs3 = vrt_idx * 3;
		// Check position
		Vector3 vrt_pos(poss[ofs3 + 0], poss[ofs3 + 1], poss[ofs3 + 2]);
		if ((vrt_pos - pos).lengthTo2() > SAME_DIFF_THRESHOLD_TO_2) {
			continue;
		}
		// Check normal
		if (use_nrm) {
			Vector3 vrt_nrm(nrms[ofs3 + 0], nrms[ofs3 + 1], nrms[ofs3 + 2]);
			if ((vrt_nrm - nrm).lengthTo2() > SAME_DIFF_THRESHOLD_TO_2) {
				continue;
			}
		}
		// Check UV
		if (use_uv) {
			size_t ofs2 = vrt_idx * 2;
			Vector2 vrt_uv(uvs[ofs2 + 0], uvs[ofs2 + 1]);
			if ((vrt_uv - uv).lengthTo2() > SAME_DIFF_THRESHOLD_TO_2) {
				continue;
			}
		}
		return vrt_idx;
	}

	// No vertex could be found, so create new one
	poss.push_back(pos.x);
	poss.push_back(pos.y);
	poss.push_back(pos.z);
	if (use_nrm) {
		nrms.push_back(nrm.x);
		nrms.push_back(nrm.y);
		nrms.push_back(nrm.z);
	}
	if (use_uv) {
		uvs.push_back(uv.x);
		uvs.push_back(uv.y);
	}
	return poss.size() / 3 - 1;
}

}

#endif
