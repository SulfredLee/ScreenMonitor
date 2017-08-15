#include "stdafx.h"
#include "ImageGreper_Win32MultipleScreen.h"

BOOL CALLBACK CountMonitorsProxy(_In_  HMONITOR hMonitor,
	_In_  HDC DevC,
	_In_  LPRECT lprcMonitor,
	_In_  LPARAM dwData)
{
	CImageGreper_Win32MultipleScreen* pData = (CImageGreper_Win32MultipleScreen*)dwData;
	pData->AddMonitor();
	return TRUE;
}

BOOL CALLBACK TakeScreenshotsProxy(_In_  HMONITOR hMonitor,
	_In_  HDC DevC,
	_In_  LPRECT lprcMonitor,
	_In_  LPARAM dwData)
{
	CImageGreper_Win32MultipleScreen* pData = (CImageGreper_Win32MultipleScreen*)dwData;
	pData->TakeScreenshots(hMonitor,
		DevC,
		lprcMonitor,
		dwData);
	return TRUE;
}

BOOL CALLBACK TakeAScreenshotProxy(_In_  HMONITOR hMonitor,
	_In_  HDC DevC,
	_In_  LPRECT lprcMonitor,
	_In_  LPARAM dwData)
{
	CImageGreper_Win32MultipleScreen* pData = (CImageGreper_Win32MultipleScreen*)dwData;
	pData->TakeScreenshots(hMonitor,
		DevC,
		lprcMonitor,
		dwData,
		false);
	return TRUE;
}

CImageGreper_Win32MultipleScreen::CImageGreper_Win32MultipleScreen()
{
	m_nNumOfMons = 0;
	m_nCurMonitor = 0;
	HDC DevC = GetDC(NULL);
	BOOL b = EnumDisplayMonitors(DevC, NULL, CountMonitorsProxy, (LPARAM)this);
	Sleep(1000); // wait to finish monitor counting
}


CImageGreper_Win32MultipleScreen::~CImageGreper_Win32MultipleScreen()
{
}

// override
void CImageGreper_Win32MultipleScreen::ThreadMain()
{
	while (m_atombThreadRun)
	{
		HDC DevC = GetDC(NULL);
		EnumDisplayMonitors(DevC, NULL, TakeScreenshotsProxy, (LPARAM)this);
	}
}

void CImageGreper_Win32MultipleScreen::AddMonitor()
{
	m_nNumOfMons++;
}

// override
cv::Mat CImageGreper_Win32MultipleScreen::TakeAScreenShot()
{
	m_bOneScreenCaptureReady = false;
	HDC DevC = GetDC(NULL);
	EnumDisplayMonitors(DevC, NULL, TakeAScreenshotProxy, (LPARAM)this);
	while (!m_bOneScreenCaptureReady)
	{
		Sleep(1);
	}
	return m_matImageForOneShot;
}

void CImageGreper_Win32MultipleScreen::TakeScreenshots(_In_  HMONITOR hMonitor,
	_In_  HDC DevC,
	_In_  LPRECT lprcMonitor,
	_In_  LPARAM dwData,
	bool bUpdateObservers)
{
	//char*BmpName;

	//if (screenCounter == 1){
	//	BmpName = "1 screen.bmp";
	//}
	//else {
	//	BmpName = "2 screen.bmp";
	//}

	MONITORINFO  info;
	info.cbSize = sizeof(MONITORINFO);

	BOOL monitorInfo = GetMonitorInfo(hMonitor, &info);

	if (monitorInfo) {

		DWORD Width = info.rcMonitor.right - info.rcMonitor.left;
		DWORD Height = info.rcMonitor.bottom - info.rcMonitor.top;

		DWORD FileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBTRIPLE) + 1 * (Width*Height * 4));
		char *BmpFileData = (char*)GlobalAlloc(0x0040, FileSize);

		PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER)BmpFileData;
		PBITMAPINFOHEADER  BInfoHeader = (PBITMAPINFOHEADER)&BmpFileData[sizeof(BITMAPFILEHEADER)];

		BFileHeader->bfType = 0x4D42; // BM
		BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
		BFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		BInfoHeader->biPlanes = 1;
		BInfoHeader->biBitCount = 24;
		BInfoHeader->biCompression = BI_RGB;
		BInfoHeader->biHeight = Height;
		BInfoHeader->biWidth = Width;

		RGBTRIPLE *Image = (RGBTRIPLE*)&BmpFileData[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];
		RGBTRIPLE color;

		HDC CaptureDC = CreateCompatibleDC(DevC);
		HBITMAP CaptureBitmap = CreateCompatibleBitmap(DevC, Width, Height);
		SelectObject(CaptureDC, CaptureBitmap);
		BitBlt(CaptureDC, 0, 0, Width, Height, DevC, info.rcMonitor.left, info.rcMonitor.top, SRCCOPY | CAPTUREBLT);
		GetDIBits(CaptureDC, CaptureBitmap, 0, Height, Image, (LPBITMAPINFO)BInfoHeader, DIB_RGB_COLORS);

		//DWORD Junk;
		//HANDLE FH = CreateFileA(BmpName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		//WriteFile(FH, BmpFileData, FileSize, &Junk, 0);
		//CloseHandle(FH);
		if (m_nCurMonitor == 0)
		{
			m_matImageForOneShot.release();
			cv::Mat(Height, Width, CV_8UC3, (unsigned*)BmpFileData).copyTo(m_matImageForOneShot);
			m_nCurMonitor++;
		}
		else if (m_nCurMonitor < m_nNumOfMons)
		{
			// Get image from bit map
			cv::Mat matTempImg;
			cv::Mat(Height, Width, CV_8UC3, (unsigned*)BmpFileData).copyTo(matTempImg);
			//m_matImageForOneShot.push_back(matTempImg);
			cv::hconcat(m_matImageForOneShot, matTempImg, m_matImageForOneShot);
			m_nCurMonitor++;
		}

		if (m_nCurMonitor == m_nNumOfMons)
		{
			// FPS calculating
			SYSTEMTIME st;
			GetSystemTime(&st); // measure the time we spend doing <count> captures	
			char temp[1024];
			memset(temp, 0, 1024);
			sprintf_s(temp, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);
			if (m_strPreviousTime != temp)
			{
				wprintf(L"Greper: %i FPS: %i\n", m_Config.nGreperID, m_nCaptureCounter);
				m_nCaptureCounter = 0;
			}
			m_strPreviousTime = temp;
			memset(temp, 0, 1024);
			m_nCaptureCounter++;
			
			// Reset monitor counting
			m_nCurMonitor = 0;

			cv::flip(m_matImageForOneShot, m_matImageForOneShot, 0); // flipping since the image is up side down
			if (bUpdateObservers)
			{
				sprintf_s(temp, "%04d%02d%02d_%02d%02d%02d_%04d", st.wYear, st.wMonth, st.wDay,
					st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
				std::string strTimeStamp(temp);

				UpdateObserver(m_matImageForOneShot, strTimeStamp);
			}
			else
			{
				m_bOneScreenCaptureReady = true;
			}
		}
		GlobalFree(BmpFileData);
	}
}