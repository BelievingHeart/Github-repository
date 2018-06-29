#pragma once
#include<opencv2\core.hpp>
using namespace cv;
class MPL
{
private:
	Mat wih, who, hidden_bias, output_bias;
	float learning_rate = 0.1f;
	int nodes_of_inputs = 0, nodes_of_outputs = 0;

	Mat map(Mat x, float(*func)(float));
	void randomize(Mat &x);

public:
	MPL();
	MPL(const int input_nodes, const int hidden_nodes, const int output_nodes, const float lr);
	~MPL();
	float* predict(const float Inputs[]);
	void train(const float Inputs[], const float Targets[]);
};


