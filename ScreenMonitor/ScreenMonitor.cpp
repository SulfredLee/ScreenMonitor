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

	if (argc < 5)
	{
		nRetCode = 1;
		std::cout << "Error:\n"
			<< "Usage: ScreenMonitor.exe -m [Single|Multi]-t Threshold(percentage, mask is 3x3)\n";
		return nRetCode;
	}

	ParsingCMD cmdPar;
	cmdPar.Set("-t"); cmdPar.Set("-m");
	cmdPar.DoParsing(argc, argv);
	double dThreshold = std::stod(cmdPar["-t"]); // threshold is percentage
	bool bIsSingleMon = cmdPar["-m"] == "Single" ? true : false;

	std::shared_ptr<CThreadPool_Simple_Std> shr_ptrThreadPool(std::make_shared<CThreadPool_Simple_Std>());

	//std::shared_ptr<CImageGreper_DirectX9> ImageGreper(std::make_shared<CImageGreper_DirectX9>());
	//std::shared_ptr<CImageGreper_Win32> ImageGreper(std::make_shared<CImageGreper_Win32>());
	//std::shared_ptr<CImageGreper_Win32MultipleScreen> ImageGreper(std::make_shared<CImageGreper_Win32MultipleScreen>());
	std::shared_ptr<CImageGreper> ImageGreper;
	if (bIsSingleMon)
	{
		ImageGreper = std::make_shared<CImageGreper_DirectX9>();
	}
	else
	{
		ImageGreper = std::make_shared<CImageGreper_Win32MultipleScreen>();
	}
	ImageGreper->InitComponent(CImageGreper_Config(1));
	// Select region of interest
	CROISelector ROISelector;
	std::vector<cv::Rect> vecROIs;
	std::vector<int> vecIDs;
	ROISelector.GetROI(ImageGreper->TakeAScreenShot(), vecROIs, vecIDs);

	// Image update checking
	std::shared_ptr<CImageSelector_TaskMaker> ImageSelector_TaskMaker(std::make_shared<CImageSelector_TaskMaker>());
	ImageSelector_TaskMaker->InitComponent(CImageSelector_TaskMaker_Config(vecROIs,
		vecIDs,
		dThreshold,
		shr_ptrThreadPool));

	// Handle Image saver
	std::shared_ptr<CImageSaver> ImageSaver(std::make_shared<CImageSaver>());
	ImageSaver->InitComponent(CImageSaver_Config(".\\"));

	// Object connection
	ImageGreper->AddObserver(ImageSelector_TaskMaker);
	ImageSelector_TaskMaker->AddObserver(ImageSaver);

	// Start application
	ImageGreper->StartThread();


	while (true)
	{
		Sleep(1000);
	}
	return nRetCode;
}
