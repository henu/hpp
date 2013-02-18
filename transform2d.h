#ifndef HPP_TRANSFORM2D_H
#define HPP_TRANSFORM2D_H

#include "matrix3.h"
#include "vector2.h"
#include "angle.h"

namespace Hpp
{

class Transform2D
{

public:

	inline Transform2D(void);
	inline Transform2D(Vector2 const& pos);
	inline Transform2D(Angle const& angle);
	inline Transform2D(Matrix3 const& m);

	inline void reset(void);

	// Setters
	inline void setPosition(Vector2 const& pos);

	// Getters
	inline Matrix3 getMatrix(void) const { return transf; }
	inline Vector2 getPosition(void) const;
	inline Vector2 getXVector(void) const;
	inline Vector2 getYVector(void) const;
	inline Transform2D getRotScale(void) const;

	// Modifiers. Rotation is counterclockwise when X is right and Y up.
	inline void translate(Vector2 const& v) { transf = Matrix3::translMatrix(v) * transf; }
	inline void rotate(Angle const& angle) { transf = Matrix3::rotMatrix2d(angle) * transf; }
	inline void scale(Vector2 const& v) { transf = Matrix3::scaleMatrix(v) * transf; }

	// Operators for combining transforms. First this
	// transform is done and then another one.
	inline Transform2D operator+=(Transform2D const& transf2);
	inline Transform2D operator+(Transform2D const& transf2) const;

	// Operators to undo specific transform from this one.
	inline Transform2D operator-=(Transform2D const& transf2);
	inline Transform2D operator-(Transform2D const& transf2) const;

	// Combines this and another so that first this current transform is
	// done and then another one.
	inline Transform2D addAnotherTransform(Transform2D const& transf2) const;

	inline Vector2 applyToPosition(Vector2 const& pos) const;
	inline Real getMaximumScaling(void) const;

	// Get how Transform sees the given absolute position
	inline Vector2 getRelativePositionTo(Vector2 const& pos) const;

private:

	Matrix3 transf;

};

inline Transform2D::Transform2D(void) :
transf(Matrix3::IDENTITY)
{
}

inline Transform2D::Transform2D(Vector2 const& pos) :
transf(Matrix3::translMatrix(pos))
{
}

inline Transform2D::Transform2D(Angle const& angle) :
transf(Matrix3::rotMatrix2d(angle))
{
}

inline Transform2D::Transform2D(Matrix3 const& m) :
transf(m)
{
}

inline void Transform2D::reset(void)
{
	transf = Matrix3::IDENTITY;
}

inline void Transform2D::setPosition(Vector2 const& pos)
{
	transf.cell(2) = pos.x;
	transf.cell(5) = pos.y;
}

inline Vector2 Transform2D::getPosition(void) const
{
	return Vector2(transf.cell(2), transf.cell(5));
}

inline Vector2 Transform2D::getXVector(void) const
{
	return Vector2(transf.cell(0), transf.cell(1));
}

inline Vector2 Transform2D::getYVector(void) const
{
	return Vector2(transf.cell(3), transf.cell(4));
}

inline Transform2D Transform2D::getRotScale(void) const
{
	Real const* cells = transf.getCells();
	return Transform2D(Matrix3(cells[0], cells[1], 0,
	                           cells[3], cells[4], 0,
	                           cells[6], cells[7], 1));
}

inline Transform2D Transform2D::operator+=(Transform2D const& transf2)
{
	transf = transf2.transf * transf;
	return *this;
}

inline Transform2D Transform2D::operator+(Transform2D const& transf2) const
{
	return Transform2D(transf2.transf * transf);
}

inline Transform2D Transform2D::operator-=(Transform2D const& transf2)
{
	transf = transf2.transf.inverse() * transf;
	return *this;
}

inline Transform2D Transform2D::operator-(Transform2D const& transf2) const
{
	return Transform2D(transf2.transf.inverse() * transf);
}

inline Transform2D Transform2D::addAnotherTransform(Transform2D const& transf2) const
{
	return Transform2D(transf2.transf * transf);
}

inline Vector2 Transform2D::applyToPosition(Vector2 const& pos) const
{
	return transf * pos;
}

inline Real Transform2D::getMaximumScaling(void) const
{
// TODO/3D: Ensure this goes right!
	Real len_x_to2 = transf.cell(0)*transf.cell(0) + transf.cell(1)*transf.cell(1);
	Real len_y_to2 = transf.cell(3)*transf.cell(3) + transf.cell(5)*transf.cell(5);
	if (len_x_to2 > len_y_to2) {
		return sqrt(len_x_to2);
	}
	return sqrt(len_y_to2);
}

inline Vector2 Transform2D::getRelativePositionTo(Vector2 const& pos) const
{
	return transf.inverse() * pos;
}

}

#endif
