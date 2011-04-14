#ifndef HPP_LAMP_H
#define HPP_LAMP_H

#include "color.h"
#include "movable.h"
#include "lightsource.h"
#include "vector3.h"

namespace Hpp
{

class Lamp : public Lightsource, public Movable
{

public:

	inline Lamp(void);
	inline Lamp(Color const& color, float energy = 1);

	inline void setColor(Color const& color);
	inline void setEnergy(float energy);

	inline Color getColor(void) const;

private:

	Color color;
	float energy;

	float color_buf[4];

	inline virtual void setUpGlLight(GLenum gl_light) const;

};

inline Lamp::Lamp(void)
{
	color_buf[3] = 0.0;
}

inline Lamp::Lamp(Color const& color, float energy) :
color(color),
energy(energy)
{
	color_buf[0] = color.getRed() * energy;
	color_buf[1] = color.getGreen() * energy;
	color_buf[2] = color.getBlue() * energy;
	color_buf[3] = 0.0;
}

inline void Lamp::setColor(Color const& color)
{
	this->color = color;
	color_buf[0] = color.getRed() * energy;
	color_buf[1] = color.getGreen() * energy;
	color_buf[2] = color.getBlue() * energy;
}

inline void Lamp::setEnergy(float energy)
{
	this->energy = energy;
	color_buf[0] = color.getRed() * energy;
	color_buf[1] = color.getGreen() * energy;
	color_buf[2] = color.getBlue() * energy;
}

inline Color Lamp::getColor(void) const
{
	return color;
}

inline void Lamp::setUpGlLight(GLenum gl_light) const
{
	// Get position
	Vector3 pos = getAbsoluteTransform().getPosition();
	float pos_buf[4];
	pos_buf[0] = pos.x;
	pos_buf[1] = pos.y;
	pos_buf[2] = pos.z;
	pos_buf[3] = 1.0;
	// Init gl light
	HppCheckGlErrors();
	glEnable(gl_light);
	glLightfv(gl_light, GL_DIFFUSE, color_buf);
	glLightfv(gl_light, GL_POSITION, pos_buf);
	glLightf(gl_light, GL_CONSTANT_ATTENUATION, 0);
	glLightf(gl_light, GL_LINEAR_ATTENUATION, 0);
	glLightf(gl_light, GL_QUADRATIC_ATTENUATION, 1);
	HppCheckGlErrors();
}

}

#endif
