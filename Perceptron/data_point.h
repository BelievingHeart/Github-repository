#pragma once
class data_point
{
public:
	float pixelX, pixelY, cartX, cartY, bias = 100.0;
	int label;
	data_point();
	data_point(float pixel_X, float pixel_Y, float cart_X, float cart_Y, float STD);
	~data_point();
};

