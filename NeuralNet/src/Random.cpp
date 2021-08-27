/*
 * Random.cpp
 *
 *  Created on: Sep 15, 2020
 *      Author: wade4
 */
#include <cstdlib>

double fRand(double fMin, double fMax) {
	double f = (double) rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}
