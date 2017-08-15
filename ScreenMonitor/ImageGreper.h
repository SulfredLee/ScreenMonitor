#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <thread>

#include "Image_DataLine.h"

struct CImageGreper_Config
{
	// Get from outside
	int nGreperID;
	// Generate from inside

	CImageGreper_Config()
	{}
	CImageGreper_Config(int nGreperIDIN)
		: nGreperID(nGreperIDIN)
	{}
};

class CImageGreper
{
protected:
	CImageGreper_Config m_Config;
	std::vector<std::shared_ptr<CImage_DataLine> > m_vecObservers;
	std::string m_strPreviousTime;
	std::atomic<bool> m_atombThreadRun;
	cv::Mat m_matImageForOneShot; // this image is used when user request one screen shot
	int m_nCaptureCounter; // used to calculate the fps
public:
	CImageGreper();
	virtual ~CImageGreper();

	bool InitComponent(CImageGreper_Config&& ConfigIN);

	void StopThread();
	void StartThread();
	bool IsThreadRun();

	void AddObserver(std::shared_ptr<CImage_DataLine> shr_ptrObserver);

	virtual cv::Mat TakeAScreenShot() = 0;
protected:
	virtual void ThreadMain() = 0;

	void UpdateObserver(cv::Mat matImage, const std::string& strTime);
};

