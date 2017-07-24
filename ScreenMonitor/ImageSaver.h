#pragma once
#include <boost/shared_ptr.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include "myThread.h"
#include "blockingQ.h"
#include "CroppedImage.h"
class ImageSaver :
	public myThread
{
public:
	enum msgType
	{
		IMG_IN
	};
	blockingQ<boost::shared_ptr<CroppedImage> > m_imageQ;
	blockingQ<msgType> m_msgQ;
	std::string m_outputPath;
public:
	ImageSaver();
	~ImageSaver();

	bool Init(const std::string& outputPath);
	//Override
	void ThreadMain();

	//Callback
	void ImageSaver_DataLine(boost::shared_ptr<CroppedImage> ptr);
private:
	void SaveImg();
	std::string ConvertID(const int& ID);
};

