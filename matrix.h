#ifndef HPP_MATRIX_H
#define HPP_MATRIX_H

#include "exception.h"
#include "assert.h"

#include <glob.h>
#include <cstring>

namespace Hpp
{

class Matrix
{

public:

	inline Matrix(size_t rows, size_t cols);
	inline ~Matrix(void);

	// Matrix operations
	inline Matrix transpose(void) const;
	inline float determinant(void) const;
	inline Matrix inverse(void) const;
	inline Matrix adjugate(void) const;

	inline size_t getRows(void) const;
	inline size_t getCols(void) const;

	// Copy constructor and assignment operator
	inline Matrix(Matrix const& m);
	inline Matrix operator=(Matrix const& m);

	// Cell accessing methods and operators. Methods throw exception, but
	// operator[] not, so be sure to check ranges.
	inline float& at(size_t row, size_t col);
	inline float at(size_t row, size_t col) const;
	inline float& operator[](size_t index);
	inline float operator[](size_t index) const;

	// Plus and minus operators
	inline Matrix operator+(Matrix const& m) const;
	inline Matrix operator-(Matrix const& m) const;
	inline Matrix operator+=(Matrix const& m);
	inline Matrix operator-=(Matrix const& m);

	// Multiply and divide operators
	inline Matrix operator*(Matrix const& m) const;
	inline Matrix operator*=(Matrix const& m);
	inline Matrix operator*(float f) const;
	inline Matrix operator/(float f) const;
	inline Matrix operator*=(float f);
	inline Matrix operator/=(float f);

private:

	size_t rows, cols;

	float* cells;

	inline float realDeterminant(void) const;

};

inline Matrix operator*(float f, Matrix const& m);


// ----------------------------------------
// Implementations of inline functions
// ----------------------------------------

inline Matrix::Matrix(size_t rows, size_t cols) :
rows(rows),
cols(cols)
{
	if (rows == 0 || cols == 0) throw Exception("Zero dimension matrix not allowed!");
	cells = new float[rows * cols];
}

inline Matrix::~Matrix(void)
{
	delete[] cells;
}

inline Matrix Matrix::transpose(void) const
{
	Matrix result(cols, rows);
	size_t index;
	size_t result_index = 0;
	for (size_t result_row = 0; result_row < cols; result_row ++) {
		index = result_row;
		for (size_t result_col = 0; result_col < rows; result_col ++) {
			result.cells[result_index] = cells[index];
			result_index ++;
			index += cols;
		}
	}
	return result;
}

inline float Matrix::determinant(void) const
{
	if (rows != cols) throw Exception("Determinant requires square matrix!");
	return realDeterminant();
}

inline Matrix Matrix::inverse(void) const
{
	if (rows != cols) throw Exception("Inverse requires square matrix!");
	float det = realDeterminant();
	if (det == 0) throw Exception("Matrix is not invertible!");

	Matrix m = adjugate();

	return m / det;
}

inline Matrix Matrix::adjugate(void) const
{
	if (rows != cols) throw Exception("Adjugation requires square matrix!");

	Matrix result(rows, cols);

	size_t result_index = 0;
	for (size_t row = 0; row < rows; row ++) {
		for (size_t col = 0; col < cols; col ++) {

			Matrix subm(cols-1, cols-1);

			size_t subm_index = 0;
			size_t index = 0;
			for (size_t row2 = 0; row2 < rows; row2 ++) {
				for (size_t col2 = 0; col2 < cols; col2 ++) {
					if (col != row2 && row != col2) {
						subm.cells[subm_index] = cells[index];
						subm_index ++;
					}
					index ++;
				}
			}

			float det = subm.realDeterminant();

			if ((row + col) % 2 == 0) result.cells[result_index] = det;
			else result.cells[result_index] = -det;
			result_index ++;
		}
	}

	return result;

}

inline size_t Matrix::getRows(void) const
{
	return rows;
}

inline size_t Matrix::getCols(void) const
{
	return cols;
}

inline Matrix::Matrix(Matrix const& m) :
rows(m.rows),
cols(m.cols)
{
	cells = new float[m.rows * m.cols];
	memcpy(cells, m.cells, sizeof(float) * rows * cols);
}

inline Matrix Matrix::operator=(Matrix const& m)
{
	delete[] cells;
	cells = new float[m.rows * m.cols];
	memcpy(cells, m.cells, sizeof(float) * rows * cols);
	return *this;
}

inline float& Matrix::at(size_t row, size_t col)
{
	if (row >= rows) throw Exception("Row out of range!");
	if (col >= cols) throw Exception("Column out of range!");
	return cells[row * cols + col];
}

inline float Matrix::at(size_t row, size_t col) const
{
	if (row >= rows) throw Exception("Row out of range!");
	if (col >= cols) throw Exception("Column out of range!");
	return cells[row * cols + col];
}

inline float& Matrix::operator[](size_t index)
{
	HppAssert(index < rows*cols, "Index out of range!");
	return cells[index];
}

inline float Matrix::operator[](size_t index) const
{
	HppAssert(index < rows*cols, "Index out of range!");
	return cells[index];
}

inline Matrix Matrix::operator+(Matrix const& m) const
{
	if (m.cols != cols || m.rows != rows) throw Exception("Matrix dimensions does not agree!");
	Matrix result(rows, cols);
	for (size_t index = 0; index < rows*cols; index ++) {
		result.cells[index] = cells[index] + m.cells[index];
	}
	return result;
}

inline Matrix Matrix::operator-(Matrix const& m) const
{
	if (m.cols != cols || m.rows != rows) throw Exception("Matrix dimensions does not agree!");
	Matrix result(rows, cols);
	for (size_t index = 0; index < rows*cols; index ++) {
		result.cells[index] = cells[index] - m.cells[index];
	}
	return result;
}

inline Matrix Matrix::operator+=(Matrix const& m)
{
	if (m.cols != cols || m.rows != rows) throw Exception("Matrix dimensions does not agree!");
	for (size_t index = 0; index < rows*cols; index ++) {
		cells[index] += m.cells[index];
	}
	return *this;
}

inline Matrix Matrix::operator-=(Matrix const& m)
{
	if (m.cols != cols || m.rows != rows) throw Exception("Matrix dimensions does not agree!");
	for (size_t index = 0; index < rows*cols; index ++) {
		cells[index] -= m.cells[index];
	}
	return *this;
}

inline Matrix Matrix::operator*(Matrix const& m) const
{
	if (cols != m.rows) throw Exception("Matrix dimensions does not agree!");
	Matrix result(rows, m.cols);
	size_t result_index = 0;
	for (size_t result_row = 0; result_row < rows; result_row ++) {
		for (size_t result_col = 0; result_col < m.cols; result_col ++) {
			float cell_result = 0;
			for (size_t mult = 0; mult < cols; mult ++) {
				cell_result += cells[result_row * cols + mult] * m.cells[mult * m.cols + result_col];
			}
			result.cells[result_index] = cell_result;
			result_index ++;
		}
	}
	return result;
}

inline Matrix Matrix::operator*=(Matrix const& m)
{
	*this = *this * m;
	return *this;
}

inline Matrix Matrix::operator*(float f) const
{
	Matrix result(rows, cols);
	for (size_t index = 0; index < rows*cols; index ++) {
		result.cells[index] = cells[index] * f;
	}
	return result;
}

inline Matrix Matrix::operator/(float f) const
{
	Matrix result(rows, cols);
	for (size_t index = 0; index < rows*cols; index ++) {
		result.cells[index] = cells[index] / f;
	}
	return result;
}

inline Matrix Matrix::operator*=(float f)
{
	for (size_t index = 0; index < rows*cols; index ++) {
		cells[index] *= f;
	}
	return *this;
}

inline Matrix Matrix::operator/=(float f)
{
	Matrix result(rows, cols);
	for (size_t index = 0; index < rows*cols; index ++) {
		cells[index] /= f;
	}
	return *this;
}

inline float Matrix::realDeterminant(void) const
{
	HppAssert(cols == rows, "Square matrix required!");
	if (cols == 1) {
		return cells[0];
	} else if (cols == 2) {
		return cells[0] * cells[3] - cells[1] * cells[2];
	}
	float result = 0;
	for (size_t col = 0; col < cols; col ++) {
		Matrix subm(cols-1, cols-1);
		size_t subm_index = 0;
		size_t index = cols;
		for (size_t row = 1; row < rows; row ++) {
			for (size_t col2 = 0; col2 < rows; col2 ++) {
				if (col2 != col) {
					subm.cells[subm_index] = cells[index];
					subm_index ++;
				}
				index ++;
			}
		}
		float num = cells[col] * subm.realDeterminant();
		if (col % 2 == 0) {
			result += num;
		} else {
			result -= num;
		}
	}
	return result;
}

inline Matrix operator*(float f, Matrix const& m)
{
	return m * f;
}

}

#endif
