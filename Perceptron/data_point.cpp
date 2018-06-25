#include "stdafx.h"
#include "data_point.h"

data_point::data_point()
{
}

data_point::data_point(float pixel_X, float pixel_Y, float cart_X, float cart_Y, float STD)
{
	pixelX = pixel_X;
	pixelY = pixel_Y;
	cartX = cart_X;
	cartY = cart_Y;
	bias = STD;
}

data_point::~data_point()
{
}
