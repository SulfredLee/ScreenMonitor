#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <string>
#include <memory>
#include <atomic>

#include "Image_DataLine.h"
#include "ThreadPool_Simple_Std.h"
struct CImageGreper_Config
{
	// Get from outside
	int nNumShot;
	std::shared_ptr<CThreadPool_Simple_Std> shr_ptrThreadPool;
	// Generate from inside
	int nDuration;

	CImageGreper_Config()
	{}
	CImageGreper_Config(int nNumShotIN,
		std::shared_ptr<CThreadPool_Simple_Std>& sh_ptrThreadPoolIN)
		: nNumShot(nNumShotIN),
		shr_ptrThreadPool(sh_ptrThreadPoolIN)
	{}
};
class CImageGreper
{
private:
	CImageGreper_Config m_Config;
	std::vector<std::shared_ptr<CImage_DataLine> > m_vecObservers;
	std::string m_strPreviousTime;
	std::atomic<bool> m_atombThreadRun;
	cv::Mat m_matImageForOneShot; // this image is used when user request one screen shot
public:
	CImageGreper();
	~CImageGreper();

	bool InitComponent(CImageGreper_Config&& ConfigIN);

	void StopThread();
	void StartThread();
	bool IsThreadRun();

	void AddObserver(std::shared_ptr<CImage_DataLine> shr_ptrObserver);
	cv::Mat TakeAScreenShot();
private:
	void ThreadMain();

	cv::Mat ConvertImage(const UINT& unWidth,
		const UINT& unHeight,
		const UINT& unStride,
		const LPBYTE& pbPixels);
	HRESULT SavePixelsToFile32bppPBGRA(UINT width,
		UINT height,
		UINT stride,
		LPBYTE pixels,
		LPWSTR filePath,
		const GUID &format);
	HRESULT Direct3D9TakeScreenshots(UINT adapter, UINT count, bool bUpdateObservers = true);
	void UpdateObserver(cv::Mat matImage, const std::string& strTime);
};

