#include "stdafx.h"
#include "ImageChecker_Task.h"


CImageChecker_Task::CImageChecker_Task()
{
}


CImageChecker_Task::~CImageChecker_Task()
{
}

bool CImageChecker_Task::InitComponent(CImageChecker_Task_Config&& ConfigIN)
{
	m_Config = std::move(ConfigIN);
	return true;
}

// Run by thread
void CImageChecker_Task::StartChecking()
{
	if (IsImgChanged(m_Config.matPreviousImage,
		m_Config.matCurrentImage,
		m_Config.dThreshold))
	{
		// update observers
		for (auto it = m_Config.vecObservers.begin(); it != m_Config.vecObservers.end(); it++)
		{
			(*it)->SendChangedImageWithTime_ID(m_Config.matCurrentImage, m_Config.strTime, m_Config.nID);
		}
	}
	m_Config.shr_ptrImageChecker_Task.reset();
}

bool CImageChecker_Task::IsImgChanged(const cv::Mat preImg,
	const cv::Mat curImg,
	const double& threshold)
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