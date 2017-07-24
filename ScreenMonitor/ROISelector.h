#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <vector>

#include "FullImage.h"
class ROISelector
{
public:
	//IplImage * m_img1;
	cv::Mat m_orgFrame, m_selectedFrame, m_midFrame;
	cv::Point m_point;
	int m_drag = 0;
	int m_key = 0;
	cv::Rect m_rect;
	std::vector<cv::Rect> m_ROIs;
	std::vector<int> m_IDs;
	int m_curID;
public:
	ROISelector();
	~ROISelector();

	static void MouseHandlerProxy(int event, int x, int y, int flags, void* param);
	void MouseHandler(int event, int x, int y, int flags);
	int StartGetROI();

	void ImgSelector_Dataline(boost::shared_ptr<FullImage> ptr);
};

