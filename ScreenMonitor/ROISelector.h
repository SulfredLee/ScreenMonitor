#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
class CROISelector
{
private:
	cv::Mat m_orgFrame, m_selectedFrame, m_midFrame;
	cv::Point m_point;
	int m_drag = 0;
	int m_key = 0;
	cv::Rect m_rect;
	int m_curID;
	std::vector<cv::Rect> m_vecROIs;
	std::vector<int> m_vecIDs;
	double m_dImageFactor;
public:
	CROISelector();
	~CROISelector();

	int GetROI(cv::Mat matImage, std::vector<cv::Rect>& vecROIs, std::vector<int>& vecIDs);
private:
	static void MouseHandlerProxy(int event, int x, int y, int flags, void* param);
	void MouseHandler(int event, int x, int y, int flags);
};

