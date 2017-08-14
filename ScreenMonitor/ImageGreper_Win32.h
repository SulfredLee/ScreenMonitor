#pragma once
#include "ImageGreper.h"
class CImageGreper_Win32 :
	public CImageGreper
{
public:
	CImageGreper_Win32();
	~CImageGreper_Win32();

	cv::Mat TakeAScreenShot() override;
private:
	void ThreadMain() override;

	int CaptureAnImage(HWND hWnd, bool bUpdateObservers = true);
};

