#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <set>
#include <string>

#include "myThread.h"
#include "FullImage.h"
class ImageSelector;
class ROISelector;
class ImageGreper :
	public myThread
{
public:
	std::set<ImageSelector*> m_observers;
	std::set<ROISelector*> m_observers_ROI;
private:
	int m_iNumShot;
	int m_iDuration;
	std::string m_preTime;
public:
	ImageGreper();
	~ImageGreper();

	void Init(const int& numShot,
		const std::set<ImageSelector*>& observers);

	//Override
	void ThreadMain();

	HRESULT SavePixelsToFile32bppPBGRA(UINT width, UINT height, UINT stride, LPBYTE pixels, LPWSTR filePath, const GUID &format);
	HRESULT Direct3D9TakeScreenshots(UINT adapter, UINT count);
private:	
	void ConvertImage(const UINT& width, const UINT& height, const UINT& stride, const LPBYTE& pixels, const std::string& timeStamp);

	void UpdateObserver(boost::shared_ptr<FullImage> ptr);
};

