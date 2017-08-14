#pragma once
#include "ImageGreper.h"
class CImageGreper_Win32MultipleScreen :
	public CImageGreper
{
private:
	int m_nNumOfMons;
	int m_nCurMonitor;
	std::atomic<bool> m_bOneScreenCaptureReady;
public:
	CImageGreper_Win32MultipleScreen();
	~CImageGreper_Win32MultipleScreen();

	void AddMonitor();
	cv::Mat TakeAScreenShot() override;

	void TakeScreenshots(_In_  HMONITOR hMonitor,
		_In_  HDC DevC,
		_In_  LPRECT lprcMonitor,
		_In_  LPARAM dwData,
		bool bUpdateObservers = true);
private:
	void ThreadMain() override;
	//void CountMonitors(_In_  HMONITOR hMonitor,
	//	_In_  HDC DevC,
	//	_In_  LPRECT lprcMonitor,
	//	_In_  LPARAM dwData);	
};

