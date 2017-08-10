#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <string>

class CImage_DataLine
{
public:
	CImage_DataLine(){}
	virtual ~CImage_DataLine(){}

	virtual void SendImageWithTime(cv::Mat matImage, const std::string& strTime){}
};

