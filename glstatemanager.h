#ifndef HPP_GLSTATEMANAGER_H
#define HPP_GLSTATEMANAGER_H

// The idea of this class is to track what the state of GL
// shouldbe, what it is, and what operations can be ignored
// because they would not really change the state.

#include "inc_gl.h"

#include <cstddef>
#include <vector>

namespace Hpp
{

class Bufferobject;

class GlStatemanager
{

public:

	// Sets Bufferobject that should be at the specific vertex attribute
	static void setVertexattributeState(GLuint vertexattr_loc, Bufferobject const* bo);

	// Marks that all bindings should be unbound.
	static void clearAllVertexarrays(void);

	// This finally binds all requested
	// Bufferobjects to the vertexattributes.
	static void syncVertexarrays(void);

	// Informs that specific Bufferobject will be destroyed. If
	// Bufferobject is currently bound to some vertex attribute, it
	// will be unbound immediately. This is called by Bufferobject.
	static void markBufferobjectDestroyed(Bufferobject const* bo);

private:

	static GlStatemanager instance;

	struct Vertexattribute
	{
		inline Vertexattribute(void) : bo_bound(NULL), bo_should_be(NULL) { }
		Bufferobject const* bo_bound;
		Bufferobject const* bo_should_be;
	};
	typedef std::vector< Vertexattribute > Vertexattributes;

	Vertexattributes vattrs;

};

}

#endif
