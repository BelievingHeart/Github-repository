// Neural_Network_Demo.cpp: 定义控制台应用程序的入口点。
//

//增加nodes和增加learning_rate要比增加loops收敛快得多

#include "stdafx.h"
#include<opencv2\core.hpp>
#include<opencv2\shape.hpp>
#include<opencv2\highgui.hpp>
#include<iostream>
#include"MPL.h"
using namespace cv;

const int train_loops = 1000;
const float learning_rate = 1.1f;
const int nodes[] = { 2,1000,1 };

int main()
{
	srand((unsigned int)getTickCount());
	const float input_array[][2]= { { 1.f, 0.f }, { 1.f,1.f }, { 0.f,1.f }, { 0.f,0.f }};
	const float target_array[][1] = { {1.f},{0.f},{1.f},{0.f} };
	std::vector<std::string> XOR;
	XOR.push_back("1 XOR 0 gets ");
	XOR.push_back("1 XOR 1 gets ");
	XOR.push_back("0 XOR 1 gets ");
	XOR.push_back("0 XOR 0 gets ");
	MPL nn(nodes[0], nodes[1], nodes[2], learning_rate);

	//INTIAL GUESS
	printf("Initial guess:\n");
	for (int i = 0; i < 4; i++)
	{ 
		float *output = nn.predict(input_array[i]);
		std::cout << XOR[i]<<output[0] << "\n";
	}
	printf("Training...\n");

	float pre_time = getTickCount() / getTickFrequency();
	for (int i = 0; i < train_loops; i++)
	{
		int flags = rand() % 4;
		switch (flags)
		{
		default:
			break;
		case 0:nn.train(input_array[0], target_array[0]); break;
		case 1:nn.train(input_array[1], target_array[1]); break;
		case 2:nn.train(input_array[2], target_array[2]); break;
		case 3:nn.train(input_array[3], target_array[3]); break;
		}
	}
	float cur_time = getTickCount() / getTickFrequency();
	int training_time = (int)cur_time - (int)pre_time;
	int minutes = training_time / 60;
	int seconds = training_time % 60;
	printf("Training over. Time cost: %d minutes %d seconds\n\n", minutes, seconds);

	printf("Rectified guess:\n");
	for (int i = 0; i < 4; i++)
	{
		float *output = nn.predict(input_array[i]);
		std::cout << XOR[i] << output[0] << "\n";
	}
	//VISUALIZE THE RESULT
	int width = 500;
	Mat canvas(width, width, CV_8UC1);
	int resolution = 10,
		rows = width / resolution,
		cols = width / resolution;
	for(int i=0;i<rows;i++)
		for (int j = 0; j < cols; j++)
		{
			float inputs[] = { (float)j / (float)cols,(float)i / (float)rows };
			rectangle(canvas, Point(j*resolution, i*resolution), Point((j + 1)*resolution, (i + 1)*resolution), nn.predict(inputs)[0] * 255, FILLED);
		}
	imshow("XOR DISTRIBUTION", canvas);
	waitKey(0);

	return 0;
}
