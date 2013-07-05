#include "shaderprogram.h"

#include "shaderprogramhandle.h"

namespace Hpp
{

Shaderprogramhandle* Shaderprogram::createHandle(Flags const& flags)
{
	// Ensure program is linked with these flags
	LinkedPrograms::iterator lprogs_find = lprogs.find(flags);
	if (lprogs_find == lprogs.end()) {
		linkProgram(flags);
		lprogs_find = lprogs.find(flags);
	}

	// Get pointer to program
	GLuint prog_id = lprogs_find->second;

	// Construct conversion table for uniform locations
	Shaderprogramhandle::Shortcuts uniformshortcuts;
	uniformshortcuts.reserve(uniforms.size());
	for (Strings::const_iterator uniforms_it = uniforms.begin();
	      uniforms_it != uniforms.end();
	      ++ uniforms_it) {
		std::string const& name = *uniforms_it;
		GLint uniform_location = GlSystem::GetUniformLocation(prog_id, name.c_str());
		uniformshortcuts.push_back(uniform_location);
	}

	// Check which vertex attributes exist and
	// ensure they are in correct locations.
	Bools existing_vertexattrs;
	existing_vertexattrs.reserve(vertexattrs.size());
	for (GLuint vertexattr_loc = 0;
	     vertexattr_loc < vertexattrs.size();
	     ++ vertexattr_loc) {
		std::string const& name = vertexattrs[vertexattr_loc];
		GLint vertexattr_loc_check = GlSystem::GetAttribLocation(prog_id, name.c_str());
		if (vertexattr_loc_check >= 0) {
			existing_vertexattrs.push_back(true);
			if (vertexattr_loc_check != GLint(vertexattr_loc)) {
				throw Exception("Vertexattribute \"" + name + "\" is in wrong location!");
			}
		} else {
			existing_vertexattrs.push_back(false);
		}
	}

	return new Shaderprogramhandle(this, prog_id, uniformshortcuts, existing_vertexattrs);
}

}
