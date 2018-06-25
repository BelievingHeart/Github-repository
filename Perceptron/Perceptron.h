#pragma once
#include"data_point.h"

class Perceptron
{
public:
	Perceptron();
	~Perceptron();

	float weight[3];
	float lr = 1;
	int train(const data_point &pt);
	void setLR(float LR);
	void showWeight() const;
	int guess(const data_point &pt);
};

