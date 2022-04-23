
#include "NeuralNetwork.hpp"
#include <iostream>

NeuralNetwork::NeuralNetwork(int layerCount, int* topology, float learningRate) {
    this->learningRate = learningRate;
    inputLayer = Matrix(topology[0], 1);
    outputLayer = Matrix(topology[layerCount - 1], 1);
    weightsAll = vector<Matrix>(layerCount - 1);
    deltas = vector<Matrix>(layerCount - 1);
    rawNodeValues = vector<Matrix>(layerCount - 1);
    for (unsigned int i = 0; i < weightsAll.size(); i++)
        weightsAll[i] = Matrix(topology[i + 1], topology[i]); // [cols] + 1 for bias
}

Matrix NeuralNetwork::ForwardStep(Matrix input, Matrix weights) { // last input element = 1 for bias
    return weights.Multiply(input);
}

Matrix NeuralNetwork::ForwardPropagate(Matrix input) {
    inputLayer = input;
    Matrix output = input;
    for (unsigned int i = 0; i < weightsAll.size(); i++) {
        output = ForwardStep(output.Sigmoid(), weightsAll[i]);
        rawNodeValues[i] = output;
    }
    return output.Sigmoid();
}

void NeuralNetwork::Backpropagate(Matrix output, Matrix target) {
    Matrix derivative = rawNodeValues[rawNodeValues.size() - 1].DeSigmoid();
    deltas[deltas.size() - 1] = output.Negative().Add(target).MultiplyByElement(derivative);
    for (unsigned int i = deltas.size() - 1; i > 0; i--) {
        Matrix weightedDeltaSum = weightsAll[i].Transpose().Multiply(deltas[i]);
        deltas[i - 1] = rawNodeValues[i - 1].DeSigmoid().MultiplyByElement(weightedDeltaSum);
    }
    for (unsigned int i = 0; i < weightsAll.size(); i++) {
        // Error
        Matrix partialError = rawNodeValues[i].Sigmoid().MultiplyByElement(deltas[i]).MultiplyScalar(learningRate);
        weightsAll[i] = weightsAll[i].AddVector(partialError);
    }
}

float NeuralNetwork::GetError(Matrix output, Matrix target) {
    Matrix negative = target.Negative();
    Matrix difference = output.Add(negative);
    return difference.Transpose().Multiply(difference).At(0, 0);
}