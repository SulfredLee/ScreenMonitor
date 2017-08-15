// ScreenMonitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ScreenMonitor.h"

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
			<< "Usage: ScreenMonitor.exe -m [Single|Multi]-t Threshold(percentage, mask is 3x3) -n NumOfPipeLine\n";
		return nRetCode;
	}

	ParsingCMD cmdPar;
	cmdPar.Set("-t"); cmdPar.Set("-m"); cmdPar.Set("-n");
	cmdPar.DoParsing(argc, argv);
	double dThreshold = std::stod(cmdPar["-t"]); // threshold is percentage
	int nNumOfPipeline = std::stoi(cmdPar["-n"]);
	bool bIsSingleMon = cmdPar["-m"] == "Single" ? true : false;

	if (nNumOfPipeline == 0)
	{
		nRetCode = 1;
		std::cout << "Error:\n"
			<< "Usage: Number of Pipeline cannot be zero\n";
		return nRetCode;
	}

	std::shared_ptr<CThreadPool_Simple_Std> shr_ptrThreadPool(std::make_shared<CThreadPool_Simple_Std>());
	std::vector<std::shared_ptr<CImageGreper> > vecImageGrepers;
	std::vector<std::shared_ptr<CImageSelector_TaskMaker> > vecImageSelector_TaskMakers;
	std::vector<std::shared_ptr<CImageSaver> > vecImageSavers;


	// Handle Image Grepers
	for (int i = 0; i < nNumOfPipeline; i++)
	{
		if (bIsSingleMon)
		{
			vecImageGrepers.emplace_back(std::make_shared<CImageGreper_DirectX9>());
		}
		else
		{
			vecImageGrepers.emplace_back(std::make_shared<CImageGreper_Win32MultipleScreen>());
		}
		vecImageGrepers.back()->InitComponent(CImageGreper_Config(i + 1));
	}
	
	// Select region of interest
	CROISelector ROISelector;
	std::vector<cv::Rect> vecROIs;
	std::vector<int> vecIDs;
	ROISelector.GetROI(vecImageGrepers.front()->TakeAScreenShot(), vecROIs, vecIDs);	


	for (int i = 0; i < nNumOfPipeline; i++)
	{
		// Handle Image update checking
		vecImageSelector_TaskMakers.emplace_back(std::make_shared<CImageSelector_TaskMaker>());
		vecImageSelector_TaskMakers.back()->InitComponent(CImageSelector_TaskMaker_Config(vecROIs,
			vecIDs,
			dThreshold,
			shr_ptrThreadPool));

		// Handle Image saver
		vecImageSavers.emplace_back(std::make_shared<CImageSaver>());
		vecImageSavers.back()->InitComponent(CImageSaver_Config(".\\"));

		// Object connection
		vecImageGrepers[i]->AddObserver(vecImageSelector_TaskMakers[i]);
		vecImageSelector_TaskMakers[i]->AddObserver(vecImageSavers[i]);		
	}

	// Start application
	for (int i = 0; i < nNumOfPipeline; i++)
	{
		vecImageGrepers[i]->StartThread();
	}

	while (true)
	{
		Sleep(1000);
	}
	return nRetCode;
}
