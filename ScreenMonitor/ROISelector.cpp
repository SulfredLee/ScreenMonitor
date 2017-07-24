#include "stdafx.h"
#include "ROISelector.h"


ROISelector::ROISelector()
{
	m_curID = 0;
}


ROISelector::~ROISelector()
{
}

void ROISelector::MouseHandlerProxy(int event, int x, int y, int flags, void* param)
{
	ROISelector* ptr = (ROISelector*)param;
	ptr->MouseHandler(event, x, y, flags);
}
void ROISelector::MouseHandler(int event, int x, int y, int flags)
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
		m_rect.x *= 2;
		m_rect.y *= 2;
		m_rect.height *= 2;
		m_rect.width *= 2;
		//get the original size [end]
		m_ROIs.push_back(m_rect);
		m_IDs.push_back(m_curID++);
	}

	/* user click right button: reset all */
	if (event == CV_EVENT_RBUTTONUP)
	{
		m_drag = 0;
	}
}

int ROISelector::StartGetROI()
{	
	while (m_key != 'q')
	{
		cvSetMouseCallback("result", ROISelector::MouseHandlerProxy, this);
		m_key = cvWaitKey(10);
		if ((char)m_key == 'r')
		{ 
			m_rect = cvRect(0, 0, 0, 0); 
			m_orgFrame.copyTo(m_selectedFrame);
			m_selectedFrame.copyTo(m_midFrame);
			m_ROIs.clear();
			m_IDs.clear();
			m_curID = 0;
		}
		cv::imshow("result", m_midFrame);
	}
	cvDestroyWindow("result");
	return 0;
}

void ROISelector::ImgSelector_Dataline(boost::shared_ptr<FullImage> ptr)
{
	int newCol, newRow;
	newCol = ptr->m_image.cols / 2; newRow = ptr->m_image.rows / 2;
	cv::resize(ptr->m_image, m_orgFrame, cv::Size(newCol, newRow));
	cv::resize(ptr->m_image, m_selectedFrame, cv::Size(newCol, newRow));
	cv::resize(ptr->m_image, m_midFrame, cv::Size(newCol, newRow));
}