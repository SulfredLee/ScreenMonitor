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

//class Foo
//{
//public:
//	Foo();
//	~Foo();
//
//	void PrintFoo()
//	{
//		std::cout << "Foo\n";
//	}
//private:
//
//};
//
//Foo::Foo()
//{
//}
//
//Foo::~Foo()
//{
//}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;


	//CThreadPool_Simple_Std testPool;
	//Foo foo;
	//testPool.submit(std::bind(&Foo::PrintFoo, foo));

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
	double dThreshold = std::stod(cmdPar["-t"]); // threshold is percentage
	int nNumOfThread = std::stoi(cmdPar["-p"]);
	int nNumOfPhoto = std::stoi(cmdPar["-n"]);

	std::shared_ptr<CThreadPool_Simple_Std> shr_ptrThreadPool(std::make_shared<CThreadPool_Simple_Std>());

	CImageGreper ImageGreper;
	ImageGreper.InitComponent(CImageGreper_Config(nNumOfPhoto,
		shr_ptrThreadPool));
	// Select region of interest
	CROISelector ROISelector;
	std::vector<cv::Rect> vecROIs;
	std::vector<int> vecIDs;
	ROISelector.GetROI(ImageGreper.TakeAScreenShot(), vecROIs, vecIDs);

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
	ImageGreper.AddObserver(ImageSelector_TaskMaker);
	ImageSelector_TaskMaker->AddObserver(ImageSaver);

	// Start application
	ImageGreper.StartThread();


	while (true)
	{
		Sleep(1000);
	}
	return nRetCode;
}
