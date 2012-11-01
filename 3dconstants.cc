#include "vector2.h"
#include "vector3.h"
#include "matrix3.h"
#include "matrix4.h"
#include "quaternion.h"
#include "angle.h"

namespace Hpp

{

Vector2 const Vector2::ONE(1, 1);
Vector2 const Vector2::ZERO(0, 0);
Vector2 const Vector2::X(1, 0);
Vector2 const Vector2::Y(0, 1);

Vector3 const Vector3::ONE(1, 1, 1);
Vector3 const Vector3::ZERO(0, 0, 0);
Vector3 const Vector3::X(1, 0, 0);
Vector3 const Vector3::Y(0, 1, 0);
Vector3 const Vector3::Z(0, 0, 1);

Matrix3 const Matrix3::IDENTITY(1, 0, 0,
                                0, 1, 0,
                                0, 0, 1);
Matrix3 const Matrix3::ZERO(0, 0, 0,
                            0, 0, 0,
                            0, 0, 0);

Matrix4 const Matrix4::IDENTITY(1, 0, 0, 0,
                                0, 1, 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1);
Matrix4 const Matrix4::ZERO(0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0);

Quaternion const Quaternion::IDENTITY(0.0, 0.0, 0.0, 1.0);
Quaternion const Quaternion::ZERO(0.0, 0.0, 0.0, 0.0);

Angle const Angle::ZERO(Hpp::Angle::fromDegrees(0));

}
