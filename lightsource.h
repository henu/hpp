#ifndef HPP_LIGHTSOURCE_H
#define HPP_LIGHTSOURCE_H

#include "color.h"
#include "vector3.h"
#include "inc_gl.h"

#include <vector>

namespace Hpp
{

class Lightsource
{

	friend class Renderingenvironment;

public:

	inline Lightsource(void);

	virtual Vector3 getPosition(void) const = 0;
	virtual Color getColor(void) const = 0;

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

