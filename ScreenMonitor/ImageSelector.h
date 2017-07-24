#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
//#include <boost/lockfree/queue.hpp>


#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <map>
#include <set>

#include "myThread.h"
#include "blockingQ.h"
#include "FullImage.h"
#include "CroppedImage.h"

class ImageSaver;
class ImageSelector :
	public myThread
{
public:
	enum msgType
	{
		FULLIMG
	};
	struct SelectorProfile
	{
		cv::Rect m_ROI;
		int m_ID;
		cv::Mat m_preImg;
		cv::Mat m_curImg;
	};
	std::vector<cv::Rect> m_ROIs;
	std::vector<int> m_IDs;
	std::vector<int> m_imageCode;
	std::set<ImageSaver*> m_observers;
	//boost::lockfree::queue<boost::shared_ptr<FullImage> > m_imageQ;
	blockingQ<boost::shared_ptr<FullImage> > m_imageQ;
	blockingQ<msgType> m_msgQ;
	std::map<int, SelectorProfile> m_mapProfiles; // Key: ID, Value: SelectorProfile
	double m_threshold;
public:
	ImageSelector();
	~ImageSelector();

	bool Init(const std::vector<cv::Rect>& ROIs,
		const std::vector<int>& IDs,
		const double threshold,
		const std::set<ImageSaver*>& observers);

	//Override
	void ThreadMain();

	//Callback
	void ImgSelector_Dataline(boost::shared_ptr<FullImage> ptr);
private:
	void ProcessSelection();
	bool ImgChanged(const cv::Mat& preImg, const cv::Mat& curImg, const double& threshold);
	void UpdateObserver(boost::shared_ptr<CroppedImage> ptr);
};

