#ifndef HPP_MATRIX3_H
#define HPP_MATRIX3_H

#include "angle.h"
#include "vector2.h"
#include "real.h"

namespace Hpp
{

class Matrix3
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
	static inline Matrix3 rotMatrix(Angle const& angle);

	// Static function for generating translation matrix
	static inline Matrix3 translMatrix(Vector2 const& v);

	// Static function for generating scaling matrix
	static inline Matrix3 scaleMatrix(Vector2 const& v);

private:

	// Cells of Matrix
	Real cells[9];

	// Function to get complement of spcific cell in Matrix
	inline Real subdeterminant(uint8_t cell_id) const;
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

	out.cells[0] = cells[0] * m.cells[0] + cells[3] * m.cells[1] + cells[6] * m.cells[2];
	out.cells[1] = cells[1] * m.cells[0] + cells[4] * m.cells[1] + cells[7] * m.cells[2];
	out.cells[2] = cells[2] * m.cells[0] + cells[5] * m.cells[1] + cells[8] * m.cells[2];

	out.cells[3] = cells[0] * m.cells[3] + cells[3] * m.cells[4] + cells[6] * m.cells[5];
	out.cells[4] = cells[1] * m.cells[3] + cells[4] * m.cells[4] + cells[7] * m.cells[5];
	out.cells[5] = cells[2] * m.cells[3] + cells[5] * m.cells[4] + cells[8] * m.cells[5];

	out.cells[6] = cells[0] * m.cells[6] + cells[3] * m.cells[7] + cells[6] * m.cells[8];
	out.cells[7] = cells[1] * m.cells[6] + cells[4] * m.cells[7] + cells[7] * m.cells[8];
	out.cells[8] = cells[2] * m.cells[6] + cells[5] * m.cells[7] + cells[8] * m.cells[8];

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

	cells[0] = cells_old[0] * m.cells[0] + cells_old[3] * m.cells[1] + cells_old[6] * m.cells[2];
	cells[1] = cells_old[1] * m.cells[0] + cells_old[4] * m.cells[1] + cells_old[7] * m.cells[2];
	cells[2] = cells_old[2] * m.cells[0] + cells_old[5] * m.cells[1] + cells_old[8] * m.cells[2];

	cells[3] = cells_old[0] * m.cells[3] + cells_old[3] * m.cells[4] + cells_old[6] * m.cells[5];
	cells[4] = cells_old[1] * m.cells[3] + cells_old[4] * m.cells[4] + cells_old[7] * m.cells[5];
	cells[5] = cells_old[2] * m.cells[3] + cells_old[5] * m.cells[4] + cells_old[8] * m.cells[5];

	cells[6] = cells_old[0] * m.cells[6] + cells_old[3] * m.cells[7] + cells_old[6] * m.cells[8];
	cells[7] = cells_old[1] * m.cells[6] + cells_old[4] * m.cells[7] + cells_old[7] * m.cells[8];
	cells[8] = cells_old[2] * m.cells[6] + cells_old[5] * m.cells[7] + cells_old[8] * m.cells[8];

	return *this;
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

inline Matrix3 Matrix3::rotMatrix(Angle const& angle)
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
