// ScreenMonitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ScreenMonitor.h"
#include "ImageGreper.h"
#include "ImageSelector.h"
#include "ImageSaver.h"
#include "ROISelector.h"
#include "ParsingCMD.h"

#include <vector>
#include <set>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	if (argc < 7)
	{
		nRetCode = 1;
		std::cout << "Error:\n"
			<< "Usage: ScreenMonitor.exe -p NumberOfThread -t Threshold(percentage, mask is 3x3) -n photoPerSec\n";
		return nRetCode;
	}
	
	ParsingCMD cmdPar;
	cmdPar.Set("-p"); cmdPar.Set("-t"); cmdPar.Set("-n");
	cmdPar.DoParsing(argc, argv);
	double threshold = std::stod(cmdPar["-t"]); // threshold is percentage
	int numOfThread = std::stod(cmdPar["-p"]);
	int numOfPhoto = std::stod(cmdPar["-n"]);

	

	// Get ROIs
	ImageGreper imgGrp;
	ROISelector ROISel;
	imgGrp.m_observers_ROI.insert(&ROISel);
	imgGrp.Direct3D9TakeScreenshots(0, 1);
	imgGrp.m_observers_ROI.erase(&ROISel);
	ROISel.StartGetROI();
	// Get ROIs end

	std::set<ImageSelector*> selectors;
	std::set<ImageSaver*> savers;

	for (int i = 0; i < numOfThread; i++)
	{
		ImageSelector* pSel = new ImageSelector;
		ImageSaver* pSav = new ImageSaver;

		selectors.insert(pSel);
		savers.insert(pSav);

		std::vector<cv::Rect> ROIs;
		std::vector<int> IDs;
		for (int j = 0; j < ROISel.m_ROIs.size(); j++)
		{
			if (j % ROISel.m_ROIs.size() == i)
			{
				ROIs.push_back(ROISel.m_ROIs[j]);
				IDs.push_back(ROISel.m_IDs[j]);
			}
		}
		std::set<ImageSaver*> saversTemp;
		saversTemp.insert(pSav);
		pSel->Init(ROIs, IDs, threshold, saversTemp);
		pSav->Init(".\\");

		pSav->Start();
		pSel->Start();
	}
	imgGrp.Init(numOfPhoto, selectors);

	imgGrp.Start();
	imgGrp.m_t->join();

	return nRetCode;
}
