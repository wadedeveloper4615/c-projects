//============================================================================
// Name        : Matrix.cpp
// Author      : Christopher D. Wade
// Version     :
// Copyright   : 2020 Christopher D. Wade
// Description : implementation for the matrix class implementation
//============================================================================
#include "Matrix.h"
#include "Array.h"

Matrix::Matrix(unsigned rowSize, unsigned colSize, double low, double high) {
	bias = NULL;
	m_rowSize = rowSize;
	m_colSize = colSize;
	m_matrix = new Array[rowSize];
	for (IndexType i = 0; i < rowSize; i++) {
		m_matrix[i].setup(colSize, fRand(low, high));
	}
}

Matrix::Matrix(unsigned rowSize, unsigned colSize, double value) {
	bias = NULL;
	m_rowSize = rowSize;
	m_colSize = colSize;
	m_matrix = new Array[rowSize];
	for (IndexType i = 0; i < rowSize; i++) {
		m_matrix[i].setup(colSize, value);
	}
}

Matrix::Matrix(initializer_list<initializer_list<double>> list) {
	bias = NULL;
	m_rowSize = (int) list.size();
	m_colSize = (int) (list.begin())->size();
	m_matrix = new Array[m_rowSize];
	for (auto i = 0; i < m_rowSize; i++) {
		m_matrix[i].setup(m_colSize, 0);
		for (auto j = 0; j < m_colSize; j++) {
			m_matrix[i][j] = ((list.begin() + i)->begin())[j];
		}
	}
}

Matrix::~Matrix() {
	delete[] m_matrix;
}

Array Matrix::dot(Array& rhs) const {
    Array temp(rhs.getSize(),false);
	for(IndexType i=0; i<rhs.getSize(); i++) {
		double accumulate = 0;
        if (bias == NULL)
        	accumulate = 0;
        else
        	accumulate = (*bias)[i];
		for(IndexType j=0; j<m_rowSize; j++) {
			accumulate += m_matrix[i][j] * rhs[j];
		}
		temp[i] = accumulate;
	}
    return temp;
}

Array& Matrix::operator[](const IndexType &index) {
	return m_matrix[index];
}

void Matrix::print(char *text) const {
	cout << text << endl;
	string delimiter = "";
	for (IndexType i = 0; i < m_rowSize; i++) {
		delimiter = "";
		for (IndexType j = 0; j < m_colSize; j++) {
			cout << delimiter << m_matrix[i][j];
			delimiter = ",";
		}
		cout << std::endl;
	}
}

IndexType Matrix::getRows() const {
	return m_rowSize;
}

IndexType Matrix::getCols() const {
	return m_colSize;
}

void Matrix::setBias(Array *other) {
	bias = other;
}
