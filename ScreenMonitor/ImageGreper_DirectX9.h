#pragma once
#include "ImageGreper.h"

class CImageGreper_DirectX9 :
	public CImageGreper
{
public:
	CImageGreper_DirectX9();
	~CImageGreper_DirectX9();

	cv::Mat TakeAScreenShot() override;
private:
	void ThreadMain() override;

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
};

