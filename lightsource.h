#ifndef HPP_LIGHTSOURCE_H
#define HPP_LIGHTSOURCE_H

#include "inc_gl.h"

#include <vector>

namespace Hpp
{

class Lightsource
{

	friend class Renderingenvironment;

public:

	inline Lightsource(void);

private:

	// Functions for friends
	virtual void setUpGlLight(GLenum gl_light) const = 0;

private:

};

typedef std::vector< Lightsource* > Lightsources;

inline Lightsource::Lightsource(void)
{
}

}

#endif

