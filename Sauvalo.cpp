#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include <iostream>


cv::Mat sauvola(const cv::Mat &gray_scale, double k, int kernerl_width)      //kͨ��Ϊ0.����kernerl_widthͨ��Ϊ��ʮ
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

			

			if (xmin == 0 && ymin == 0) {                            //����Kernerl�����Ͻ�δ�뿪ͼƬ���Ͻǣ����е��diff��sqrDiff.at<float>����
				diff.at<float>(row, col) = integral_sum_tailored.at<float>(ymax, xmax);       //Kernerl���½ǵ�integralImag��integral_sum_tailored.at<float>Sqrt
				sqrDiff.at<float>(row, col) = integral_sqrSum_tailored.at<float>(ymax, xmax);
			}
			else if (xmin > 0 && ymin == 0) {
				diff.at<float>(row, col) = integral_sum_tailored.at<float>(ymax, xmax) - integral_sum_tailored.at<float>(ymax, xmin-1);             //����Kernerl�϶�δ�뿪�����е㡱��diff��sqrDiff.at<float>����
				sqrDiff.at<float>(row, col) = integral_sqrSum_tailored.at<float>(ymax, xmax) - integral_sqrSum_tailored.at<float>(ymax, xmin-1);   //                                  Kernerl���½�-���½�
			}
			else if (xmin == 0 && ymin > 0) {
				diff.at<float>(row, col) = integral_sum_tailored.at<float>(ymax, xmax) - integral_sum_tailored.at<float>(ymin-1, xmax);            //����Kernerl���δ�뿪�����е㡱��diff��sqrDiff.at<float>����
				sqrDiff.at<float>(row, col) = integral_sqrSum_tailored.at<float>(ymax, xmax) - integral_sqrSum_tailored.at<float>(ymin - 1, xmax);; //                        Kernerl���½�-���Ͻ�
			}
			else {
				float diagsum = integral_sum_tailored.at<float>(ymax, xmax) + integral_sum_tailored.at<float>(ymin-1, xmin-1);       //����Kernerl�������ȫ�뿪�����е��diff��sqrDiff.at<float>����
				float idiagsum = integral_sum_tailored.at<float>(ymin-1, xmax) + integral_sum_tailored.at<float>(ymax, xmin-1);      //���Խ�����Ԫ�غ�-���Խ�����Ԫ�غ�
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