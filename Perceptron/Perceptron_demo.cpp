// Perceptron_demo.cpp: 定义控制台应用程序的入口点。
//
/*
    对一个Perceptron的功能进行演示
基于 https://www.youtube.com/watch?v=ntKn5TPHHAk&t=370s; 10.2 10.3
	Perceptron通过调整自身的属性(若干个权重)使得这个Perceptron描述的直线(对于二维数据点)能够将+1和-1两个类分离
	为了分离典型的二维点(分离两个类的直线除了有斜率还有截距),这个Perceptron使用了3个权w0,w1和w2, w0和w1用于确
定直线的斜率m, w1和w2用于确定直线的截距.
	int Perceptron::train分为两部分.
	第一部分是猜测部分int Perceptron::guess,就是用这个Perceptron自身的w0,w1,w2来尝试将两个类正确划分,然后返回这
个Perceptron认为的标签(+1或者-1).
	第二部分是纠正部分,假如认为标签同真实标签一致则什么都不做.假如不一致,则根据error(2或者+2)来纠正w0,w1和w2,
纠正的量Δweights[i]=inputs[i]*error*learning_rate.其中inputs[0]=x, inputs[1]=y, inputs[2]=bias.据观察,bias的值
应该根据数据点的量,数据点的分布半径等确定,以获得较快的收敛速度.这里的bias = 数据点的高斯分布标准差

	1. 梯度下降法的Δ量 = cost function对相应权值的偏导,比如这里的 inputs[i]*error
	2. 斜率m = -w0/w1	截距b = -w2*bias/w1
	3. 所有数据点的bias必须是相同的
	4. learning_rate或者bias太大可能导致点群密集时的最后划分无法收敛.
*/

#include "stdafx.h"
#include<opencv2\opencv.hpp>
#include "data_point.h"
#include "Perceptron.h"

//Ajust parameters here
std::string winName = "Divide points";
	const int width = 500, height = 500;
	const float STD = (float)width / 4.0f;
	const int point_number = 100;
	const int radius = 8, thickness = 2;
	const int outliars_allowed = point_number / 100;	//Increasing this number can speed up the convergence but lose some accuracy.
	const float learning_rate = 1.0f;
	const float m = -2.0f, b = 130.0f;			

	cv::Mat canvas(width, height, CV_8UC3), mask;
	data_point point_set[point_number];

void draw_point_prior(data_point pt);
float f_(float x, float guess_m, float guess_b);
float f(float x);
float cartX(float X);
float cartY(float Y);
float pixelX(float X);
float pixelY(float Y);

int main()
{
	Perceptron brain;
	brain.setLR(learning_rate);
	cv::RNG rng(cv::getTickCount());
	canvas = cv::Scalar::all(255);

	//Draw coordinate lines
	line(canvas, cv::Point(cartX(0), cartY(height/2)), cv::Point(cartX(0), cartY(-height / 2)), cv::Scalar(0, 0, 0), 1);
	line(canvas, cv::Point(cartX(-width/2), cartY(0)), cv::Point(cartX(width/2), cartY(0)), cv::Scalar(0, 0, 0), 1);

	//Draw dividing line
	line(canvas, cv::Point(cartX(-width/2), cartY(f(-width / 2))), cv::Point(cartX(width/2), cartY(f(width/2))), cv::Scalar(255, 0, 0), 2);

	//Create random points and label them.
	for (int i = 0; i < point_number; i++)
	{
		data_point &pt = point_set[i];
		float cart_x = rng.gaussian(STD),
			cart_y = rng.gaussian(STD);
		float pixel_x = cartX(cart_x),
			pixel_y = cartY(cart_y);
		pt = data_point(pixel_x, pixel_y, cart_x, cart_y, STD);

		if (cart_y >= cart_x*m + b) pt.label = 1;	//Division criteria of points
		else pt.label = -1;
		draw_point_prior(pt);
		imshow(winName, canvas);
	}
	cv::waitKey(0);

	//Guess part
	printf("Guess starts...\n");
		for (int i = 0; i < point_number; i++)
		{
			int guess = brain.guess(point_set[i]);
			//If guess right, light it with green, otherwise red.
			if (guess == point_set[i].label)
				circle(canvas, cv::Point2f(point_set[i].pixelX, point_set[i].pixelY), radius/2, cv::Scalar(0,255,0), cv::FILLED);
			else
				circle(canvas, cv::Point2f(point_set[i].pixelX, point_set[i].pixelY), radius / 2, cv::Scalar(0, 0, 255), cv::FILLED);
			imshow(winName, canvas);
		}
		cv::Mat scratch = canvas.clone();
		printf("Guess over. Press any key to train\n\n");
		cv::waitKey(0);

	//Train part
	printf("Strat training...\n");
	
	float pre_time = cv::getTickCount() / cv::getTickFrequency();
	int loops = 0;
	for(;;)
	{ 
		int outliars = 0;
		loops++;
		for (int i = 0; i < point_number; i++)
		{
			data_point &pt = point_set[i];

			int error_exsit= brain.train(pt);
			outliars += error_exsit;
			if(error_exsit)
				circle(scratch, cv::Point2f(pt.pixelX, pt.pixelY), radius / 2, cv::Scalar(0, 0, 255), cv::FILLED);
			else
				circle(scratch, cv::Point2f(pt.pixelX, pt.pixelY), radius / 2, cv::Scalar(0, 255, 0), cv::FILLED);
			
			//Calculate parameters for the guess line
			float guess_m = -brain.weight[0] / brain.weight[1],
				guess_b = -brain.weight[2] * pt.bias / brain.weight[1],
				y1 = f_(-width/2, guess_m, guess_b),
				y2 = f_(width/2, guess_m, guess_b);
				printf("m=%f	b=%f\n", guess_m, guess_b);

			//Draw the guess line
			mask = scratch.clone();
			line(mask, cv::Point(cartX(-width / 2), cartY(y1)), cv::Point(cartX(width / 2), cartY(y2)), cv::Scalar(125, 50, 0), 2);
			
			imshow(winName, mask);
			cv::waitKey(1);
			
		}

		//If all the guesses correspond to all the labels, end training.
		if (outliars <= outliars_allowed)
		{ 
			float current_time = cv::getTickCount() / cv::getTickFrequency();
			float cost = current_time - pre_time;
			int minute = (int)cost / 60;
			int second = (int)cost % 60;
			printf("\nTrain over!\n%d minutes %d seconds cost, %d loops and %d outliars in total.\nPress any key to start rectified guess.\n\n", minute, second, loops, outliars);
			break;
		}
	}
	cv::waitKey(0);

	//Guess again using the corrected weights.
	printf("Guess again using the corrected weights...\n");
	for (int i = 0; i < point_number; i++)
	{
		int guess = brain.guess(point_set[i]);

		if (guess == point_set[i].label)
			circle(canvas, cv::Point2f(point_set[i].pixelX, point_set[i].pixelY), radius / 2, cv::Scalar(0, 255, 0), cv::FILLED);
		else
			circle(canvas, cv::Point2f(point_set[i].pixelX, point_set[i].pixelY), radius / 2, cv::Scalar(0, 0, 255), cv::FILLED);
		imshow(winName, canvas);
		cv::waitKey(33);
	}
	printf("Rectified guess over. Press any key to quit.\n");
	cv::waitKey(0);

    return 0;
}

void draw_point_prior(data_point pt)
{
	if (pt.label == 1)
		circle(canvas, cv::Point2f(pt.pixelX, pt.pixelY), radius, cv::Scalar::all(0), thickness);
	else
		circle(canvas, cv::Point2f(pt.pixelX, pt.pixelY), radius, cv::Scalar::all(0), cv::FILLED);
}

float f_(float x, float guess_m, float guess_b)
{
	return guess_m * x + guess_b;
}

float f(float x)
{
	return m * x + b;
}

float cartX(float X)
{
	return X + width / 2.0;
}

float cartY(float Y)
{
	return height / 2.0 - Y;
}

float pixelX(float X)
{
	return X - width / 2.0;
}

float pixelY(float Y)
{
	return height / 2.0 - Y;
}


