#include "stdafx.h"
#include "ImageSaver.h"


CImageSaver::CImageSaver()
{
}


CImageSaver::~CImageSaver()
{
}

bool CImageSaver::InitComponent(CImageSaver_Config&& ConfigIN)
{
	m_Config = std::move(ConfigIN);
	return true;
}

// override
void CImageSaver::SendChangedImageWithTime_ID(cv::Mat matImage,
	const std::string& strTime,
	int nID)
{
	std::string fileName = m_Config.strOutputPath + ConvertID(nID) + "_" + strTime + ".jpg";
	cv::imwrite(fileName, matImage);
}

std::string CImageSaver::ConvertID(const int& ID)
{
	if (ID < 26)
		return std::string(1, ID + 'A');
	char curChar = ID % 26 + 'A';
	std::string curStr(1, curChar);	
	return ConvertID(static_cast<int>((double)ID / 26)) + curStr;
}