#ifndef HPP_LIGHT
#define HPP_LIGHT

#include "vector3.h"
#include "color.h"
#include "camera.h"
#include "boundingsphere.h"
#include "orthographiccamera.h"
#include "perspectivecamera.h"

namespace Hpp
{

class Light
{

public:

// TODO: Support spotlight!
	enum Type { NOTHING, SUN, POINT };

	// Default light. This cannot be used!
	inline Light(void);
	inline ~Light(void);

	// Creator functions
	inline static Light sun(Vector3 const& dir, Color const& color = Color(1, 1, 1));
	inline static Light point(Vector3 const& pos, Color const& color = Color(1, 1, 1), Real attenuation_constant = 1, Real attenuation_linear = 0, Real attenuation_quadratic = 0);

	inline Light::Type getType(void) const { return type; }
	inline Color getColor(void) const { return color; }

	// For sun
// TODO: Code setters!
	inline Vector3 getDirection(void) const;

	// For point
	inline void setPosition(Hpp::Vector3 const& pos);
// TODO: Code setters!
	inline Vector3 getPosition(void) const;
	inline Real getConstantAttenuation(void) const;
	inline Real getLinearAttenuation(void) const;
	inline Real getQuadraticAttenuation(void) const;

	// Get Camera for shadow mapping. You may not destroy this
	// Camera! Also, in case of point light, only one of
	// cameras can be used at a time. NULL is returned if
	// Camera is not needed to show the requested area.
	inline Camera* getCameraFromLight(Boundingsphere const& area,
	                                  size_t texture_width,
	                                  size_t camera_id = 0) const;

private:

	// Constructor in private, to prevent creation by others.
	inline Light(Type type, Color const& color);

	Type type;
	Color color;

	// For sun: Normalized direction
	// For point: Position
	Vector3 v1;

	// For point: Attenuations
	Vector3 v2;

	// For shadow mapping
	mutable Camera* camera;
};

inline Light::Light(void) :
type(NOTHING),
camera(NULL)
{
}

inline Light::~Light(void)
{
	delete camera;
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

void Light::setPosition(Hpp::Vector3 const& pos)
{
	HppAssert(type == POINT, "Unable to set position, because light is not point!");
	v1 = pos;
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

inline Camera* Light::getCameraFromLight(Boundingsphere const& area, size_t texture_width, size_t camera_id) const
{
	if (area.isNothing()) {
		return NULL;
	}

	if (type == POINT) {
// TODO: Code this!
(void)camera_id;
HppAssert(false, "Not implemented yet!");
return NULL;
	} else if (type == SUN) {
		if (area.isInfinite()) {
			throw Exception("You have to give finite Boundingsphere in case of sun!");
		}
		// Construct or update camera
		Real camera_size = area.getRadius() * 2;
		if (!camera) {
			camera = new Orthographiccamera(camera_size, 0, camera_size,
			                                0, 0, texture_width, texture_width);
		} else {
			Orthographiccamera* ocamera = dynamic_cast< Orthographiccamera* >(camera);
			HppAssert(ocamera, "Invalid camera!");
			ocamera->setSize(camera_size);
			camera->setNearAndFarPlanes(0, camera_size);
			camera->setViewport(0, 0, texture_width, texture_width);
		}
		// Prepare for camera positioning and rotating
		Vector3 const& camera_dir = v1;
		Vector3 camera_pos = area.getPosition() - camera_dir * area.getRadius();
		Vector2 camera_dir_xy(camera_dir.x, camera_dir.y);
		Angle camera_yaw = angleOfVector(camera_dir_xy);
		Angle camera_pitch = angleOfVector(Vector2(-camera_dir.z, camera_dir_xy.length()));
		// Position and rotate camera
		Transform cam_transf;
		cam_transf.setRotationFromUpRight(Vector3::Z, Vector3::X);
		cam_transf.rotateX(camera_pitch);
		cam_transf.rotateZ(camera_yaw);
		cam_transf.translate(camera_pos);
		camera->setTransform(cam_transf);
		// Finalize
		camera->update();
	} else {
		throw Exception("Unable to get shadow mapping Camera from default light!");
	}
	return camera;
}

inline Light::Light(Type type, Color const& color) :
type(type),
color(color),
camera(NULL)
{
}

}

#endif
