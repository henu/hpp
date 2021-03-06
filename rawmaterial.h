#ifndef HPP_RAWMATERIAL_H
#define HPP_RAWMATERIAL_H

#include "serialize.h"
#include "color.h"
#include "texture.h"

#include <vector>
#include <string>

namespace Hpp
{

class Rawmaterial
{

public:

	inline Rawmaterial(void);
	inline Rawmaterial(std::istream& strm);

	// Name
	std::string name;

	// Lighting options
	Color color;
	Color specular;
	float shininess;
	float ambient_multiplier;
	float emittance;
	float normalmap_weight;

	// Textures. If pointers to real textures
	// are given, then they omit strings.
	std::string colormap;
	std::string normalmap;
	std::string specularmap;
	Texture* colormap_tex;
	Texture* normalmap_tex;
	Texture* specularmap_tex;

};
typedef std::vector< Rawmaterial > Rawmaterials;

inline Rawmaterial::Rawmaterial(void) :
color(1, 1, 1),
specular(0, 0, 0),
shininess(0),
ambient_multiplier(1),
emittance(0),
normalmap_weight(1),
colormap_tex(NULL),
normalmap_tex(NULL),
specularmap_tex(NULL)
{
}

inline Rawmaterial::Rawmaterial(std::istream& strm)
{
	name = deserializeString(strm, 4);
	float color_r = deserializeFloat(strm);
	float color_g = deserializeFloat(strm);
	float color_b = deserializeFloat(strm);
	float specular_r = deserializeFloat(strm);
	float specular_g = deserializeFloat(strm);
	float specular_b = deserializeFloat(strm);
	shininess = deserializeFloat(strm);
	ambient_multiplier = deserializeFloat(strm);
	emittance = deserializeFloat(strm);
	float alpha = deserializeFloat(strm);

	if (alpha >= 1.0) {
		color = Color(color_r, color_g, color_b);
		specular = Color(specular_r, specular_g, specular_b);
	} else {
		color = Color(color_r, color_g, color_b, alpha);
		specular = Color(specular_r, specular_g, specular_b, alpha);
	}
}

}

#endif
