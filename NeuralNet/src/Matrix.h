/*
 * Matrix.h
 *
 *  Created on: Sep 15, 2020
 *      Author: wade4
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>
#include <initializer_list>
#include <cstdlib>
#include <ctime>
#include "Array.h"

using namespace std;

typedef unsigned IndexType;

double fRand(double fMin, double fMax);

class Matrix {
	IndexType m_rowSize;
	IndexType m_colSize;
	Array *m_matrix;
	Array *bias;
public:
	Matrix(IndexType rowSize, IndexType colSize, double low, double high);
	Matrix(IndexType rowSize, IndexType colSize, double value);
	Matrix(initializer_list<initializer_list<double>> list);
	virtual ~Matrix();
	Array dot(Array& rhs) const;
	Array& operator[](const IndexType &index);
	void print(char *text) const;
	IndexType getRows() const;
	IndexType getCols() const;
	void setBias(Array *other);
};
#endif /* MATRIX_H_ */
