#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <memory>

#include "ChangedImage_DataLine.h"
class CImageChecker_Task;
struct CImageChecker_Task_Config
{
	// Get from outside
	cv::Mat matPreviousImage;
	cv::Mat matCurrentImage;
	double dThreshold;
	std::string strTime;
	int nID;
	std::vector<std::shared_ptr<CChangedImage_DataLine> > vecObservers;
	// Generate from inside

	CImageChecker_Task_Config(){}
	CImageChecker_Task_Config(cv::Mat matPreviousImageIN,
		cv::Mat matCurrentImageIN,
		double dThresholdIN,
		std::string strTimeIN,
		int nIDIN,
		const std::vector<std::shared_ptr<CChangedImage_DataLine> >& vecObserversIN) :
		matPreviousImage(matPreviousImageIN),
		matCurrentImage(matCurrentImageIN),
		dThreshold(dThresholdIN),
		strTime(strTimeIN),
		nID(nIDIN),
		vecObservers(vecObserversIN)
	{}
};
class CImageChecker_Task
{
private:
	CImageChecker_Task_Config m_Config;
public:
	CImageChecker_Task();
	~CImageChecker_Task();

	bool InitComponent(CImageChecker_Task_Config&& ConfigIN);

	// Run by thread
	void StartChecking();
private:
	bool IsImgChanged(const cv::Mat preImg,
		const cv::Mat curImg,
		const double& threshold);
};

