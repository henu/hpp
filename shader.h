#ifndef HPP_SHADER_H
#define HPP_SHADER_H

#include "path.h"

#include <string>
#include <stdint.h>


namespace Hpp
{

class Shader
{

public:

	// ----------------------------------------
	// Public types
	// ----------------------------------------

	// Type of shader
	enum Type { FRAGMENT_SHADER,
	            VERTEX_SHADER };


	// ----------------------------------------
	// Public functions
	// ----------------------------------------

	// Constructor and destructor
	inline Shader(void);

	// Loads shader from file/src
	inline void load(Path const& path, Type type);
	inline void load(std::string const& src, Type type);

	inline std::string getSource(void) const { return src; }
	inline Type getType(void) const { return type; }

private:

	std::string src;
	Type type;

};

inline Shader::Shader(void)
{
}

inline void Shader::load(Path const& path, Type type)
{
	// Load shader source
	std::string src;
	std::ifstream file(path.toString().c_str(), std::ios_base::binary);
	if (!file.is_open()) {
		throw Exception("Unable to open file \"" + path.toString() + "\" for reading!");
	}
	while (!file.eof()) {
		src += static_cast< unsigned char >(file.get());
	}
	file.close();
	src.resize(src.size()-1);

	// Do actual loading
	load(src, type);
}

inline void Shader::load(std::string const& src, Type type)
{
	this->src = src;
	this->type = type;
}

}

#endif
