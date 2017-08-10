#include "stdafx.h"
#include "ROISelector.h"


CROISelector::CROISelector()
{
	m_curID = 0;
	m_dImageFactor = 1.5;
}


CROISelector::~CROISelector()
{
}

void CROISelector::MouseHandlerProxy(int event, int x, int y, int flags, void* param)
{
	CROISelector* ptr = (CROISelector*)param;
	ptr->MouseHandler(event, x, y, flags);
}
void CROISelector::MouseHandler(int event, int x, int y, int flags)
{
	/* user press left button */
	if (event == CV_EVENT_LBUTTONDOWN && !m_drag)
	{
		m_point = cvPoint(x, y);
		m_drag = 1;
	}
	/* user drag the mouse */
	if (event == CV_EVENT_MOUSEMOVE && m_drag)
	{		
		m_selectedFrame.copyTo(m_midFrame);
		m_rect = cvRect(m_point.x, m_point.y, x - m_point.x, y - m_point.y);
		cv::rectangle(m_midFrame, m_rect, cv::Scalar(255, 0, 0));
		cv::imshow("result", m_midFrame);
	}
	/* user release left button */
	if (event == CV_EVENT_LBUTTONUP && m_drag)
	{
		m_rect = cvRect(m_point.x, m_point.y, x - m_point.x, y - m_point.y);
		cv::rectangle(m_selectedFrame, m_rect, cv::Scalar(255, 255, 255));
		cv::imshow("result", m_selectedFrame);
		m_selectedFrame.copyTo(m_midFrame);
		m_drag = 0;
		//get the original size
		m_rect.x *= m_dImageFactor;
		m_rect.y *= m_dImageFactor;
		m_rect.height *= m_dImageFactor;
		m_rect.width *= m_dImageFactor;
		//get the original size [end]
		m_vecROIs.push_back(m_rect);
		m_vecIDs.push_back(m_curID++);
	}

	/* user click right button: reset all */
	if (event == CV_EVENT_RBUTTONUP)
	{
		m_drag = 0;
	}
}

int CROISelector::GetROI(cv::Mat matImage, std::vector<cv::Rect>& vecROIs, std::vector<int>& vecIDs)
{	
	int newCol, newRow;
	newCol = static_cast<int>(matImage.cols / m_dImageFactor);
	newRow = static_cast<int>(matImage.rows / m_dImageFactor);
	cv::resize(matImage, m_orgFrame, cv::Size(newCol, newRow));
	cv::resize(matImage, m_selectedFrame, cv::Size(newCol, newRow));
	cv::resize(matImage, m_midFrame, cv::Size(newCol, newRow));

	while (m_key != 'q')
	{
		cvSetMouseCallback("result", CROISelector::MouseHandlerProxy, this);
		m_key = cvWaitKey(10);
		if ((char)m_key == 'r')
		{ 
			m_rect = cvRect(0, 0, 0, 0); 
			m_orgFrame.copyTo(m_selectedFrame);
			m_selectedFrame.copyTo(m_midFrame);
			m_vecROIs.clear();
			m_vecIDs.clear();
			m_curID = 0;
		}
		cv::imshow("result", m_midFrame);
	}
	cvDestroyWindow("result");
	
	vecROIs = m_vecROIs;
	vecIDs = m_vecIDs;
	return 0;
}