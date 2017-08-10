#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

#include "ChangedImage_DataLine.h"
struct CImageSaver_Config
{
	// Get from outside
	std::string strOutputPath;
	// Generate from inside

	CImageSaver_Config(){}
	CImageSaver_Config(const std::string& strOutputPathIN) :
		strOutputPath(strOutputPathIN)
	{}
};
class CImageSaver
	: public CChangedImage_DataLine
{
private:
	CImageSaver_Config m_Config;
public:
	CImageSaver();
	~CImageSaver();

	bool InitComponent(CImageSaver_Config&& ConfigIN);
	// override
	void SendChangedImageWithTime_ID(cv::Mat matImage, const std::string& strTime, int nID);
private:
	std::string ConvertID(const int& ID);
};

