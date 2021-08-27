//============================================================================
// Name        : NeuralNetwork.cpp
// Author      : Christopher D. Wade
// Version     :
// Copyright   : 2020 Christopher D. Wade
// Description : implementation for a basic backpropagation net
//============================================================================
#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(IndexType inputSize, IndexType middleSize, IndexType outputSize, IndexType exampleSize) :
		inputSize(inputSize),
		middleSize(middleSize),
		outputSize(outputSize),
		numTrainingSets(exampleSize),
		inputLayer(inputSize, 0.0, 1.0),
		inputMiddleWeights(inputSize, middleSize, 0.0, 1.0),
		middleLayer(middleSize, 0.0, 1.0),
		middleOutputWeights(middleSize, outputSize, 0.0, 1.0),
		outputLayer(outputSize, 0.0, 1.0),
		outputLayerError(outputSize, 0.0, 1.0),
		middleLayerError(middleSize, 0.0, 1.0),
		hiddenLayerBias(middleSize, -1.0),
		outputLayerBias(outputSize, -1.0),
		trainingInput(inputSize, exampleSize, 0.0, 1.0),
		trainingOutput(outputSize, exampleSize, 0.0, 1.0),
		middleLayerDelta(middleSize, 0.0, 1.0),
		outputLayerDelta(outputSize, 0.0, 1.0) {
}

NeuralNetwork::~NeuralNetwork() {
}

void NeuralNetwork::shuffle(int *array, IndexType n) {
	if (n > 1) {
		size_t i;
		for (i = 0; i < n - 1; i++) {
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

void NeuralNetwork::loadTrainingData(Matrix &input, Matrix &output) {
	numTrainingSets = input.getRows();
	trainingInput = input;
	trainingOutput = output;
}

double NeuralNetwork::sigmoid(double x) {
	return 1 / (1 + exp(-x));
}

double NeuralNetwork::sigmoidDerivative(double x) {
	return sigmoid(x) * (1 - sigmoid(x));
}

Array NeuralNetwork::sigmoid(Array layerIn) {
	Array layerOut(layerIn.getSize(),0);
	for (auto j = 0; j < layerIn.getSize(); j++) {
		layerOut[j] = sigmoid(layerIn[j]);
	}
	return layerOut;
}

Array NeuralNetwork::sigmoidDerivative(Array layerIn) {
	Array layerOut(layerIn.getSize(),0);
	for (auto j = 0; j < layerIn.getSize(); j++) {
		layerIn[j] = sigmoidDerivative(layerIn[j]);
	}
	return layerOut;
}

IndexType NeuralNetwork::getInputSize() {
	return inputSize;
}

IndexType NeuralNetwork::getMiddleSize() {
	return middleSize;
}

IndexType NeuralNetwork::getOutputSize() {
	return outputSize;
}

Array NeuralNetwork::dot(Matrix& lhs, Array& rhs) {
	return lhs.dot(rhs);
}

Array NeuralNetwork::abs(Array rhs) {
    Array temp(rhs.getSize(),0);
	for(auto i=0; i<rhs.getSize(); i++) {
		temp[i] = std::abs(rhs[i]);
	}
    return temp;
}

double NeuralNetwork::mean(Array rhs) {
    double total = 0;
	for(auto i=0; i<rhs.getSize(); i++) {
		total+= rhs[i];
	}
    return total/((double)rhs.getSize());
}

void NeuralNetwork::train(IndexType epochs) {
	cout << "examples=" << numTrainingSets << endl;
	int *trainingSetOrder = new int[numTrainingSets];
	for (int o = 0; o < numTrainingSets; o++) {
		trainingSetOrder[o] = o;
	}
	for (int n = 0; n < epochs; n++) {
		shuffle(trainingSetOrder, numTrainingSets);
		for (int x = 0; x < numTrainingSets; x++) {
			int i = trainingSetOrder[x];

			// Forward pass
			inputLayer = trainingInput[i];

			middleLayer = sigmoid(dot(inputMiddleWeights, inputLayer));

			outputLayer = sigmoid(dot(middleOutputWeights, middleLayer));

			// Backprop
			outputLayerError = trainingOutput[i] - outputLayer;

			cout << "Error : " << fixed << setprecision(6) << mean(abs(outputLayerError)) << endl;

	        outputLayerDelta = outputLayerError*sigmoidDerivative(outputLayer);

	        middleLayerError = dot(middleOutputWeights, outputLayerDelta);

	        middleLayerDelta = middleLayerError * sigmoidDerivative(middleLayer);

			for (int j = 0; j < outputSize; j++) {
				for (int k = 0; k < middleSize; k++) {
					middleOutputWeights[k][j] += middleLayer[k] * outputLayerDelta[j];
				}
			}

			for (int j = 0; j < middleSize; j++) {
				for (int k = 0; k < inputSize; k++) {
					inputMiddleWeights[k][j] += trainingInput[i][k]	* middleLayerDelta[j];
				}
			}
		}
	}
}

