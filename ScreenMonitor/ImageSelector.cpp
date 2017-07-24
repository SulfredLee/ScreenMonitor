#include "stdafx.h"
#include "ImageSelector.h"
#include "ImageSaver.h"


ImageSelector::ImageSelector()
{
}


ImageSelector::~ImageSelector()
{
}

bool ImageSelector::Init(const std::vector<cv::Rect>& ROIs,
	const std::vector<int>& IDs,
	const double threshold,
	const std::set<ImageSaver*>& observers)
{
	if (ROIs.size() != IDs.size())
		return false;
	for (int i = 0; i < ROIs.size(); i++)
	{
		SelectorProfile tempProfile;
		tempProfile.m_ROI = ROIs[i];
		tempProfile.m_ID = IDs[i];
		m_mapProfiles.insert(std::pair<int, SelectorProfile>(IDs[i], tempProfile));
	}

	m_threshold = threshold;
	m_observers = observers;

	return true;
}

//Override
void ImageSelector::ThreadMain()
{
	while (!myThread::IsEndThread())
	{
		msgType curMsgType;
		m_msgQ.wait_and_pop(curMsgType);
		switch (curMsgType)
		{
		case FULLIMG:
			ProcessSelection();
			break;
		default:
			break;
		}
	}
}

//Callback
void ImageSelector::ImgSelector_Dataline(boost::shared_ptr<FullImage> ptr)
{
	m_imageQ.push(ptr);
	m_msgQ.push(FULLIMG);
}

void ImageSelector::ProcessSelection()
{
	boost::shared_ptr<FullImage> curFullImg;
	
	if (!m_imageQ.try_pop(curFullImg))
		return;

	for (std::map<int, SelectorProfile>::iterator it = m_mapProfiles.begin();
		it != m_mapProfiles.end(); it++)
	{
		if (it->second.m_preImg.size().area() > 0)
			it->second.m_preImg.release();
		it->second.m_curImg.copyTo(it->second.m_preImg);
		curFullImg->m_image(it->second.m_ROI).copyTo(it->second.m_curImg);
		

		if (ImgChanged(it->second.m_preImg, it->second.m_curImg, m_threshold))
		{
			//Send the changed image to imageSaver
			boost::shared_ptr<CroppedImage> tempP(new CroppedImage);
			it->second.m_curImg.copyTo(tempP->m_img);
			tempP->m_ID = it->second.m_ID;
			tempP->m_timeStamp = curFullImg->m_timeStamp;
			UpdateObserver(tempP);
		}
	}
}

void ImageSelector::UpdateObserver(boost::shared_ptr<CroppedImage> ptr)
{
	for (std::set<ImageSaver*>::iterator it = m_observers.begin(); it != m_observers.end(); it++)
	{
		(*it)->ImageSaver_DataLine(ptr);
	}
}

bool ImageSelector::ImgChanged(const cv::Mat& preImg, const cv::Mat& curImg, const double& threshold)
{
	if (preImg.size().area() <= 0)
		return false;
	cv::Mat grayPreImg, grayCurImg;
	cv::cvtColor(preImg, grayPreImg, CV_RGB2GRAY);
	cv::cvtColor(curImg, grayCurImg, CV_RGB2GRAY);

	//cv::imwrite("Test_001.jpg", grayPreImg);
	//cv::imwrite("Test_002.jpg", grayCurImg);
	

	cv::Mat imgDelta, dst;
	cv::absdiff(grayPreImg, grayCurImg, imgDelta);
	int kernel_size = 3;
	cv::Mat kernel = cv::Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size*kernel_size);
	int ddepth = -1;
	cv::Point anchor = cv::Point(-1, -1);
	double delta = 0;
	filter2D(imgDelta, dst, ddepth, kernel, anchor, delta, cv::BORDER_DEFAULT);
	double min, max;
	cv::minMaxLoc(dst, &min, &max);
	if ((max / 255 * 100) >= threshold)
		return true;
	else
		return false;
}