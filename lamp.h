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

	inline void setConstantAttenuation(float attenu_c) { this->attenu_c = attenu_c; }
	inline void setLinearAttenuation(float attenu_l) { this->attenu_l = attenu_l; }
	inline void setQuadraticAttenuation(float attenu_q) { this->attenu_q = attenu_q; }

	inline float getConstantAttenuation(void) const { return attenu_c; }
	inline float getLinearAttenuation(void) const { return attenu_l; }
	inline float getQuadraticAttenuation(void) const { return attenu_q; }

	inline virtual Vector3 getPosition(void) const { return getAbsoluteTransform().getPosition(); }
	inline virtual Color getColor(void) const { return color * energy; }

private:

	Color color;
	float energy;

	float color_buf[4];

	float attenu_c;
	float attenu_l;
	float attenu_q;

	inline virtual void setUpGlLight(GLenum gl_light) const;//DEPRECATED

};

inline Lamp::Lamp(void) :
attenu_c(1),
attenu_l(0),
attenu_q(0)
{
	color_buf[3] = 0.0;
}

inline Lamp::Lamp(Color const& color, float energy) :
color(color),
energy(energy),
attenu_c(1),
attenu_l(0),
attenu_q(0)
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
	glLightfv(gl_light, GL_DIFFUSE, color_buf);//DEPRECATED
	glLightfv(gl_light, GL_POSITION, pos_buf);//DEPRECATED
	glLightf(gl_light, GL_CONSTANT_ATTENUATION, 0);//DEPRECATED
	glLightf(gl_light, GL_LINEAR_ATTENUATION, 0);//DEPRECATED
	glLightf(gl_light, GL_QUADRATIC_ATTENUATION, 1);//DEPRECATED
	HppCheckGlErrors();
}

}

#endif
