#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <string>
class FullImage
{
public:
	cv::Mat m_image;
	std::string m_timeStamp;
public:
	FullImage();
	~FullImage();
};

