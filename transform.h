#ifndef HPP_TRANSFORM_H
#define HPP_TRANSFORM_H

#include "3dconversions.h"
#include "quaternion.h"
#include "matrix4.h"
#include "3dutils.h"
#include "vector3.h"
#include "angle.h"
#include "axis.h"
#include "json.h"
#include "serializable.h"
#include "deserializable.h"

namespace Hpp
{

class Transform : public Serializable, public Deserializable
{

public:

	inline Transform(void);
	inline Transform(Vector3 const& pos);
	inline Transform(Axis axis, Angle const& angle);
	inline Transform(Matrix3 const& m);
	inline Transform(Matrix4 const& m);
	inline Transform(Quaternion const& q);
	inline Transform(Json const& json);

	inline void reset(void);

	// Setters
	inline void setPosition(Vector3 const& pos);
	inline void setRotationFromUpRight(Vector3 const& up, Vector3 const& right);

	// Getters
	inline Matrix4 getMatrix(void) const { return transf; }
	inline Matrix3 getMatrix3(void) const { return matrix4ToMatrix3(transf); }
	inline Quaternion getQuaternion(void) const { return matrix4ToQuaternion(transf); }
	inline Vector3 getPosition(void) const;
	inline Vector3 getXVector(void) const;
	inline Vector3 getYVector(void) const;
	inline Vector3 getZVector(void) const;
	inline Transform getRotScale(void) const;

	// Modifiers. Right hand rotations, where your thumb is rotation axis
	// and your fingers show the direction of turning when your fingers are
	// in "fist form".
	inline void translate(Vector3 const& v) { transf = Matrix4::translMatrix(v) * transf; }
	inline void rotateX(Angle const& angle) { transf = Matrix4::rotMatrixX(angle) * transf; }
	inline void rotateY(Angle const& angle) { transf = Matrix4::rotMatrixY(angle) * transf; }
	inline void rotateZ(Angle const& angle) { transf = Matrix4::rotMatrixZ(angle) * transf; }
	inline void rotate(Quaternion const& q) { transf = quaternionToMatrix4(q) * transf; }
	inline void rotate(Vector3 const& axis, Angle const& angle) { transf = Matrix4::rotMatrix(axis, angle) * transf; }
	inline void scale(Vector3 const& v) { transf = Matrix4::scaleMatrix(v) * transf; }

	// Operators for combining transforms. First this
	// transform is done and then another one.
	inline Transform operator+=(Transform const& transf2);
	inline Transform operator+(Transform const& transf2) const;

	// Operators to undo specific transform from this one.
	inline Transform operator-=(Transform const& transf2);
	inline Transform operator-(Transform const& transf2) const;

	// Combines this and another so that first this current transform is
	// done and then another one.
	inline Transform addAnotherTransform(Transform const& transf2) const;

	inline Vector3 applyToPosition(Vector3 const& pos) const;
	inline Real getMaximumScaling(void) const;

	// Get how Transform sees the given absolute position
	inline Vector3 getRelativePositionTo(Vector3 const& pos) const;

	inline Json toJson(void) const;

private:

	Matrix4 transf;

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual void doSerialize(ByteV& result) const;
	inline virtual void doDeserialize(std::istream& strm);

};

inline Transform::Transform(void) :
transf(Matrix4::IDENTITY)
{
}

inline Transform::Transform(Vector3 const& pos) :
transf(Matrix4::translMatrix(pos))
{
}

inline Transform::Transform(Axis axis, Angle const& angle) :
transf(Matrix4::rotMatrix(axis, angle))
{
}

inline Transform::Transform(Matrix3 const& m) :
transf(matrix3ToMatrix4(m))
{
}

inline Transform::Transform(Matrix4 const& m) :
transf(m)
{
}

inline Transform::Transform(Quaternion const& q) :
transf(Matrix4::rotMatrix(q))
{
}

inline Transform::Transform(Json const& json) :
transf(Matrix4::IDENTITY)
{
	size_t arg_id = 0;
	while (arg_id < json.getArraySize()) {
		std::string arg = json.getItem(arg_id ++).getString();
		if (arg == "matrix") {
			transf = Matrix4(json.getItem(arg_id ++));
		} else if (arg == "translate") {
			translate(Vector3(json.getItem(arg_id ++)));
		} else if (arg == "rotate") {
			Vector3 rot_axis = Vector3(json.getItem(arg_id ++));
			Angle rot_angle(json.getItem(arg_id ++).getNumber());
			rotate(rot_axis, rot_angle);
		} else if (arg == "scale") {
			scale(Vector3(json.getItem(arg_id ++)));
		} else {
			throw Exception("Invalid transform command in JSON: " + arg);
		}
	}
}

inline void Transform::reset(void)
{
	transf = Matrix4::IDENTITY;
}

inline void Transform::setPosition(Vector3 const& pos)
{
	transf.cell(3) = pos.x;
	transf.cell(7) = pos.y;
	transf.cell(11) = pos.z;
}

inline void Transform::setRotationFromUpRight(Vector3 const& up, Vector3 const& right)
{
	HppAssert(fabs(dotProduct(up, right)) < 0.00001, "Up and right vectors must be perpendicular to each others!");
	Vector3 front = crossProduct(up, right);
	transf.cell(0) = right.x;
	transf.cell(1) = right.y;
	transf.cell(2) = right.z;
	transf.cell(4) = -up.x;
	transf.cell(5) = -up.y;
	transf.cell(6) = -up.z;
	transf.cell(8) = front.x;
	transf.cell(9) = front.y;
	transf.cell(10) = front.z;
}

inline Vector3 Transform::getPosition(void) const
{
	return Vector3(transf.cell(3), transf.cell(7), transf.cell(11));
}

inline Vector3 Transform::getXVector(void) const
{
	return Vector3(transf.cell(0), transf.cell(4), transf.cell(8));
}

inline Vector3 Transform::getYVector(void) const
{
	return Vector3(transf.cell(1), transf.cell(5), transf.cell(9));
}

inline Vector3 Transform::getZVector(void) const
{
	return Vector3(transf.cell(2), transf.cell(6), transf.cell(10));
}

inline Transform Transform::getRotScale(void) const
{
	Real const* cells = transf.getCells();
	return Transform(Matrix4(cells[0], cells[1], cells[2], 0,
	                         cells[4], cells[5], cells[6], 0,
	                         cells[8], cells[9], cells[10], 0,
	                         cells[12], cells[13], cells[14], 1));
}

inline Transform Transform::operator+=(Transform const& transf2)
{
	transf = transf2.transf * transf;
	return *this;
}

inline Transform Transform::operator+(Transform const& transf2) const
{
	return Transform(transf2.transf * transf);
}

inline Transform Transform::operator-=(Transform const& transf2)
{
	transf = transf2.transf.inverse() * transf;
	return *this;
}

inline Transform Transform::operator-(Transform const& transf2) const
{
	return Transform(transf2.transf.inverse() * transf);
}

inline Transform Transform::addAnotherTransform(Transform const& transf2) const
{
	return Transform(transf2.transf * transf);
}

inline Vector3 Transform::applyToPosition(Vector3 const& pos) const
{
	return transf * pos;
}

inline Real Transform::getMaximumScaling(void) const
{
// TODO/3D: Ensure this goes right!
	Real len_x_to2 = transf.cell(0)*transf.cell(0) + transf.cell(1)*transf.cell(1) + transf.cell(2)*transf.cell(2);
	Real len_y_to2 = transf.cell(4)*transf.cell(4) + transf.cell(5)*transf.cell(5) + transf.cell(6)*transf.cell(6);
	Real len_z_to2 = transf.cell(8)*transf.cell(8) + transf.cell(9)*transf.cell(9) + transf.cell(10)*transf.cell(10);
	if (len_x_to2 > len_y_to2 && len_x_to2 > len_z_to2) {
		return sqrt(len_x_to2);
	}
	if (len_y_to2 > len_z_to2) {
		return sqrt(len_y_to2);
	}
	return sqrt(len_z_to2);
}

inline Vector3 Transform::getRelativePositionTo(Vector3 const& pos) const
{
	return transf.inverse() * pos;
}

inline Json Transform::toJson(void) const
{
	Json result = Json::newArray();
	result.addItem(Json::newString("matrix"));
	result.addItem(transf.toJson());
	return result;
}

inline void Transform::doSerialize(ByteV& result) const
{
	transf.serialize(result);
}

inline void Transform::doDeserialize(std::istream& strm)
{
	transf.deserialize(strm);
}

}

#endif
