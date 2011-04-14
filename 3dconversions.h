#ifndef HPP_3DCONVERSIONS_H
#define HPP_3DCONVERSIONS_H

#include "matrix3.h"
#include "matrix4.h"
#include "quaternion.h"

namespace Hpp
{

inline Quaternion matrix3ToQuaternion(Matrix3 const& m);
inline Quaternion matrix4ToQuaternion(Matrix4 const& m);
inline Matrix4 matrix3ToMatrix4(Matrix3 const& m);
inline Matrix3 matrix4ToMatrix3(Matrix4 const& m);
inline Matrix3 quaternionToMatrix3(Quaternion const& q);
inline Matrix4 quaternionToMatrix4(Quaternion const& q);
inline Vector3 quaternionToVector(Quaternion const& q);

inline Quaternion matrix3ToQuaternion(Matrix3 const& m)
{
	Quaternion result;
	Real m_trace = m.cell(0) + m.cell(4) + m.cell(8) + 1.0;

	if (m_trace > 0.0) {
		Real s = 0.5 / sqrt(m_trace);
		HppAssert(s != 0.0, "Division by zero!");
		result.w = 0.25 / s;
		result.x = (m.cell(5) - m.cell(7)) * s;
		result.y = (m.cell(6) - m.cell(2)) * s;
		result.z = (m.cell(1) - m.cell(3)) * s;
	} else {
		uint8_t greatest_diag = 0;
		Real greatest_diag_value = m.cell(0);
		if (m.cell(4) > greatest_diag_value) {
			greatest_diag_value = m.cell(4);
			greatest_diag = 1;
		}
		if (m.cell(8) > greatest_diag_value) {
			greatest_diag_value = m.cell(8);
			greatest_diag = 2;
		}
		Real s;
		switch (greatest_diag) {
		case 0:
			s = sqrt(1.0 + m.cell(0) - m.cell(4) - m.cell(8)) * 2;
			HppAssert(s != 0.0, "Division by zero!");
			result.w = (m.cell(7) + m.cell(5)) / s;
			result.x = 0.5 / s;
			result.y = (m.cell(3) + m.cell(1)) / s;
			result.z = (m.cell(6) + m.cell(2)) / s;
			break;
		case 1:
			s = sqrt(1.0 + m.cell(4) - m.cell(0) - m.cell(8)) * 2;
			HppAssert(s != 0.0, "Division by zero!");
			result.w = (m.cell(6) + m.cell(2)) / s;
			result.x = (m.cell(3) + m.cell(1)) / s;
			result.y = 0.5 / s;
			result.z = (m.cell(7) + m.cell(5)) / s;
			break;
		case 2:
			s = sqrt(1.0 + m.cell(8) - m.cell(0) - m.cell(4)) * 2;
			HppAssert(s != 0.0, "Division by zero!");
			result.w = (m.cell(3) + m.cell(1)) / s;
			result.x = (m.cell(6) + m.cell(2)) / s;
			result.y = (m.cell(7) + m.cell(5)) / s;
			result.z = 0.5 / s;
			break;
		}
	}
	#ifndef NDEBUG
	Matrix3 check_m = quaternionToMatrix3(result);
	HppAssert(fabs(m.cell(0) - check_m.cell(0)) < 0.001, "");
	HppAssert(fabs(m.cell(1) - check_m.cell(1)) < 0.001, "");
	HppAssert(fabs(m.cell(2) - check_m.cell(2)) < 0.001, "");
	HppAssert(fabs(m.cell(3) - check_m.cell(3)) < 0.001, "");
	HppAssert(fabs(m.cell(4) - check_m.cell(4)) < 0.001, "");
	HppAssert(fabs(m.cell(5) - check_m.cell(5)) < 0.001, "");
	HppAssert(fabs(m.cell(6) - check_m.cell(6)) < 0.001, "");
	HppAssert(fabs(m.cell(7) - check_m.cell(7)) < 0.001, "");
	HppAssert(fabs(m.cell(8) - check_m.cell(8)) < 0.001, "");
	#endif
	return result;
}

inline Quaternion matrix4ToQuaternion(Matrix4 const& m)
{
	return matrix3ToQuaternion(matrix4ToMatrix3(m));
}

inline Matrix4 matrix3ToMatrix4(Matrix3 const& m)
{
	return Matrix4(m.cell(0), m.cell(1), m.cell(2), 0,
	               m.cell(3), m.cell(4), m.cell(5), 0,
	               m.cell(6), m.cell(7), m.cell(8), 0,
	               0,         0,         0,         1);
}

inline Matrix3 matrix4ToMatrix3(Matrix4 const& m)
{
	return Matrix3(m.cell(0), m.cell(1), m.cell(2),
	               m.cell(4), m.cell(5), m.cell(6),
	               m.cell(8), m.cell(9), m.cell(10));
}

inline Matrix3 quaternionToMatrix3(Quaternion const& q)
{
	Quaternion q_unit = q.normalized();
	return Matrix3(1 - 2*q_unit.y*q_unit.y - 2*q_unit.z*q_unit.z, 2*q_unit.x*q_unit.y + 2*q_unit.w*q_unit.z,     2*q_unit.x*q_unit.z - 2*q_unit.w*q_unit.y,
	               2*q_unit.x*q_unit.y - 2*q_unit.w*q_unit.z,     1 - 2*q_unit.x*q_unit.x - 2*q_unit.z*q_unit.z, 2*q_unit.y*q_unit.z + 2*q_unit.w*q_unit.x,
	               2*q_unit.x*q_unit.z + 2*q_unit.w*q_unit.y,     2*q_unit.y*q_unit.z - 2*q_unit.w*q_unit.x,     1 - 2*q_unit.x*q_unit.x - 2*q_unit.y*q_unit.y);
}

inline Matrix4 quaternionToMatrix4(Quaternion const& q)
{
	Quaternion q_unit = q.normalized();
	return Matrix4(1 - 2*q_unit.y*q_unit.y - 2*q_unit.z*q_unit.z, 2*q_unit.x*q_unit.y - 2*q_unit.w*q_unit.z,     2*q_unit.x*q_unit.z + 2*q_unit.w*q_unit.y,     0,
	               2*q_unit.x*q_unit.y + 2*q_unit.w*q_unit.z,     1 - 2*q_unit.x*q_unit.x - 2*q_unit.z*q_unit.z, 2*q_unit.y*q_unit.z - 2*q_unit.w*q_unit.x,     0,
	               2*q_unit.x*q_unit.z - 2*q_unit.w*q_unit.y,     2*q_unit.y*q_unit.z + 2*q_unit.w*q_unit.x,     1 - 2*q_unit.x*q_unit.x - 2*q_unit.y*q_unit.y, 0,
	               0,                                             0,                                             0,                                             1);
}

inline Vector3 quaternionToVector(Quaternion const& q)
{
	return Vector3(q.x, q.y, q.z);
}

}

#endif
