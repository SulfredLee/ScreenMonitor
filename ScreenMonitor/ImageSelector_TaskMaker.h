#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <memory>

#include "Image_DataLine.h"
#include "ThreadPool_Simple_Std.h"
#include "ImageChecker_Task.h"
#include "ChangedImage_DataLine.h"
struct CImageSelector_TaskMaker_Config
{
	// Get from outside
	std::vector<cv::Rect> vecROIs;
	std::vector<int> vecIDs;
	double dThreshold;
	std::shared_ptr<CThreadPool_Simple_Std> shr_ptrThreadPool;
	// Generate from inside
	cv::Mat matPreviousImage;

	CImageSelector_TaskMaker_Config()
	{}
	CImageSelector_TaskMaker_Config(std::vector<cv::Rect>& vecROIsIN,
		std::vector<int>& vecIDsIN,
		const double& dThresholdIN,
		std::shared_ptr<CThreadPool_Simple_Std> shr_ptrThreadPoolIN)
		: vecROIs(vecROIsIN),
		vecIDs(vecIDsIN),
		dThreshold(dThresholdIN),
		shr_ptrThreadPool(shr_ptrThreadPoolIN)
	{}
};
class CImageSelector_TaskMaker
	: public CImage_DataLine
{
private:
	CImageSelector_TaskMaker_Config m_Config;
	std::vector<std::shared_ptr<CChangedImage_DataLine> > m_vecObservers;
public:
	CImageSelector_TaskMaker();
	~CImageSelector_TaskMaker();

	bool InitComponent(CImageSelector_TaskMaker_Config&& ConfigIN);
	void AddObserver(const std::shared_ptr<CChangedImage_DataLine>& shr_ptrObserver);

	// override
	void SendImageWithTime(cv::Mat matImage, const std::string& strTime);
};

