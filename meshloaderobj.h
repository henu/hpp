#ifndef HPP_MESHLOADEROBJ
#define HPP_MESHLOADEROBJ

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

inline Mesh* load(Path const& path)
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
			throw Hpp::Exception("Invalid index: " + index_raw + "!");
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
		throw Hpp::Exception("Invalid number of normals in buffer!");
	}
	if (!uvs.empty() && poss.size() / 3 != uvs.size() / 2) {
		throw Hpp::Exception("Invalid number of UVs in buffer!");
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

	return result;
}

}

}

#endif
