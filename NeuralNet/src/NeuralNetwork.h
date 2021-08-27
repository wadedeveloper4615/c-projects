/*
 * NeuralNetwork.h
 *
 *  Created on: Sep 15, 2020
 *      Author: wade4
 */

#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include <iostream>
#include <list>
#include <cstdlib>
#include <math.h>
#include <iomanip>
#include <cmath>
#include <limits>
#include "Matrix.h"
#include "Array.h"

const double lr = 0.1f;

class NeuralNetwork {
	Array inputLayer;
	Matrix inputMiddleWeights;
	Array middleLayer;
	Matrix middleOutputWeights;
	Array outputLayer;
	Array outputLayerError;
	Array middleLayerError;
	Array hiddenLayerBias;
	Array outputLayerBias;
	Matrix trainingInput;
	Matrix trainingOutput;
	Array middleLayerDelta;
	Array outputLayerDelta;
	IndexType inputSize;
	IndexType middleSize;
	IndexType outputSize;
	IndexType numTrainingSets;
public:
	NeuralNetwork(IndexType inputSize, IndexType middleSize, IndexType outputSize, IndexType exampleSize);
	virtual ~NeuralNetwork();
	void shuffle(int *array, IndexType n);
	void loadTrainingData(Matrix &input, Matrix &output);
	void train(IndexType epochs);
	IndexType getInputSize();
	IndexType getMiddleSize();
	IndexType getOutputSize();
protected:
	double sigmoid(double x);
	double sigmoidDerivative(double x);
	Array sigmoid(Array layerIn);
	Array sigmoidDerivative(Array layerIn);
	Array dot(Matrix& lhs, Array& rhs);
	Array abs(Array rhs);
	double mean(Array rhs);
};
#endif /* NEURALNETWORK_H_ */
