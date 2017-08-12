#include "stdafx.h"
#include "ImageSelector_TaskMaker.h"


CImageSelector_TaskMaker::CImageSelector_TaskMaker()
{
}


CImageSelector_TaskMaker::~CImageSelector_TaskMaker()
{
}

bool CImageSelector_TaskMaker::InitComponent(CImageSelector_TaskMaker_Config&& ConfigIN)
{
	m_Config = std::move(ConfigIN);

	return true;
}

void CImageSelector_TaskMaker::AddObserver(const std::shared_ptr<CChangedImage_DataLine>& shr_ptrObserver)
{
	m_vecObservers.emplace_back(shr_ptrObserver);
}

// override
void CImageSelector_TaskMaker::SendImageWithTime(cv::Mat matImage, const std::string& strTime)
{
	for (size_t i = 0; i < m_Config.vecROIs.size(); i++)
	{
		cv::Mat matPreSelectedImage;
		cv::Mat matCurSelectedImage;
		matImage(m_Config.vecROIs[i]).copyTo(matCurSelectedImage);
		if (!m_Config.matPreviousImage.empty())
		{
			m_Config.matPreviousImage(m_Config.vecROIs[i]).copyTo(matPreSelectedImage);
		}
		else
		{
			matImage(m_Config.vecROIs[i]).copyTo(matPreSelectedImage);
		}

		std::shared_ptr<CImageChecker_Task> shr_ptrImageChecker(std::make_shared<CImageChecker_Task>());
		shr_ptrImageChecker->InitComponent(CImageChecker_Task_Config(matPreSelectedImage,
			matCurSelectedImage,
			m_Config.dThreshold,
			strTime,
			m_Config.vecIDs[i],
			m_vecObservers));
		m_Config.shr_ptrThreadPool->submit(
			std::bind(&CImageChecker_Task::StartChecking, *shr_ptrImageChecker));// bind will make a copy of object CImageChecker
	}
	matImage.copyTo(m_Config.matPreviousImage);
}
