#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include <iostream>


cv::Mat sauvola(const cv::Mat &gray_scale, double k, int kernerl_width)      //k通常为0.几，kernerl_width通常为几十
{
	cv::Mat integral_sum, integral_sqrSum;
	cv::Mat mean = cv::Mat::zeros(gray_scale.size(), CV_32F);
	cv::Mat threshold = cv::Mat::zeros(gray_scale.size(), CV_8UC1);
	cv::Mat diff = cv::Mat::zeros(gray_scale.size(), CV_32F);
	cv::Mat sqrDiff = cv::Mat::zeros(gray_scale.size(), CV_32F);
	cv::Mat Std = cv::Mat::zeros(gray_scale.size(), CV_32F);
	cv::Mat binary_image = cv::Mat::zeros(gray_scale.size(), CV_8UC1);
	integral(gray_scale, integral_sum, integral_sqrSum, CV_32F, CV_32F);
	cv::Mat integral_sum_tailored(integral_sum, cv::Rect(1, 1, gray_scale.cols, gray_scale.rows));
	cv::Mat integral_sqrSum_tailored(integral_sqrSum, cv::Rect(1, 1, gray_scale.cols, gray_scale.rows));
	int xmin, ymin, xmax, ymax, area, whalf = kernerl_width >> 1, height = gray_scale.rows, width = gray_scale.cols;


	for(int row=0;row<integral_sum_tailored.rows;row++)
		for (int col = 0; col < integral_sum_tailored.cols; col++)
		{
			xmin = std::max(0, col - whalf);
			ymin = std::max(0, row - whalf);
			xmax = std::min(width - 1, col + whalf);
			ymax = std::min(height - 1, row + whalf);

			area = (xmax - xmin + 1) * (ymax - ymin + 1);
			if (area <= 0)
			{
				exit(1);
			}

			

			if (xmin == 0 && ymin == 0) {                            //假如Kernerl的左上角未离开图片左上角，则中点的diff和sqrDiff.at<float>等于
				diff.at<float>(row, col) = integral_sum_tailored.at<float>(ymax, xmax);       //Kernerl右下角的integralImag和integral_sum_tailored.at<float>Sqrt
				sqrDiff.at<float>(row, col) = integral_sqrSum_tailored.at<float>(ymax, xmax);
			}
			else if (xmin > 0 && ymin == 0) {
				diff.at<float>(row, col) = integral_sum_tailored.at<float>(ymax, xmax) - integral_sum_tailored.at<float>(ymax, xmin-1);             //假如Kernerl上端未离开，则“中点”的diff、sqrDiff.at<float>等于
				sqrDiff.at<float>(row, col) = integral_sqrSum_tailored.at<float>(ymax, xmax) - integral_sqrSum_tailored.at<float>(ymax, xmin-1);   //                                  Kernerl右下角-左下角
			}
			else if (xmin == 0 && ymin > 0) {
				diff.at<float>(row, col) = integral_sum_tailored.at<float>(ymax, xmax) - integral_sum_tailored.at<float>(ymin-1, xmax);            //假如Kernerl左端未离开，则“中点”的diff、sqrDiff.at<float>等于
				sqrDiff.at<float>(row, col) = integral_sqrSum_tailored.at<float>(ymax, xmax) - integral_sqrSum_tailored.at<float>(ymin - 1, xmax);; //                        Kernerl右下角-右上角
			}
			else {
				float diagsum = integral_sum_tailored.at<float>(ymax, xmax) + integral_sum_tailored.at<float>(ymin-1, xmin-1);       //假如Kernerl左和上完全离开，则中点的diff、sqrDiff.at<float>等于
				float idiagsum = integral_sum_tailored.at<float>(ymin-1, xmax) + integral_sum_tailored.at<float>(ymax, xmin-1);      //主对角线两元素和-副对角线两元素和
				diff.at<float>(row, col) = diagsum - idiagsum;

				float sqdiagsum = integral_sqrSum_tailored.at<float>(ymax, xmax) + integral_sqrSum_tailored.at<float>(ymin - 1, xmin - 1);
				float sqidiagsum = integral_sqrSum_tailored.at<float>(ymin - 1, xmax) + integral_sqrSum_tailored.at<float>(ymax, xmin - 1);
				sqrDiff.at<float>(row, col) = sqdiagsum - sqidiagsum;
			}

			mean.at<float>(row, col) = diff.at<float>(row, col) / area;
			Std.at<float>(row,col) = sqrt((sqrDiff.at<float>(row, col) - diff.at<float>(row, col) * diff.at<float>(row, col) / area) / (area - 1));
			threshold.at<uchar>(row, col)= mean.at<float>(row, col) * (1 + k * ((Std.at<float>(row, col) / 128) - 1));
			if (gray_scale.at<uchar>(row, col) < threshold.at<uchar>(row, col))

				binary_image.at<uchar>(row, col) = 255;
			else
				binary_image.at<uchar>(row, col) = 0;
		}
	return binary_image;
}