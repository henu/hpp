#ifndef HPP_SUN_H
#define HPP_SUN_H

#include "color.h"
#include "lightsource.h"
#include "vector3.h"
#include "gldebug.h"

namespace Hpp
{

class Sun : public Lightsource
{

public:

	inline Sun(void);
	inline Sun(Color const& color, float energy = 1);

	inline void setColor(Color const& color);
	inline void setEnergy(float energy);

	inline void setDirection(Vector3 const& dir);

	inline virtual Vector3 getPosition(void) const { return dir; }
	inline virtual Color getColor(void) const { return color * energy; }

private:

	Color color;
	float energy;

	Vector3 dir;

	float color_buf[4];
	float dir_buf[4];

	inline virtual void setUpGlLight(GLenum gl_light) const;

};

inline Sun::Sun(void) :
color(1, 1, 1),
dir(0, 0, -1)
{
	color_buf[0] = color.getRed();
	color_buf[1] = color.getGreen();
	color_buf[2] = color.getBlue();
	color_buf[3] = 0.0;
	dir_buf[0] = -dir.x;
	dir_buf[1] = -dir.y;
	dir_buf[2] = -dir.z;
	dir_buf[3] = 0.0;
}

inline Sun::Sun(Color const& color, float energy) :
color(color),
energy(energy)
{
	color_buf[0] = color.getRed() * energy;
	color_buf[1] = color.getGreen() * energy;
	color_buf[2] = color.getBlue() * energy;
	color_buf[3] = 0.0;
}

inline void Sun::setColor(Color const& color)
{
	this->color = color;
	color_buf[0] = color.getRed() * energy;
	color_buf[1] = color.getGreen() * energy;
	color_buf[2] = color.getBlue() * energy;
}

inline void Sun::setEnergy(float energy)
{
	this->energy = energy;
	color_buf[0] = color.getRed() * energy;
	color_buf[1] = color.getGreen() * energy;
	color_buf[2] = color.getBlue() * energy;
}

inline void Sun::setDirection(Vector3 const& dir)
{
	this->dir = dir;
	dir_buf[0] = -dir.x;
	dir_buf[1] = -dir.y;
	dir_buf[2] = -dir.z;
	dir_buf[3] = 0.0;
}

inline void Sun::setUpGlLight(GLenum gl_light) const
{
	HppCheckGlErrors();
	glLightfv(gl_light, GL_DIFFUSE, color_buf);//DEPRECATED
	glLightfv(gl_light, GL_POSITION, dir_buf);//DEPRECATED
	glLightf(gl_light, GL_CONSTANT_ATTENUATION, 0);//DEPRECATED
	glLightf(gl_light, GL_LINEAR_ATTENUATION, 0);//DEPRECATED
	glLightf(gl_light, GL_QUADRATIC_ATTENUATION, 0);//DEPRECATED
	HppCheckGlErrors();
}

}

#endif

