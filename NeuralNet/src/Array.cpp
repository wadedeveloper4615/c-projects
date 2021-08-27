//============================================================================
// Name        : Array.cpp
// Author      : Christopher D. Wade
// Version     :
// Copyright   : 2020 Christopher D. Wade
// Description : implementation for the array class implementation
//============================================================================
#include "Array.h"

Array::Array() {
	m_size = 0;
	m_matrix = NULL;
}

Array::Array(unsigned size, double low, double high) {
	m_size = size;
	m_matrix = new double[m_size];
	for (auto i = 0; i < m_size; i++) {
		m_matrix[i] = fRand(low, high);
	}
}

Array::Array(unsigned size, double value) {
	m_size = size;
	m_matrix = new double[m_size];
	for (auto i = 0; i < m_size; i++) {
		m_matrix[i] = value;
	}
}

Array::~Array() {
	if (!m_matrix) delete[] m_matrix;
}

Array::Array(initializer_list<double> list) {
	m_size = (int) list.size();
	m_matrix = new double[m_size];
	auto count = 0;
	for (auto element : list) {
		m_matrix[count] = element;
		count++;
	}
}

void Array::setup(unsigned size, double low, double high) {
	m_size = size;
	m_matrix = new double[m_size];
	for (auto i = 0; i < m_size; i++) {
		m_matrix[i] = fRand(low, high);
	}
}

void Array::setup(unsigned size, double value) {
	m_size = size;
	m_matrix = new double[m_size];
	for (auto i = 0; i < m_size; i++) {
		m_matrix[i] = value;
	}
}

double& Array::operator[](const IndexType &index) {
	return m_matrix[index];
}

Array operator -(Array lhs, Array rhs) {
    Array temp(rhs.getSize(),0);
	for(auto i=0; i<rhs.getSize(); i++) {
		temp[i] = lhs[i] - rhs[i];
	}
    return temp;
}

Array operator *(Array lhs, Array rhs) {
    Array temp(rhs.getSize(),0);
	for(auto i=0; i<rhs.getSize(); i++) {
		temp[i] = lhs[i] * rhs[i];
	}
    return temp;
}

Array dot(Array lhs, Array rhs) {
	return lhs * rhs;
}

void Array::print(char *text) const {
	cout << text << endl;
	string delimiter = "";
	for (unsigned j = 0; j < m_size; j++) {
		cout << delimiter << m_matrix[j];
		delimiter = ",";
	}
	cout << endl;
}

IndexType Array::getSize() const {
	return m_size;

}
