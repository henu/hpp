#ifndef HPP_MATRIX3_H
#define HPP_MATRIX3_H

#include "bytev.h"
#include "json.h"
#include "angle.h"
#include "vector2.h"
#include "quaternion.h"
#include "real.h"
#include "axis.h"
#include "serializable.h"
#include "deserializable.h"

#include <stdint.h>

namespace Hpp
{

class Matrix3 : public Serializable, public Deserializable
{

public:

	static Matrix3 const IDENTITY;
	static Matrix3 const ZERO;

	// Constructor
	inline Matrix3(void);

	// Copy constructor and assignment operator
	inline Matrix3(Matrix3 const& m);
	inline Matrix3& operator=(Matrix3 const& m);

	// Miscellaneous constructors
	inline Matrix3(Real a, Real b, Real c,
	               Real d, Real e, Real f,
	               Real g, Real h, Real i);
	inline Matrix3(Json const& json);

	// Conversion functions
	inline Json toJson(void) const;
	inline ByteV toBytes(bool bigendian = true) const;

	// Comparison operators
	inline bool operator==(Matrix3 const& m) const;
	inline bool operator!=(Matrix3 const& m) const;

	// Miscellaneous operators
	inline Matrix3 operator*(Matrix3 const& m) const;
	inline Matrix3 operator+(Matrix3 const& m) const;
	inline Matrix3 operator*(Real r) const;
	inline Matrix3 operator/(Real r) const;
	inline Matrix3 const& operator*=(Matrix3 const& m);
	inline Matrix3 const& operator+=(Matrix3 const& m);

	// Vector operators
	inline Vector2 operator*(Vector2 const& v) const;
	inline Vector3 operator*(Vector3 const& v) const;

	// Function to get determinant of this Matrix
	inline Real determinant(void) const;

	// Function to get inverse of this Matrix
	Matrix3 inverse(void) const;

	// Function to get cells array
	inline Real const* getCells(void) const;

	// Functions to use cells in matrix
	inline Real& cell(uint8_t offset);
	inline Real& cell(uint8_t row, uint8_t offset);
	inline Real const& cell(uint8_t offset) const;
	inline Real const& cell(uint8_t row, uint8_t offset) const;

	// Static functions for generating rotation matrices
	static inline Matrix3 rotMatrix2d(Angle const& angle);
	static inline Matrix3 rotMatrix(Axis axis, Angle const& angle);
	static inline Matrix3 rotMatrix(Quaternion const& q);
	static inline Matrix3 rotMatrixX(Angle const& angle);
	static inline Matrix3 rotMatrixY(Angle const& angle);
	static inline Matrix3 rotMatrixZ(Angle const& angle);
	static inline Matrix3 rotMatrix(Vector3 const& axis, Angle const& angle);

	// Static function for generating various special matrices.
	static inline Matrix3 translMatrix(Vector2 const& v);
	static inline Matrix3 scaleMatrix(Vector2 const& v);

private:

	// Cells of Matrix
	Real cells[9];

	// Function to get complement of spcific cell in Matrix
	inline Real subdeterminant(uint8_t cell_id) const;

	// Virtual functions needed by superclasses Serializable and Deserializable
	inline virtual void doSerialize(ByteV& result) const;
	inline virtual void doDeserialize(std::istream& strm);

};

inline Matrix3 transpose(Matrix3 const& m);

inline std::ostream& operator<<(std::ostream& strm, Matrix3 const& m);



// ----------------------------------------
// ----------------------------------------
//
// Implementation of inline functions of Matrix3
//
// ----------------------------------------
// ----------------------------------------


inline Matrix3::Matrix3(void)
{
}

inline Matrix3::Matrix3(Matrix3 const& m)
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
}

inline Matrix3& Matrix3::operator=(Matrix3 const& m)
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
	return *this;
}

inline Matrix3::Matrix3(Real a, Real b, Real c,
                        Real d, Real e, Real f,
                        Real g, Real h, Real i)
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
}

inline Matrix3::Matrix3(Json const& json)
{
	// Check JSON validity
	if (json.getType() != Json::ARRAY || json.getArraySize() != 9) throw Exception("JSON for Matrix3 must be array that has 9 numbers!");
	for (uint8_t cell_id = 0; cell_id < 9; ++ cell_id) {
		Json const& cell_json = json.getItem(cell_id);
		if (cell_json.getType() != Json::NUMBER) throw Exception("Found non-number cell from Matrix3 JSON!");
		cells[cell_id] = cell_json.getNumber();
	}
}

inline Json Matrix3::toJson(void) const
{
	Json result = Json::newArray();
	for (uint8_t cell_id = 0; cell_id < 9; ++ cell_id) {
		result.addItem(Json::newNumber(cells[cell_id]));
	}
	return result;
}

inline ByteV Matrix3::toBytes(bool bigendian) const
{
	ByteV result;
	result.reserve(4*9);
	for (uint8_t cell_id = 0; cell_id < 9; cell_id ++) {
		result += floatToByteV(cells[cell_id], bigendian);
	}
	return result;
}

inline bool Matrix3::operator==(Matrix3 const& m) const
{
	for (size_t cells_id = 0; cells_id < 9; cells_id ++) {
		if (cells[cells_id] != m.cells[cells_id]) {
			return false;
		}
	}
	return true;
}

inline bool Matrix3::operator!=(Matrix3 const& m) const
{
	return !(*this == m);
}

inline Matrix3 Matrix3::operator*(Matrix3 const& m) const
{
	Matrix3 out;
	for (uint8_t row = 0; row < 3; ++ row) {
		for (uint8_t col = 0; col < 3; ++ col) {
			uint8_t ofs = row*3 + col;
			out.cells[ofs] = 0;
			for (uint8_t mult = 0; mult < 3; ++ mult) {
				out.cells[ofs] += cells[row*3 + mult] * m.cells[mult*3 + col];
			}
		}
	}
	return out;
}

inline Matrix3 Matrix3::operator/(Real r) const
{
	HppAssert(r != 0, "Division by zero!");
	Matrix3 out;
	out.cells[0] = cells[0] / r;
	out.cells[1] = cells[1] / r;
	out.cells[2] = cells[2] / r;
	out.cells[3] = cells[3] / r;
	out.cells[4] = cells[4] / r;
	out.cells[5] = cells[5] / r;
	out.cells[6] = cells[6] / r;
	out.cells[7] = cells[7] / r;
	out.cells[8] = cells[8] / r;
	return out;
}

inline Matrix3 const& Matrix3::operator*=(Matrix3 const& m)
{
	Real cells_old[9];
	for (uint8_t cells_id = 0;
	     cells_id < 9;
	     cells_id ++) {
		cells_old[cells_id] = cells[cells_id];
	}
	for (uint8_t row = 0; row < 3; ++ row) {
		for (uint8_t col = 0; col < 3; ++ col) {
			uint8_t ofs = row*3 + col;
			cells[ofs] = 0;
			for (uint8_t mult = 0; mult < 3; ++ mult) {
				cells[ofs] += cells_old[row*3 + mult] * m.cells[mult*3 + col];
			}
		}
	}

	return *this;
}

inline Matrix3 Matrix3::operator+(Matrix3 const& m) const
{
	return Matrix3(cells[0] + m.cells[0],
	               cells[1] + m.cells[1],
	               cells[2] + m.cells[2],
	               cells[3] + m.cells[3],
	               cells[4] + m.cells[4],
	               cells[5] + m.cells[5],
	               cells[6] + m.cells[6],
	               cells[7] + m.cells[7],
	               cells[8] + m.cells[8]);
}

inline Matrix3 Matrix3::operator*(Real r) const
{
	return Matrix3(cells[0] * r,
	               cells[1] * r,
	               cells[2] * r,
	               cells[3] * r,
	               cells[4] * r,
	               cells[5] * r,
	               cells[6] * r,
	               cells[7] * r,
	               cells[8] * r);
}

inline Matrix3 const& Matrix3::operator+=(Matrix3 const& m)
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
	return *this;
}

inline Vector2 Matrix3::operator*(Vector2 const& v) const
{
	return Vector2(cells[0]*v.x + cells[1]*v.y + cells[2],
	               cells[3]*v.x + cells[4]*v.y + cells[5]);
}

inline Vector3 Matrix3::operator*(Vector3 const& v) const
{
	return Vector3(cells[0]*v.x + cells[1]*v.y + cells[2]*v.z,
	               cells[3]*v.x + cells[4]*v.y + cells[5]*v.z,
	               cells[6]*v.x + cells[7]*v.y + cells[8]*v.z);
}

inline Real Matrix3::determinant(void) const
{
	return cells[0] * subdeterminant(0)
	     - cells[1] * subdeterminant(1)
	     + cells[2] * subdeterminant(2);
}

inline Matrix3 Matrix3::inverse(void) const
{

	// First get determinant and ensure it's not zero
	Real det = determinant();
	HppAssert(det != 0, "Cannot make inverse matrix because determinant is zero!");

	// Form adjugate matrix
	Matrix3 adj = Matrix3(+subdeterminant(0), -subdeterminant(3), +subdeterminant(6),
	                      -subdeterminant(1), +subdeterminant(4), -subdeterminant(7),
	                      +subdeterminant(2), -subdeterminant(5), +subdeterminant(8));

	// Ensure inverse matrix is valid
	#ifndef NDEBUG
// TODO: Check also cond of matrix!
	Matrix3 check = *this * (adj / det);
	HppAssert(check.cells[0] > 0.9 && check.cells[0] < 1.1 &&
	          check.cells[1] > -0.1 && check.cells[1] < 0.1 &&
	          check.cells[2] > -0.1 && check.cells[2] < 0.1 &&
	          check.cells[3] > -0.1 && check.cells[3] < 0.1 &&
	          check.cells[4] > 0.9 && check.cells[4] < 1.1 &&
	          check.cells[5] > -0.1 && check.cells[5] < 0.1 &&
	          check.cells[6] > -0.1 && check.cells[6] < 0.1 &&
	          check.cells[7] > -0.1 && check.cells[7] < 0.1 &&
	          check.cells[8] > 0.9 && check.cells[8] < 1.1, "Matrix inversion has failed!");
	#endif
	return adj / det;

}

inline Real const* Matrix3::getCells(void) const
{
	return cells;
}

inline Real& Matrix3::cell(uint8_t offset)
{
	HppAssert(offset < 9, "Offset must be between 0 and 8!");
	return cells[offset];
}

inline Real& Matrix3::cell(uint8_t row, uint8_t col)
{
	HppAssert(row < 3, "Row must be between 0 and 2!");
	HppAssert(col < 3, "Column must be between 0 and 2!");
	return cells[(row / 3) + col];
}

inline Real const& Matrix3::cell(uint8_t offset) const
{
	HppAssert(offset < 9, "Offset must be between 0 and 8!");
	return cells[offset];
}

inline Real const& Matrix3::cell(uint8_t row, uint8_t col) const
{
	HppAssert(row < 3, "Row must be between 0 and 2!");
	HppAssert(col < 3, "Column must be between 0 and 2!");
	return cells[(row / 3) + col];
}

inline Matrix3 Matrix3::rotMatrix2d(Angle const& angle)
{
	Real const s = angle.sin();
	Real const c = angle.cos();
	return Matrix3(c, -s, 0,
	               s, c, 0,
	               0, 0, 1);
}

inline Matrix3 Matrix3::rotMatrix(Axis axis, Angle const& angle)
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
	return Matrix3();
}

inline Matrix3 Matrix3::rotMatrix(Quaternion const& q)
{
	Quaternion q_unit = q.normalized();
	return Matrix3(1 - 2*q_unit.y*q_unit.y - 2*q_unit.z*q_unit.z, 2*q_unit.x*q_unit.y - 2*q_unit.w*q_unit.z,     2*q_unit.x*q_unit.z + 2*q_unit.w*q_unit.y,
	               2*q_unit.x*q_unit.y + 2*q_unit.w*q_unit.z,     1 - 2*q_unit.x*q_unit.x - 2*q_unit.z*q_unit.z, 2*q_unit.y*q_unit.z - 2*q_unit.w*q_unit.x,
	               2*q_unit.x*q_unit.z - 2*q_unit.w*q_unit.y,     2*q_unit.y*q_unit.z + 2*q_unit.w*q_unit.x,     1 - 2*q_unit.x*q_unit.x - 2*q_unit.y*q_unit.y);
}

inline Matrix3 Matrix3::rotMatrix(Vector3 const& axis, Angle const& angle)
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
	return Matrix3(uu + (1 - uu)*c, uv*cm - w*s,     uw*cm + v*s,
	               uv*cm + w*s,     vv + (1 - vv)*c, vw*cm - u*s,
	               uw*cm - v*s,     vw*cm + u*s,     ww + (1 - ww)*c);
}

inline Matrix3 Matrix3::rotMatrixX(Angle const& angle)
{
	Real const s = angle.sin();
	Real const c = angle.cos();
	return Matrix3(1, 0, 0,
	               0, c, -s,
	               0, s, c);
}

inline Matrix3 Matrix3::rotMatrixY(Angle const& angle)
{
	Real const s = angle.sin();
	Real const c = angle.cos();
	return Matrix3(c, 0, s,
	               0, 1, 0,
	               -s, 0, c);
}

inline Matrix3 Matrix3::rotMatrixZ(Angle const& angle)
{
	Real const s = angle.sin();
	Real const c = angle.cos();
	return Matrix3(c, -s, 0,
	               s, c, 0,
	               0, 0, 1);
}

inline Matrix3 Matrix3::translMatrix(Vector2 const& v)
{
	return Matrix3(1, 0, v.x,
	               0, 1, v.y,
	               0, 0, 1);
}

inline Matrix3 Matrix3::scaleMatrix(Vector2 const& v)
{
	return Matrix3(v.x, 0,   0,
	               0,   v.y, 0,
	               0,   0,   1);
}

inline Real Matrix3::subdeterminant(uint8_t cell_id) const
{
	switch (cell_id) {
	case 0:
		return cells[4]*cells[8] - cells[7]*cells[5];
	case 1:
		return cells[3]*cells[8] - cells[6]*cells[5];
	case 2:
		return cells[3]*cells[7] - cells[6]*cells[4];
	case 3:
		return cells[1]*cells[8] - cells[7]*cells[2];
	case 4:
		return cells[0]*cells[8] - cells[6]*cells[2];
	case 5:
		return cells[0]*cells[7] - cells[6]*cells[1];
	case 6:
		return cells[1]*cells[5] - cells[4]*cells[2];
	case 7:
		return cells[0]*cells[5] - cells[3]*cells[2];
	case 8:
		return cells[0]*cells[4] - cells[3]*cells[1];
	}

	HppAssert(false, "Cell ID Overflow");
	return 0;
}

inline void Matrix3::doSerialize(ByteV& result) const
{
	result.reserve(result.size() + 4*9);
	for (size_t cell_id = 0; cell_id < 9; ++ cell_id) {
		result += floatToByteV(cells[cell_id]);
	}
}

inline void Matrix3::doDeserialize(std::istream& strm)
{
	char buf[4];
	for (size_t cell_id = 0; cell_id < 9; ++ cell_id) {
		strm.read(buf, 4);
		if (strm.eof()) {
			throw Exception("Unexpected end of data!");
		}
		cells[cell_id] = cStrToFloat(buf, true);
	}
}



// ----------------------------------------
// ----------------------------------------
//
// Implementations of global inline functions related to Matrix3
//
// ----------------------------------------
// ----------------------------------------

inline Matrix3 transpose(Matrix3 const& m)
{
	return Matrix3(m.cell(0), m.cell(3), m.cell(6),
	               m.cell(1), m.cell(4), m.cell(7),
	               m.cell(2), m.cell(5), m.cell(8));
}


inline std::ostream& operator<<(std::ostream& strm, Matrix3 const& m)
{
	strm << '[' << m.cell(0) << ", " << m.cell(1) << ", " << m.cell(2) << ']'<< std::endl;
	strm << '[' << m.cell(3) << ", " << m.cell(4) << ", " << m.cell(5) << ']'<< std::endl;
	strm << '[' << m.cell(6) << ", " << m.cell(7) << ", " << m.cell(8) << ']'<< std::endl;
	return strm;
}

}

#endif
