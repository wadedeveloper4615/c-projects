//============================================================================
// Name        : NeuralNet.cpp
// Author      : Christopher D. Wade
// Version     :
// Copyright   : 2020 Christopher D. Wade
// Description : Main module for a backpropaation neural net
//============================================================================
#include <iostream>
#include "Matrix.h"
#include "Array.h"
#include "NeuralNetwork.h"

using namespace std;

void networkTest() {
	Matrix trainingInput = { { 0.0, 0.0 }, { 0.0, 1.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } };
	Matrix trainingOutput = { { 0.0 }, { 0.0 }, { 0.0 }, { 1.0 } };
	NeuralNetwork net(2, 4, 1, 4);
	net.loadTrainingData(trainingInput, trainingOutput);
	net.train(2);
	cout << "Number of input neurons  : " << net.getInputSize() << endl;
	cout << "Number of hidden neurons : " << net.getMiddleSize() << endl;
	cout << "Number of output neurons : " << net.getOutputSize() << endl;
}

void matrixArrayTest() {
	Matrix matrix1 = { { 1.0, 2.0 }, { 3.0, 4.0 } };
	Array array1 = { 3.0, 4.0 };
	matrix1.print("matrix1");
	array1.print("array1");

	Array array2 = matrix1.dot(array1);
	array2.print("Array2");

	Array array3 = { 3.0, 4.0 };
	Array array4 = { 3.0, 4.0 };
	Array array5 = array4 - array3;
	array5.print("array5");

	Array array6 = { 3.0, 4.0 };
	Array array7 = { 3.0, 4.0 };
	Array array8 = array7 * array6;
	array8.print("array8");
}

int main() {
	cout << "Neural Net v1.0" << endl;
	matrixArrayTest();
	networkTest();
	return 0;
}
