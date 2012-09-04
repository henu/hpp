#ifndef HPP_MATRIX4_H
#define HPP_MATRIX4_H

#include "json.h"
#include "angle.h"
#include "vector3.h"
#include "quaternion.h"
#include "real.h"
#include "axis.h"

#include <stdint.h>

namespace Hpp
{

class Matrix4
{

public:

	static Matrix4 const IDENTITY;
	static Matrix4 const ZERO;

	// Constructor
	inline Matrix4(void);

	// Copy constructor and assignment operator
	inline Matrix4(Matrix4 const& m);
	inline Matrix4& operator=(Matrix4 const& m);

	// Miscellaneous constructors
	inline Matrix4(Real a, Real b, Real c, Real d,
	               Real e, Real f, Real g, Real h,
	               Real i, Real j, Real k, Real l,
	               Real m, Real n, Real o, Real p);
	inline Matrix4(Json const& json);

	// Conversion functions
	inline Json toJson(void) const;

	// Comparison operators
	inline bool operator==(Matrix4 const& m) const;
	inline bool operator!=(Matrix4 const& m) const;

	// Miscellaneous operators
	inline Matrix4 operator*(Matrix4 const& m) const;
	inline Matrix4 operator+(Matrix4 const& m) const;
	inline Matrix4 operator*(Real r) const;
	inline Matrix4 operator/(Real r) const;
	inline Matrix4 const& operator*=(Matrix4 const& m);
	inline Matrix4 const& operator+=(Matrix4 const& m);

	// Vector operators
	inline Vector3 operator*(Vector3 const& v) const;

	// Function to get determinant of this Matrix
	inline Real determinant(void) const;

	// Function to get inverse of this Matrix
	Matrix4 inverse(void) const;

	// Function to get cells array
	inline Real const* getCells(void) const;

	// Functions to use cells in matrix
	inline Real& cell(uint8_t offset);
	inline Real& cell(uint8_t row, uint8_t offset);
	inline Real const& cell(uint8_t offset) const;
	inline Real const& cell(uint8_t row, uint8_t offset) const;

	// Static functions for generating rotation matrices
	static inline Matrix4 rotMatrix(Axis axis, Angle const& angle);
	static inline Matrix4 rotMatrix(Quaternion const& q);
	static inline Matrix4 rotMatrix(Vector3 const& axis, Angle const& angle);
	static inline Matrix4 rotMatrixX(Angle const& angle);
	static inline Matrix4 rotMatrixY(Angle const& angle);
	static inline Matrix4 rotMatrixZ(Angle const& angle);
	#ifdef HPP_Y_UP_Z_DOWN
	static inline Matrix4 rollMatrix(Angle const& angle) { return rotMatrixZ(angle); }
	static inline Matrix4 pitchMatrix(Angle const& angle) { return rotMatrixX(angle); }
	static inline Matrix4 yawMatrix(Angle const& angle) { return rotMatrixY(angle); }
	#else
	static inline Matrix4 rollMatrix(Angle const& angle) { return rotMatrixY(angle); }
	static inline Matrix4 pitchMatrix(Angle const& angle) { return rotMatrixX(angle); }
	static inline Matrix4 yawMatrix(Angle const& angle) { return rotMatrixZ(angle); }
	#endif

	// Static function for generating various special matrices.
	// In projectionMatrix FOV means angle between opposite
	// viewfrustum planes.
	static inline Matrix4 translMatrix(Vector3 const& v);
	static inline Matrix4 scaleMatrix(Vector3 const& v);
	static inline Matrix4 projectionMatrix(Hpp::Angle const& fov_y,
	                                       Real aspectratio,
	                                       Real near, Real far);

private:

	// Cells of Matrix
	Real cells[16];

	// Function to get subdeterminant. The parameter is the offset of the
	// cell that is NOT in area of subdeterminant
	inline Real subdeterminant(uint8_t cell_id) const;

	// Function to get complement of spcific cell in Matrix
	inline Real complement(uint8_t cell_id) const;

};

inline Matrix4 transpose(Matrix4 const& m);

inline std::ostream& operator<<(std::ostream& strm, Matrix4 const& m);



// ----------------------------------------
// ----------------------------------------
//
// Implementation of inline functions of Matrix4
//
// ----------------------------------------
// ----------------------------------------


inline Matrix4::Matrix4(void)
{
}

inline Matrix4::Matrix4(Matrix4 const& m)
{
	cells[0] = m.cells[0];
	cells[1] = m.cells[1];
	cells[2] = m.cells[2];
	cells[3] = m.cells[3];
	cells[4] = m.cells[4];
	cells[5] = m.cells[5];
	cells[6] = m.cells[6];
	cells[7] = m.cells[7];
	cells[8] = m.cells[8];
	cells[9] = m.cells[9];
	cells[10] = m.cells[10];
	cells[11] = m.cells[11];
	cells[12] = m.cells[12];
	cells[13] = m.cells[13];
	cells[14] = m.cells[14];
	cells[15] = m.cells[15];
}

inline Matrix4& Matrix4::operator=(Matrix4 const& m)
{
	cells[0] = m.cells[0];
	cells[1] = m.cells[1];
	cells[2] = m.cells[2];
	cells[3] = m.cells[3];
	cells[4] = m.cells[4];
	cells[5] = m.cells[5];
	cells[6] = m.cells[6];
	cells[7] = m.cells[7];
	cells[8] = m.cells[8];
	cells[9] = m.cells[9];
	cells[10] = m.cells[10];
	cells[11] = m.cells[11];
	cells[12] = m.cells[12];
	cells[13] = m.cells[13];
	cells[14] = m.cells[14];
	cells[15] = m.cells[15];
	return *this;
}

inline Matrix4::Matrix4(Real a, Real b, Real c, Real d,
                        Real e, Real f, Real g, Real h,
                        Real i, Real j, Real k, Real l,
                        Real m, Real n, Real o, Real p)
{
	cells[0] = a;
	cells[1] = b;
	cells[2] = c;
	cells[3] = d;
	cells[4] = e;
	cells[5] = f;
	cells[6] = g;
	cells[7] = h;
	cells[8] = i;
	cells[9] = j;
	cells[10] = k;
	cells[11] = l;
	cells[12] = m;
	cells[13] = n;
	cells[14] = o;
	cells[15] = p;
}

inline Matrix4::Matrix4(Json const& json)
{
	// Check JSON validity
	if (json.getType() != Json::ARRAY || json.getArraySize() != 16) throw Exception("JSON for Matrix4 must be array that has 16 numbers!");
	for (uint8_t cell_id = 0; cell_id < 16; ++ cell_id) {
		Json const& cell_json = json.getItem(cell_id);
		if (cell_json.getType() != Json::NUMBER) throw Exception("Found non-number cell from Matrix4 JSON!");
		cells[cell_id] = cell_json.getNumber();
	}
}

inline Json Matrix4::toJson(void) const
{
	Json result = Json::newArray();
	for (uint8_t cell_id = 0; cell_id < 16; ++ cell_id) {
		result.addItem(Json::newNumber(cells[cell_id]));
	}
	return result;
}

inline bool Matrix4::operator==(Matrix4 const& m) const
{
	for (size_t cells_id = 0; cells_id < 16; cells_id ++) {
		if (cells[cells_id] != m.cells[cells_id]) {
			return false;
		}
	}
	return true;
}

inline bool Matrix4::operator!=(Matrix4 const& m) const
{
	return !(*this == m);
}

inline Matrix4 Matrix4::operator*(Matrix4 const& m) const
{
	Matrix4 out;
	for (uint8_t row = 0; row < 4; ++ row) {
		for (uint8_t col = 0; col < 4; ++ col) {
			uint8_t ofs = row*4 + col;
			out.cells[ofs] = 0;
			for (uint8_t mult = 0; mult < 4; ++ mult) {
				out.cells[ofs] += cells[row*4 + mult] * m.cells[mult*4 + col];
			}
		}
	}
	return out;
}

inline Matrix4 Matrix4::operator/(Real r) const
{
	HppAssert(r != 0, "Division by zero!");
	Matrix4 out;
	out.cells[0] = cells[0] / r;
	out.cells[1] = cells[1] / r;
	out.cells[2] = cells[2] / r;
	out.cells[3] = cells[3] / r;
	out.cells[4] = cells[4] / r;
	out.cells[5] = cells[5] / r;
	out.cells[6] = cells[6] / r;
	out.cells[7] = cells[7] / r;
	out.cells[8] = cells[8] / r;
	out.cells[9] = cells[9] / r;
	out.cells[10] = cells[10] / r;
	out.cells[11] = cells[11] / r;
	out.cells[12] = cells[12] / r;
	out.cells[13] = cells[13] / r;
	out.cells[14] = cells[14] / r;
	out.cells[15] = cells[15] / r;
	return out;
}

inline Matrix4 const& Matrix4::operator*=(Matrix4 const& m)
{
	Real cells_old[16];
	for (uint8_t cells_id = 0;
	     cells_id < 16;
	     cells_id ++) {
		cells_old[cells_id] = cells[cells_id];
	}
	for (uint8_t row = 0; row < 4; ++ row) {
		for (uint8_t col = 0; col < 4; ++ col) {
			uint8_t ofs = row*4 + col;
			cells[ofs] = 0;
			for (uint8_t mult = 0; mult < 4; ++ mult) {
				cells[ofs] += cells_old[row*4 + mult] * m.cells[mult*4 + col];
			}
		}
	}
	return *this;
}

inline Real Matrix4::determinant(void) const
{
	return cells[0] *
	       (cells[5]*(cells[10]*cells[15] - cells[11]*cells[14]) -
	        cells[9]*(cells[6]*cells[15] - cells[7]*cells[14]) +
	        cells[13]*(cells[6]*cells[11] - cells[7]*cells[10]))
	       - cells[4] *
	       (cells[1]*(cells[10]*cells[15] - cells[11]*cells[14]) -
	        cells[9]*(cells[2]*cells[15] - cells[3]*cells[14]) +
	        cells[13]*(cells[2]*cells[11] - cells[3]*cells[10]))
	       + cells[8] *
	       (cells[1]*(cells[6]*cells[15] - cells[7]*cells[14]) -
	        cells[5]*(cells[2]*cells[15] - cells[3]*cells[14]) +
	        cells[13]*(cells[2]*cells[7] - cells[3]*cells[6]))
	       - cells[12] *
	       (cells[1]*(cells[6]*cells[11] - cells[7]*cells[10]) -
	        cells[5]*(cells[2]*cells[11] - cells[3]*cells[10]) +
	        cells[9]*(cells[2]*cells[7] - cells[3]*cells[6]));
}

inline Matrix4 Matrix4::inverse(void) const
{

	// First get determinant and ensure it's not zero
	Real det = determinant();
	HppAssert(det != 0, "Cannot make inverse matrix because determinant is zero!");

	// Form adjugate matrix
	Matrix4 adj = Matrix4(+complement(0), -complement(4), +complement(8), -complement(12),
	                      -complement(1), +complement(5), -complement(9), +complement(13),
	                      +complement(2), -complement(6), +complement(10), -complement(14),
	                      -complement(3), +complement(7), -complement(11), +complement(15));

	// Ensure inverse matrix is valid
	#ifndef NDEBUG
// TODO: Check also cond of matrix!
	Matrix4 check = *this * (adj / det);
	HppAssert(check.cells[0] > 0.9 && check.cells[0] < 1.1 &&
	          check.cells[1] > -0.1 && check.cells[1] < 0.1 &&
	          check.cells[2] > -0.1 && check.cells[2] < 0.1 &&
	          check.cells[3] > -0.1 && check.cells[3] < 0.1 &&
	          check.cells[4] > -0.1 && check.cells[4] < 0.1 &&
	          check.cells[5] > 0.9 && check.cells[5] < 1.1 &&
	          check.cells[6] > -0.1 && check.cells[6] < 0.1 &&
	          check.cells[7] > -0.1 && check.cells[7] < 0.1 &&
	          check.cells[8] > -0.1 && check.cells[8] < 0.1 &&
	          check.cells[9] > -0.1 && check.cells[9] < 0.1 &&
	          check.cells[10] > 0.9 && check.cells[10] < 1.1 &&
	          check.cells[11] > -0.1 && check.cells[11] < 0.1 &&
	          check.cells[12] > -0.1 && check.cells[12] < 0.1 &&
	          check.cells[13] > -0.1 && check.cells[13] < 0.1 &&
	          check.cells[14] > -0.1 && check.cells[14] < 0.1 &&
	          check.cells[15] > 0.9 && check.cells[15] < 1.1, "Matrix inversion has failed!");
	#endif
	return adj / det;

}

inline Real const* Matrix4::getCells(void) const
{
	return cells;
}

inline Real Matrix4::subdeterminant(uint8_t cell_id) const
{
	switch (cell_id) {
	case 0:
		return cells[5] * (cells[10]*cells[15] - cells[11]*cells[14])
		       - cells[9] * (cells[6]*cells[15] - cells[7]*cells[14])
		       + cells[13] * (cells[6]*cells[11] - cells[7]*cells[10]);
	case 1:
		return cells[4] * (cells[10]*cells[15] - cells[11]*cells[14])
		       - cells[8] * (cells[6]*cells[15] - cells[7]*cells[14])
		       + cells[12] * (cells[6]*cells[11] - cells[7]*cells[10]);
	case 2:
		return cells[4] * (cells[9]*cells[15] - cells[11]*cells[13])
		       - cells[8] * (cells[5]*cells[15] - cells[7]*cells[13])
		       + cells[12] * (cells[5]*cells[11] - cells[7]*cells[9]);
	case 3:
		return cells[4] * (cells[9]*cells[14] - cells[10]*cells[13])
		       - cells[8] * (cells[5]*cells[14] - cells[6]*cells[13])
		       + cells[12] * (cells[5]*cells[10] - cells[6]*cells[9]);
	case 4:
		return cells[1] * (cells[10]*cells[15] - cells[11]*cells[14])
		       - cells[9] * (cells[2]*cells[15] - cells[3]*cells[14])
		       + cells[13] * (cells[2]*cells[11] - cells[3]*cells[10]);
	case 5:
		return cells[0] * (cells[10]*cells[15] - cells[11]*cells[14])
		       - cells[8] * (cells[2]*cells[15] - cells[3]*cells[14])
		       + cells[12] * (cells[2]*cells[11] - cells[3]*cells[10]);
	case 6:
		return cells[0] * (cells[9]*cells[15] - cells[11]*cells[13])
		       - cells[8] * (cells[1]*cells[15] - cells[3]*cells[13])
		       + cells[12] * (cells[1]*cells[11] - cells[3]*cells[9]);
	case 7:
		return cells[0] * (cells[9]*cells[14] - cells[10]*cells[13])
		       - cells[8] * (cells[1]*cells[14] - cells[2]*cells[13])
		       + cells[12] * (cells[1]*cells[10] - cells[2]*cells[9]);
	case 8:
		return cells[1] * (cells[6]*cells[15] - cells[7]*cells[14])
		       - cells[5] * (cells[2]*cells[15] - cells[3]*cells[14])
		       + cells[13] * (cells[2]*cells[7] - cells[3]*cells[6]);
	case 9:
		return cells[0] * (cells[6]*cells[15] - cells[7]*cells[14])
		       - cells[4] * (cells[2]*cells[15] - cells[3]*cells[14])
		       + cells[12] * (cells[2]*cells[7] - cells[3]*cells[6]);
	case 10:
		return cells[0] * (cells[5]*cells[15] - cells[7]*cells[13])
		       - cells[4] * (cells[1]*cells[15] - cells[3]*cells[13])
		       + cells[12] * (cells[1]*cells[7] - cells[3]*cells[5]);
	case 11:
		return cells[0] * (cells[5]*cells[14] - cells[6]*cells[13])
		       - cells[4] * (cells[1]*cells[14] - cells[2]*cells[13])
		       + cells[12] * (cells[1]*cells[6] - cells[2]*cells[5]);
	case 12:
		return cells[1] * (cells[6]*cells[11] - cells[7]*cells[10])
		       - cells[5] * (cells[2]*cells[11] - cells[3]*cells[10])
		       + cells[9] * (cells[2]*cells[7] - cells[3]*cells[6]);
	case 13:
		return cells[0] * (cells[6]*cells[11] - cells[7]*cells[10])
		       - cells[4] * (cells[2]*cells[11] - cells[3]*cells[10])
		       + cells[8] * (cells[2]*cells[7] - cells[3]*cells[6]);
	case 14:
		return cells[0] * (cells[5]*cells[11] - cells[7]*cells[9])
		       - cells[4] * (cells[1]*cells[11] - cells[3]*cells[9])
		       + cells[8] * (cells[1]*cells[7] - cells[3]*cells[5]);
	case 15:
		return cells[0] * (cells[5]*cells[10] - cells[6]*cells[9])
		       - cells[4] * (cells[1]*cells[10] - cells[2]*cells[9])
		       + cells[8] * (cells[1]*cells[6] - cells[2]*cells[5]);
	}
	HppAssert(false, "Cell ID overflow!");
	return 0;
}

inline Matrix4 Matrix4::operator+(Matrix4 const& m) const
{
	return Matrix4(cells[0] + m.cells[0],
	               cells[1] + m.cells[1],
	               cells[2] + m.cells[2],
	               cells[3] + m.cells[3],
	               cells[4] + m.cells[4],
	               cells[5] + m.cells[5],
	               cells[6] + m.cells[6],
	               cells[7] + m.cells[7],
	               cells[8] + m.cells[8],
	               cells[9] + m.cells[9],
	               cells[10] + m.cells[10],
	               cells[11] + m.cells[11],
	               cells[12] + m.cells[12],
	               cells[13] + m.cells[13],
	               cells[14] + m.cells[14],
	               cells[15] + m.cells[15]);
}

inline Matrix4 Matrix4::operator*(Real r) const
{
	return Matrix4(cells[0] * r,
	               cells[1] * r,
	               cells[2] * r,
	               cells[3] * r,
	               cells[4] * r,
	               cells[5] * r,
	               cells[6] * r,
	               cells[7] * r,
	               cells[8] * r,
	               cells[9] * r,
	               cells[10] * r,
	               cells[11] * r,
	               cells[12] * r,
	               cells[13] * r,
	               cells[14] * r,
	               cells[15] * r);
}

inline Matrix4 const& Matrix4::operator+=(Matrix4 const& m)
{
	cells[0] += m.cells[0];
	cells[1] += m.cells[1];
	cells[2] += m.cells[2];
	cells[3] += m.cells[3];
	cells[4] += m.cells[4];
	cells[5] += m.cells[5];
	cells[6] += m.cells[6];
	cells[7] += m.cells[7];
	cells[8] += m.cells[8];
	cells[9] += m.cells[9];
	cells[10] += m.cells[10];
	cells[11] += m.cells[11];
	cells[12] += m.cells[12];
	cells[13] += m.cells[13];
	cells[14] += m.cells[14];
	cells[15] += m.cells[15];
	return *this;
}

inline Vector3 Matrix4::operator*(Vector3 const& v) const
{
	return Vector3(cells[0]*v.x + cells[1]*v.y + cells[2]*v.z + cells[3],
	               cells[4]*v.x + cells[5]*v.y + cells[6]*v.z + cells[7],
	               cells[8]*v.x + cells[9]*v.y + cells[10]*v.z + cells[11]);
}

inline Real& Matrix4::cell(uint8_t offset)
{
	HppAssert(offset < 16, "Offset must be between 0 and 15!");
	return cells[offset];
}

inline Real& Matrix4::cell(uint8_t row, uint8_t col)
{
	HppAssert(row < 4, "Row must be between 0 and 3!");
	HppAssert(col < 4, "Column must be between 0 and 3!");
	return cells[(row << 2) + col];
}

inline Real const& Matrix4::cell(uint8_t offset) const
{
	HppAssert(offset < 16, "Offset must be between 0 and 15!");
	return cells[offset];
}

inline Real const& Matrix4::cell(uint8_t row, uint8_t col) const
{
	HppAssert(row < 4, "Row must be between 0 and 3!");
	HppAssert(col < 4, "Column must be between 0 and 3!");
	return cells[(row << 2) + col];
}

inline Matrix4 Matrix4::rotMatrix(Axis axis, Angle const& angle)
{
	switch (axis) {
	case X:
		return rotMatrixX(angle);
		break;
	case Y:
		return rotMatrixY(angle);
		break;
	case Z:
		return rotMatrixZ(angle);
		break;
	default:
		HppAssert(false, "");
		break;
	}
	return Matrix4();
}

inline Matrix4 Matrix4::rotMatrix(Quaternion const& q)
{
	Quaternion q_unit = q.normalized();
	return Matrix4(1 - 2*q_unit.y*q_unit.y - 2*q_unit.z*q_unit.z, 2*q_unit.x*q_unit.y - 2*q_unit.w*q_unit.z,     2*q_unit.x*q_unit.z + 2*q_unit.w*q_unit.y,     0,
	               2*q_unit.x*q_unit.y + 2*q_unit.w*q_unit.z,     1 - 2*q_unit.x*q_unit.x - 2*q_unit.z*q_unit.z, 2*q_unit.y*q_unit.z - 2*q_unit.w*q_unit.x,     0,
	               2*q_unit.x*q_unit.z - 2*q_unit.w*q_unit.y,     2*q_unit.y*q_unit.z + 2*q_unit.w*q_unit.x,     1 - 2*q_unit.x*q_unit.x - 2*q_unit.y*q_unit.y, 0,
	               0,                                             0,                                             0,                                             1);
}

inline Matrix4 Matrix4::rotMatrix(Vector3 const& axis, Angle const& angle)
{
	Real axis_len = axis.length();
	// Componets of axis as normalized vector
	Real u = axis.x / axis_len;
	Real v = axis.y / axis_len;
	Real w = axis.z / axis_len;
	// Multiplication of components
	Real uu = u*u;
	Real vv = v*v;
	Real ww = w*w;
	Real uv = u*v;
	Real uw = u*w;
	Real vw = v*w;
	// Sine, cosine and subractions from one
	Real s = angle.sin();
	Real c = angle.cos();
	Real cm = 1 - c;
	// Calculate result
	return Matrix4(uu + (1 - uu)*c, uv*cm - w*s,     uw*cm + v*s,     0,
	               uv*cm + w*s,     vv + (1 - vv)*c, vw*cm - u*s,     0,
	               uw*cm - v*s,     vw*cm + u*s,     ww + (1 - ww)*c, 0,
	               0,               0,               0,               1);
}

inline Matrix4 Matrix4::rotMatrixX(Angle const& angle)
{
	Real const s = angle.sin();
	Real const c = angle.cos();
	return Matrix4(1, 0, 0, 0,
	               0, c, -s, 0,
	               0, s, c, 0,
	               0, 0, 0, 1);
}

inline Matrix4 Matrix4::rotMatrixY(Angle const& angle)
{
	Real const s = angle.sin();
	Real const c = angle.cos();
	return Matrix4(c, 0, s, 0,
	               0, 1, 0, 0,
	               -s, 0, c, 0,
	               0, 0, 0, 1);
}

inline Matrix4 Matrix4::rotMatrixZ(Angle const& angle)
{
	Real const s = angle.sin();
	Real const c = angle.cos();
	return Matrix4(c, -s, 0, 0,
	               s, c, 0, 0,
	               0, 0, 1, 0,
	               0, 0, 0, 1);
}

inline Matrix4 Matrix4::translMatrix(Vector3 const& v)
{
	return Matrix4(1, 0, 0, v.x,
	               0, 1, 0, v.y,
	               0, 0, 1, v.z,
	               0, 0, 0, 1);
}

inline Matrix4 Matrix4::scaleMatrix(Vector3 const& v)
{
	return Matrix4(v.x, 0,   0,   0,
	               0,   v.y, 0,   0,
	               0,   0,   v.z, 0,
	               0,   0,   0,   1);
}

inline Matrix4 Matrix4::projectionMatrix(Hpp::Angle const& fov_y,
                                         Real aspectratio,
                                         Real near, Real far)
{
	Real f = 1 / (fov_y / 2).tan();
	return Matrix4(f / aspectratio, 0, 0,                       0,
	               0,               f, 0,                       0,
	               0,               0, (far+near) / (near-far), 2*far*near / (near-far),
	               0,               0, -1,                      0);
}

inline Real Matrix4::complement(uint8_t cell_id) const
{
	switch (cell_id) {
	case 0:
		return cells[5]*cells[10]*cells[15] +
		       cells[6]*cells[11]*cells[13] +
		       cells[7]*cells[9]*cells[14] -
		       cells[7]*cells[10]*cells[13] -
		       cells[6]*cells[9]*cells[15] -
		       cells[5]*cells[11]*cells[14];
	case 1:
		return cells[4]*cells[10]*cells[15] +
		       cells[6]*cells[11]*cells[12] +
		       cells[7]*cells[8]*cells[14] -
		       cells[7]*cells[10]*cells[12] -
		       cells[6]*cells[8]*cells[15] -
		       cells[4]*cells[11]*cells[14];
	case 2:
		return cells[4]*cells[9]*cells[15] +
		       cells[5]*cells[11]*cells[12] +
		       cells[7]*cells[8]*cells[13] -
		       cells[7]*cells[9]*cells[12] -
		       cells[5]*cells[8]*cells[15] -
		       cells[4]*cells[11]*cells[13];
	case 3:
		return cells[4]*cells[9]*cells[14] +
		       cells[5]*cells[10]*cells[12] +
		       cells[6]*cells[8]*cells[13] -
		       cells[6]*cells[9]*cells[12] -
		       cells[5]*cells[8]*cells[14] -
		       cells[4]*cells[10]*cells[13];
	case 4:
		return cells[1]*cells[10]*cells[15] +
		       cells[2]*cells[11]*cells[13] +
		       cells[3]*cells[9]*cells[14] -
		       cells[3]*cells[10]*cells[13] -
		       cells[2]*cells[9]*cells[15] -
		       cells[1]*cells[11]*cells[14];
	case 5:
		return cells[0]*cells[10]*cells[15] +
		       cells[2]*cells[11]*cells[12] +
		       cells[3]*cells[8]*cells[14] -
		       cells[3]*cells[10]*cells[12] -
		       cells[2]*cells[8]*cells[15] -
		       cells[0]*cells[11]*cells[14];
	case 6:
		return cells[0]*cells[9]*cells[15] +
		       cells[1]*cells[11]*cells[12] +
		       cells[3]*cells[8]*cells[13] -
		       cells[3]*cells[9]*cells[12] -
		       cells[1]*cells[8]*cells[15] -
		       cells[0]*cells[11]*cells[13];
	case 7:
		return cells[0]*cells[9]*cells[14] +
		       cells[1]*cells[10]*cells[12] +
		       cells[2]*cells[8]*cells[13] -
		       cells[2]*cells[9]*cells[12] -
		       cells[1]*cells[8]*cells[14] -
		       cells[0]*cells[10]*cells[13];
	case 8:
		return cells[1]*cells[6]*cells[15] +
		       cells[2]*cells[7]*cells[13] +
		       cells[3]*cells[5]*cells[14] -
		       cells[3]*cells[6]*cells[13] -
		       cells[2]*cells[5]*cells[15] -
		       cells[1]*cells[7]*cells[14];
	case 9:
		return cells[0]*cells[6]*cells[15] +
		       cells[2]*cells[7]*cells[12] +
		       cells[3]*cells[4]*cells[14] -
		       cells[3]*cells[6]*cells[12] -
		       cells[2]*cells[4]*cells[15] -
		       cells[0]*cells[7]*cells[14];
	case 10:
		return cells[0]*cells[5]*cells[15] +
		       cells[1]*cells[7]*cells[12] +
		       cells[3]*cells[4]*cells[13] -
		       cells[3]*cells[5]*cells[12] -
		       cells[1]*cells[4]*cells[15] -
		       cells[0]*cells[7]*cells[13];
	case 11:
		return cells[0]*cells[5]*cells[14] +
		       cells[1]*cells[6]*cells[12] +
		       cells[2]*cells[4]*cells[13] -
		       cells[2]*cells[5]*cells[12] -
		       cells[1]*cells[4]*cells[14] -
		       cells[0]*cells[6]*cells[13];
	case 12:
		return cells[1]*cells[6]*cells[11] +
		       cells[2]*cells[7]*cells[9] +
		       cells[3]*cells[5]*cells[10] -
		       cells[3]*cells[6]*cells[9] -
		       cells[2]*cells[5]*cells[11] -
		       cells[1]*cells[7]*cells[10];
	case 13:
		return cells[0]*cells[6]*cells[11] +
		       cells[2]*cells[7]*cells[8] +
		       cells[3]*cells[4]*cells[10] -
		       cells[3]*cells[6]*cells[8] -
		       cells[2]*cells[4]*cells[11] -
		       cells[0]*cells[7]*cells[10];
	case 14:
		return cells[0]*cells[5]*cells[11] +
		       cells[1]*cells[7]*cells[8] +
		       cells[3]*cells[4]*cells[9] -
		       cells[3]*cells[5]*cells[8] -
		       cells[1]*cells[4]*cells[11] -
		       cells[0]*cells[7]*cells[9];
	case 15:
		return cells[0]*cells[5]*cells[10] +
		       cells[1]*cells[6]*cells[8] +
		       cells[2]*cells[4]*cells[9] -
		       cells[2]*cells[5]*cells[8] -
		       cells[1]*cells[4]*cells[10] -
		       cells[0]*cells[6]*cells[9];
	}

	HppAssert(false, "Cell ID Overflow");
	return 0;
}



// ----------------------------------------
// ----------------------------------------
//
// Implementations of global inline functions related to Matrix4
//
// ----------------------------------------
// ----------------------------------------

inline Matrix4 transpose(Matrix4 const& m)
{
	return Matrix4(m.cell(0), m.cell(4), m.cell(8), m.cell(12),
	               m.cell(1), m.cell(5), m.cell(9), m.cell(13),
	               m.cell(2), m.cell(6), m.cell(10), m.cell(14),
	               m.cell(3), m.cell(7), m.cell(11), m.cell(15));
}


inline std::ostream& operator<<(std::ostream& strm, Matrix4 const& m)
{
	strm << '[';
	strm << m.cell(0);
	strm << ", ";
	strm << m.cell(1);
	strm << ", ";
	strm << m.cell(2);
	strm << ", ";
	strm << m.cell(3);
	strm << ']';
	strm << std::endl;
	strm << '[';
	strm << m.cell(4);
	strm << ", ";
	strm << m.cell(5);
	strm << ", ";
	strm << m.cell(6);
	strm << ", ";
	strm << m.cell(7);
	strm << ']';
	strm << std::endl;
	strm << '[';
	strm << m.cell(8);
	strm << ", ";
	strm << m.cell(9);
	strm << ", ";
	strm << m.cell(10);
	strm << ", ";
	strm << m.cell(11);
	strm << ']';
	strm << std::endl;
	strm << '[';
	strm << m.cell(12);
	strm << ", ";
	strm << m.cell(13);
	strm << ", ";
	strm << m.cell(14);
	strm << ", ";
	strm << m.cell(15);
	strm << ']';
	strm << std::endl;
	return strm;
}

}

#endif
