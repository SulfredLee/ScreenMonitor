#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <string>
class CroppedImage
{
public:
	cv::Mat m_img;
	std::string m_timeStamp;
	int m_ID;
public:
	CroppedImage();
	~CroppedImage();
};

