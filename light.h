#ifndef HPP_LIGHT
#define HPP_LIGHT

#include "vector3.h"
#include "color.h"

namespace Hpp
{

class Light
{

public:

// TODO: Support spotlight!
	enum Type { NOTHING, SUN, POINT };

	// Default light. This cannot be used!
	inline Light(void);

	// Creator functions
	inline static Light sun(Vector3 const& dir, Color const& color = Color(1, 1, 1));
	inline static Light point(Vector3 const& pos, Color const& color = Color(1, 1, 1), Real attenuation_constant = 1, Real attenuation_linear = 0, Real attenuation_quadratic = 0);

	inline Light::Type getType(void) const { return type; }
	inline Color getColor(void) const { return color; }

	// For sun
	inline Vector3 getDirection(void) const;

	// For point
	inline Vector3 getPosition(void) const;
	inline Real getConstantAttenuation(void) const;
	inline Real getLinearAttenuation(void) const;
	inline Real getQuadraticAttenuation(void) const;

private:

	// Constructor in private, to prevent creation by others.
	inline Light(Type type, Color const& color);

	Type type;
	Color color;

	// For sun: Direction
	// For point: Position
	Vector3 v1;

	// For point: Attenuations
	Vector3 v2;
};

inline Light::Light(void) :
type(NOTHING)
{
}

inline Light Light::sun(Vector3 const& dir, Color const& color)
{
	Light new_sun(SUN, color);
	new_sun.v1 = dir.normalized();
	return new_sun;
}

inline Light Light::point(Vector3 const& pos, Color const& color, Real attenuation_constant, Real attenuation_linear, Real attenuation_quadratic)
{
	Light new_point(POINT, color);
	new_point.v1 = pos;
	new_point.v2.x = attenuation_constant;
	new_point.v2.y = attenuation_linear;
	new_point.v2.z = attenuation_quadratic;
	return new_point;
}

inline Vector3 Light::getDirection(void) const
{
	HppAssert(type == SUN, "Unable to get direction, because light is not sun!");
	return v1;
}

inline Vector3 Light::getPosition(void) const
{
	HppAssert(type == POINT, "Unable to get position, because light is not point!");
	return v1;
}

inline Real Light::getConstantAttenuation(void) const
{
	HppAssert(type == POINT, "Unable to get attenuation, because light is not point!");
	return v2.x;
}

inline Real Light::getLinearAttenuation(void) const
{
	HppAssert(type == POINT, "Unable to get attenuation, because light is not point!");
	return v2.y;
}

inline Real Light::getQuadraticAttenuation(void) const
{
	HppAssert(type == POINT, "Unable to get attenuation, because light is not point!");
	return v2.z;
}

inline Light::Light(Type type, Color const& color) :
type(type),
color(color)
{
}

}

#endif
