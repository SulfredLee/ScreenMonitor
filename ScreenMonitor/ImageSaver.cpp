#include "stdafx.h"
#include "ImageSaver.h"


ImageSaver::ImageSaver()
{
}


ImageSaver::~ImageSaver()
{
}

//Override
void ImageSaver::ThreadMain()
{
	while (!myThread::IsEndThread())
	{
		msgType curMsgType;
		m_msgQ.wait_and_pop(curMsgType);
		switch (curMsgType)
		{
		case IMG_IN:
			SaveImg();
			break;
		default:
			break;
		}
	}
}

//Callback
void ImageSaver::ImageSaver_DataLine(boost::shared_ptr<CroppedImage> ptr)
{
	m_imageQ.push(ptr);
	m_msgQ.push(IMG_IN);
}

void ImageSaver::SaveImg()
{
	boost::shared_ptr<CroppedImage> ptr;
	if (!m_imageQ.try_pop(ptr))
		return;

	std::string fileName;
	fileName = m_outputPath + ConvertID(ptr->m_ID) + "_" + ptr->m_timeStamp + ".jpg";
	cv::imwrite(fileName, ptr->m_img);
}

bool ImageSaver::Init(const std::string& outputPath)
{
	m_outputPath = outputPath;

	return true;
}

std::string ImageSaver::ConvertID(const int& ID)
{
	if (ID < 26)
		return std::string(1, ID + 'A');
	char curChar = ID % 26 + 'A';
	std::string curStr(1, curChar);	
	return ConvertID((double)ID / 26) + curStr;
}