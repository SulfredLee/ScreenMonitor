#include "stdafx.h"
#include "ImageGreper.h"

CImageGreper::CImageGreper()
{
	m_atombThreadRun = false;
	m_nCaptureCounter = 0;
}


CImageGreper::~CImageGreper()
{
}

bool CImageGreper::InitComponent(CImageGreper_Config&& ConfigIN)
{
	m_Config = std::move(ConfigIN);

	return true;
}

void CImageGreper::StopThread()
{
	m_atombThreadRun = false;
}

void CImageGreper::StartThread()
{
	m_atombThreadRun = true;
	std::thread(std::bind(&CImageGreper::ThreadMain, this)).detach();
}

bool CImageGreper::IsThreadRun()
{
	return m_atombThreadRun;
}

void CImageGreper::AddObserver(std::shared_ptr<CImage_DataLine> shr_ptrObserver)
{
	m_vecObservers.emplace_back(shr_ptrObserver);
}

void CImageGreper::UpdateObserver(cv::Mat matImage, const std::string& strTime)
{
	for (auto it = m_vecObservers.begin(); it != m_vecObservers.end(); it++)
	{
		(*it)->SendImageWithTime(matImage, strTime);
	}
}