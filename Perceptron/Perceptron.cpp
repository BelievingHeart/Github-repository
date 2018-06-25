#include "stdafx.h"
#include "Perceptron.h"
#include<cstdlib>
#include<ctime>


Perceptron::Perceptron()
{
	//srand((unsigned)time(NULL));
	for (int i = 0; i < 3; i++)
		weight[i] = rand();
}


Perceptron::~Perceptron()
{
}

int Perceptron::train(const data_point &pt)
{
	float ERROR = pt.label - guess(pt);
	if (std::abs(ERROR) > 1.0)
	{
		weight[0] += ERROR * pt.cartX*lr;
		weight[1] += ERROR * pt.cartY*lr;
		weight[2] += ERROR * pt.bias*lr;
		return 1;
	}
	else return 0;
}

void Perceptron::setLR(float LR)
{
	lr = LR;
}

void Perceptron::showWeight() const
{
	printf("The weights are %f and %f.", weight[0], weight[1]);
}

int Perceptron::guess(const data_point &pt)
{
	float Guess = weight[0] * pt.cartX + weight[1] * pt.cartY + weight[2] * pt.bias;
	if (Guess >= 0)
		return 1;
	else
		return -1;
}
