/*
 * Array.h
 *
 *  Created on: Sep 15, 2020
 *      Author: wade4
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <iostream>
#include <initializer_list>
#include <cstdlib>
#include <ctime>

using namespace std;

typedef unsigned IndexType;

double fRand(double fMin, double fMax);

class Array {
	IndexType m_size;
	double *m_matrix;
public:
	Array();
	Array(IndexType size, double low, double high);
	Array(IndexType size, double value);
	virtual ~Array();
	Array(initializer_list<double> list);
	void setup(unsigned size, double low, double high);
	void setup(unsigned size, double value);
	double& operator[](const IndexType &index);
	friend Array operator-(Array lhs, Array rhs);
	friend Array operator*(Array lhs, Array rhs);
	friend Array dot(Array lhs, Array rhs);
	void print(char *text) const;
	IndexType getSize() const;
};

#endif /* ARRAY_H_ */
