#include "stdafx.h"
#include "MPL.h"
#include <iostream>


float sigmoid(float x)
{
	return (1.f / (1.f + exp(-x)));
}

float dsigmoid(float x)
{
	return (x*(1.f - x));
}

MPL::MPL()
{
}

MPL::MPL(const int input_nodes, const int hidden_nodes, const int output_nodes, const float lr)
{
	learning_rate = lr;
	nodes_of_inputs = input_nodes;
	nodes_of_outputs = output_nodes;
	wih = Mat(hidden_nodes, input_nodes, CV_32F);
	who = Mat(output_nodes, hidden_nodes, CV_32F);
	hidden_bias = Mat(hidden_nodes, 1, CV_32F);
	output_bias = Mat(output_nodes, 1, CV_32F);
	randomize(wih);
	randomize(who);
	randomize(hidden_bias);
	randomize(output_bias);
}




MPL::~MPL()
{
}

float* MPL::predict(const float Inputs[])
{
	Mat inputs(nodes_of_inputs, 1, CV_32F);
	for (int i = 0; i < nodes_of_inputs; i++)
		inputs.at<float>(i, 0) = Inputs[i];
	Mat hidden = wih * inputs + hidden_bias;
	hidden = map(hidden, sigmoid);
	Mat outputs = who * hidden + output_bias;
	outputs = map(outputs, sigmoid);
	float *output_array = new float[nodes_of_outputs];
	for (int i = 0; i < nodes_of_outputs; i++)
		output_array[i] = outputs.at<float>(i, 0);
	return output_array;
}


void MPL::train(const float Inputs[], const float Targets[])
{
	Mat inputs(nodes_of_inputs, 1, CV_32F);
	for (int i = 0; i < nodes_of_inputs; i++)
		inputs.at<float>(i, 0) = Inputs[i];
	Mat hidden = wih * inputs + hidden_bias;
	hidden = map(hidden, sigmoid);
	Mat outputs = who * hidden + output_bias;
	outputs = map(outputs, sigmoid);

	Mat target(nodes_of_outputs, 1, CV_32F);
	for (int i = 0; i < nodes_of_outputs; i++)
		target.at<float>(i, 0) = Targets[i];

	//CALCULATE ERRORS FOR THE HIDDEN LAYER AND THE OUTPUT LAYER
	Mat output_error = target - outputs;
	Mat who_t = who.t();
	Mat hidden_error = who_t * output_error;  //who_t() is a simplified weight matrix for calculating the hidden_error, 
											//with the normalizing factors being bypassed.;
	//CALCULATE DELTA WEIGHTS
	Mat output_gradiant = map(outputs, dsigmoid);
	Mat H_t = hidden.t();
	output_gradiant = output_error.mul(output_gradiant) *learning_rate;
	Mat delta_who = output_gradiant * H_t;
	Mat hidden_gradiant = map(hidden, dsigmoid);
	hidden_gradiant = hidden_error.mul(hidden_gradiant) * learning_rate;
	Mat delta_wih = hidden_gradiant * inputs.t();

	//CALCULATE NEW WEIGHTS
	who += delta_who;
	wih += delta_wih;
	//CALCULATE NEW BIAS
	output_bias += output_gradiant;
	hidden_bias += hidden_gradiant;
}

Mat MPL::map(Mat x, float (*func)(float))
{
	for (MatIterator_<float> it = x.begin<float>(); it != x.end<float>(); it++)
		*it = func(*it);
	return x;
}

void MPL::randomize(Mat & x)
{
	RNG rng(getTickCount());
	MatIterator_<float> it = x.begin<float>();
	for (; it != x.end<float>(); it++)
		*it = rng.uniform(-1.0, 1.0);
}


