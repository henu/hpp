#ifndef HPP_MESHLOADEROBJ
#define HPP_MESHLOADEROBJ

#include "meshloader.h"
#include "path.h"
#include "misc.h"
#include "exception.h"
#include "cast.h"

#include <GL/gl.h>
#include <fstream>
#include <vector>

namespace Hpp
{

class MeshloaderObj : public Meshloader
{

public:

	inline MeshloaderObj(Hpp::Path const& path);

private:

};

inline MeshloaderObj::MeshloaderObj(Hpp::Path const& path)
{
	std::ifstream file(path.toString().c_str());
	if (!file.is_open()) {
		throw Exception("Unable to open .obj-file \"" + path.toString() + "\"!");
	}

	std::vector< GLfloat > poss;
	std::vector< GLfloat > nrms;
	std::vector< GLfloat > uvs;
	std::vector< GLuint > indices_poss;
	std::vector< GLuint > indices_nrms;
	std::vector< GLuint > indices_uvs;

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
			poss.push_back(strToFloat(words[1]));
			poss.push_back(strToFloat(words[2]));
			poss.push_back(strToFloat(words[3]));
		}
		// Normal
		else if (words[0] == "vn") {
			if (words.size() < 4) {
				throw Exception("Normal components missing at line " + sizeToStr(line_num) + " in .obj-file \"" + path.toString() + "\"!");
			}
			nrms.push_back(strToFloat(words[1]));
			nrms.push_back(strToFloat(words[2]));
			nrms.push_back(strToFloat(words[3]));
		}
		// UV
		else if (words[0] == "vt") {
			if (words.size() < 3) {
				throw Exception("UV components missing at line " + sizeToStr(line_num) + " in .obj-file \"" + path.toString() + "\"!");
			}
			uvs.push_back(strToFloat(words[1]));
			uvs.push_back(strToFloat(words[2]));
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
			for (uint8_t word_id = 1; word_id <= 3; ++ word_id) {
				std::vector< std::string > parts = splitString(words[word_id], '/');
				if (parts.size() == 1) {
					indices_poss.push_back(strToSize(parts[0]));
				} else if (parts.size() == 2) {
					indices_poss.push_back(strToSize(parts[0]));
					indices_uvs.push_back(strToSize(parts[1]));
				} else if (parts.size() == 3) {
					indices_poss.push_back(strToSize(parts[0]));
					if (!parts[1].empty()) {
						indices_uvs.push_back(strToSize(parts[1]));
					}
					indices_nrms.push_back(strToSize(parts[2]));
				} else {
					throw Exception("Invalid index \"" + words[word_id] + "\" at line " + sizeToStr(line_num) + " in .obj-file \"" + path.toString() + "\"!");
				}
			}
		}

	}
	file.close();

	// Let base class do the rest
	readArrays(poss, nrms, uvs, indices_poss, indices_nrms, indices_uvs);
}

}

#endif
