#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <string>
class CChangedImage_DataLine
{
public:
	CChangedImage_DataLine(){}
	virtual ~CChangedImage_DataLine(){}

	virtual void SendChangedImageWithTime_ID(cv::Mat matImage, const std::string& strTime, int nID){}
};

